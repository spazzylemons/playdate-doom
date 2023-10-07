#define DOOM_IMPLEMENTATION
#include "PureDOOM.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

#define PRINT_BUFFER_SIZE 512

static PlaydateAPI *playdate;

static void print_fn(const char *str) {
    static char print_buffer[PRINT_BUFFER_SIZE];
    static int print_buffer_size = 0;

    for (;;) {
        char c = *str++;
        if (c == 0) {
            break;
        } else if (c == '\n') {
            playdate->system->logToConsole("%.*s", print_buffer_size, print_buffer);
            print_buffer_size = 0;
        } else if (print_buffer_size < PRINT_BUFFER_SIZE) {
            print_buffer[print_buffer_size++] = c;
        }
    }
}

static void *malloc_fn(int size) {
    if (size == 0) {
        size = 1;
    }
    return playdate->system->realloc(NULL, size);
}

static void free_fn(void *ptr) {
    if (ptr != NULL) {
        playdate->system->realloc(ptr, 0);
    }
}

static void *open_fn(const char *filename, const char *mode) {
    FileOptions options = strstr(mode, "w") ? kFileWrite : kFileRead;
    if (strstr(mode, "a")) options |= kFileAppend;
    return playdate->file->open(filename, options);
}

static void close_fn(void *handle) {
    playdate->file->close(handle);
}

static int read_fn(void *handle, void *buf, int count) {
    return playdate->file->read(handle, buf, count);
}

static int write_fn(void *handle, const void *buf, int count) {
    return playdate->file->write(handle, buf, count);
}

static int seek_fn(void *handle, int offset, doom_seek_t origin) {
    return playdate->file->seek(handle, offset, origin);
}

static int tell_fn(void *handle) {
    return playdate->file->tell(handle);
}

static int eof_fn(void *handle) {
    int old_pos = playdate->file->tell(handle);
    playdate->file->seek(handle, 1, SEEK_CUR);
    int new_pos = playdate->file->tell(handle);
    playdate->file->seek(handle, old_pos, SEEK_SET);
    return old_pos == new_pos;
}

// TODO pause menu might cause strange things to happen if we use this for timing...
static void gettime_fn(int *sec, int *usec) {
    unsigned int msec;
    unsigned int seconds = playdate->system->getSecondsSinceEpoch(&msec);
    if (sec != NULL) {
        *sec = seconds;
    }
    if (usec != NULL) {
        *usec = msec * 1000;
    }
}

static void exit_fn(int code) {
    // Can't actually exit using Playdate API. This is the best we can do
    while (true) {
        __builtin_trap();
    }
    __builtin_unreachable();
}

static char *getenv_fn(const char *var) {
    if (!strcmp(var, "HOME") || !strcmp(var, "DOOMWADDIR")) {
        return "/";
    } else {
        return NULL;
    }
}

// it's nearly unplayable this way, but playable enough for testing
static const doom_key_t keybinds_1[] = {
    DOOM_KEY_LEFT_ARROW,
    DOOM_KEY_RIGHT_ARROW,
    DOOM_KEY_UP_ARROW,
    DOOM_KEY_DOWN_ARROW,
    DOOM_KEY_SPACE,
    DOOM_KEY_CTRL,
};

static const doom_key_t keybinds_2[] = {
    DOOM_KEY_UNKNOWN,
    DOOM_KEY_UNKNOWN,
    DOOM_KEY_UNKNOWN,
    DOOM_KEY_UNKNOWN,
    DOOM_KEY_UNKNOWN,
    DOOM_KEY_ENTER,
};

static uint8_t brightness[256];

#define DitherPattern(a, b, c, d) { a * 17, b * 17, c * 17, d * 17, a * 17, b * 17, c * 17, d * 17 }

static const uint8_t shades[17][8] = {
    DitherPattern(0x0, 0x0, 0x0, 0x0),
    DitherPattern(0x8, 0x0, 0x0, 0x0),
    DitherPattern(0x8, 0x0, 0x2, 0x0),
    DitherPattern(0x8, 0x0, 0xa, 0x0),
    DitherPattern(0xa, 0x0, 0xa, 0x0),
    DitherPattern(0xa, 0x0, 0xa, 0x1),
    DitherPattern(0xa, 0x4, 0xa, 0x1),
    DitherPattern(0xa, 0x4, 0xa, 0x5),
    DitherPattern(0xa, 0x5, 0xa, 0x5),
    DitherPattern(0xd, 0x5, 0xa, 0x5),
    DitherPattern(0xd, 0x5, 0xe, 0x5),
    DitherPattern(0xd, 0x5, 0xf, 0x5),
    DitherPattern(0xf, 0x5, 0xf, 0x5),
    DitherPattern(0xf, 0x5, 0xf, 0xd),
    DitherPattern(0xf, 0x7, 0xf, 0xd),
    DitherPattern(0xf, 0x7, 0xf, 0xf),
    DitherPattern(0xf, 0xf, 0xf, 0xf),
};

static uint8_t masks[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

void recalculate_lighting(const uint8_t *palette) {
    for (int i = 0; i < 256; i++) {
        uint8_t red = *palette++;
        uint8_t green = *palette++;
        uint8_t blue = *palette++;
        brightness[i] = (uint8_t) (red * 0.299f + green * 0.587f + blue * 0.114f) / 15;
    }
}

static int update(void *userdata) {
    PDButtons pressed, released;
    playdate->system->getButtonState(NULL, &pressed, &released);
    for (int i = 0; i < 6; i++) {
        PDButtons mask = 1 << i;
        if (pressed & mask) {
            doom_key_down(keybinds_1[i]);
            doom_key_down(keybinds_2[i]);
        } else if (released & mask) {
            doom_key_up(keybinds_1[i]);
            doom_key_up(keybinds_2[i]);
        }
    }

    doom_update();
    const uint8_t *pal_buffer = doom_get_framebuffer(1);
    uint8_t *bit_buffer = playdate->graphics->getFrame() + (20 * 52);
    for (int y = 0; y < 200; y++) {
        for (int x = 5; x < 45; x++) {
            uint8_t pixel = 0;
            for (int px = 0; px < 8; px++) {
                pixel |= shades[brightness[*pal_buffer++]][y & 7] & masks[px];
            }
            bit_buffer[x] = pixel;
        }
        bit_buffer += 52;
    }
    playdate->graphics->markUpdatedRows(0, LCD_ROWS - 1);
    playdate->system->drawFPS(0, 0);

    return 1;
}

extern signed short mixbuffer_left[1024];
extern signed short mixbuffer_right[1024];

static int sound_callback(void* context, int16_t* left, int16_t* right, int len) {
    static int buffer_index = 2048;

    while (len > 0) {
        if (__builtin_expect(buffer_index == 2048, 0)) {
            I_UpdateSound();
            buffer_index = 0;
        }

        // stretch out audio to match the hardware 44.1 khz
        int index = buffer_index >> 2;
        *left++ = mixbuffer_left[index];
        *right++ = mixbuffer_right[index];
        --len;
        ++buffer_index;
    }

    return 1;
}

typedef struct {
    uint32_t pos;
    uint16_t volume;
    uint8_t note;
    uint8_t bank;
    bool held;
} Channel;

static const uint32_t incChart[128] = {
       3110,    3295,    3491,    3699,    3919,    4152,    4399,    4660,
       4937,    5231,    5542,    5872,    6221,    6591,    6983,    7398,
       7838,    8304,    8797,    9321,    9875,   10462,   11084,   11743,
      12441,   13181,   13965,   14795,   15675,   16607,   17595,   18641,
      19750,   20924,   22168,   23486,   24883,   26363,   27930,   29591,
      31351,   33215,   35190,   37282,   39499,   41848,   44336,   46973,
      49766,   52725,   55860,   59182,   62701,   66429,   70380,   74565,
      78998,   83696,   88673,   93945,   99532,  105450,  111721,  118364,
     125402,  132859,  140759,  149129,  157997,  167392,  177345,  187891,
     199063,  210900,  223441,  236728,  250804,  265718,  281518,  298258,
     315993,  334783,  354691,  375782,  398127,  421801,  446882,  473455,
     501608,  531436,  563036,  596516,  631987,  669567,  709381,  751563,
     796254,  843601,  893765,  946911, 1003217, 1062871, 1126073, 1193033,
    1263974, 1339134, 1418763, 1503127, 1592507, 1687203, 1787529, 1893821,
    2006434, 2125742, 2252146, 2386065, 2527948, 2678268, 2837526, 3006254,
    3185015, 3374406, 3575058, 3787642, 4012867, 4251485, 4504291, 4772130,
};

static int waves[16][16] = {
    {  9, 14, 14, 12, 10, 14,  5,  3,  8, 12,  7,  0,  5,  1,  0,  0 },
    {  9, 10, 11, 14, 14, 12, 14,  7,  8,  5,  1,  2,  0,  0,  5,  2 },
    { 13, 14, 12,  8, 10,  6,  4,  4, 11, 12,  8,  3,  5,  2,  1,  3 },
    { 10, 14, 13,  3,  0,  4,  5,  8, 11, 14, 14,  2,  6,  2,  5,  9 },
    {  9, 14, 10,  4,  1,  1,  2,  4,  8, 11, 13, 14, 13,  9,  2,  1 },
    { 11, 11,  7,  9, 14, 12,  6,  6,  9,  5,  0,  2,  7,  3,  3,  7 },

    {  9, 14, 14, 12, 10, 14,  5,  3,  8, 12,  7,  0,  5,  1,  0,  0 },
    {  9, 14, 14, 12, 10, 14,  5,  3,  8, 12,  7,  0,  5,  1,  0,  0 },
    {  9, 14, 14, 12, 10, 14,  5,  3,  8, 12,  7,  0,  5,  1,  0,  0 },
    {  9, 14, 14, 12, 10, 14,  5,  3,  8, 12,  7,  0,  5,  1,  0,  0 },
    {  9, 14, 14, 12, 10, 14,  5,  3,  8, 12,  7,  0,  5,  1,  0,  0 },
    {  9, 14, 14, 12, 10, 14,  5,  3,  8, 12,  7,  0,  5,  1,  0,  0 },
    {  9, 14, 14, 12, 10, 14,  5,  3,  8, 12,  7,  0,  5,  1,  0,  0 },
    {  9, 14, 14, 12, 10, 14,  5,  3,  8, 12,  7,  0,  5,  1,  0,  0 },
    {  9, 14, 14, 12, 10, 14,  5,  3,  8, 12,  7,  0,  5,  1,  0,  0 },
    {  9, 14, 14, 12, 10, 14,  5,  3,  8, 12,  7,  0,  5,  1,  0,  0 },
};

static Channel channels[8];

static int music_callback(void* context, int16_t* left, int16_t* right, int len) {
    static int samples_left_for_frame = 0;

    while (len > 0) {
        if (samples_left_for_frame == 0) {
            for (;;) {
                uint32_t midi = doom_tick_midi();
                if (midi == 0) break;

                uint8_t data1 = midi >> 8;
                uint8_t data2 = midi >> 16;
                uint8_t channel = midi & 0xf;
                uint8_t status = (midi >> 4) & 0xf;

                if (channel > 7) {
                    // might be percussion
                    continue;
                }

                switch (status) {
                    case 0x8: {
                        channels[channel].held = false;
                        break;
                    }

                    case 0x9: {
                        channels[channel].note = data1 & 0x7f;
                        channels[channel].pos = 0;
                        channels[channel].volume = data2 & 0x7c;
                        channels[channel].held = true;
                        break;
                    }

                    case 0xb: {
                        if (data1 == 120 || data1 == 123) {
                            for (int i = 0; i < 8; i++) {
                                channels[channel].held = false;
                                channels[channel].volume = 0;
                            }
                        }
                        break;
                    }

                    case 0xc: {
                        channels[channel].bank = data1 >> 4;
                    }
                }
            }

            for (int i = 0; i < 8; i++) {
                if (channels[i].volume > 0 && !channels[i].held) {
                    channels[i].volume -= 4;
                }
            }
            samples_left_for_frame = 315;
        }

        int16_t sample = 0;
        for (int i = 0; i < 8; i++) {
            if (channels[i].volume >= 0) {
                channels[i].pos = (channels[i].pos + incChart[channels[i].note]) & 0xffffff;
                int base = waves[channels[i].bank][(channels[i].pos >> 20)] - 8;
                if (channels[i].pos & 0x800000) {
                    sample += base * channels[i].volume;
                }
            }
        }
        *left++ = sample;
        --len;
        --samples_left_for_frame;
    }

    return 1;
}

static char *argv[] = { "doom", NULL };

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg) {
    if (event == kEventInit) {
        playdate = pd;

        // match DOOM tic rate
        playdate->display->setRefreshRate(35.0f);

        doom_set_print(print_fn);
        doom_set_malloc(malloc_fn, free_fn);
        doom_set_file_io(
            open_fn,
            close_fn,
            read_fn,
            write_fn,
            seek_fn,
            tell_fn,
            eof_fn
        );
        doom_set_gettime(gettime_fn);
        doom_set_exit(exit_fn);
        doom_set_getenv(getenv_fn);

        doom_init(1, argv, 0);

        playdate->sound->addSource(sound_callback, NULL, 1);
        playdate->sound->addSource(music_callback, NULL, 0);
        pd->system->setUpdateCallback(update, NULL);
    }
    
    return 0;
}
