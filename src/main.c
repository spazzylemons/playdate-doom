#include "DOOM.h"
#include "d_player.h"
#include "m_menu.h"
#include "w_wad.h"

extern player_t players[MAXPLAYERS];
extern gamestate_t gamestate;

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

#define PRINT_BUFFER_SIZE 512

static PlaydateAPI *playdate;

void doom_print(const char *str) {
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

void *doom_malloc(int size) {
    if (size == 0) {
        size = 1;
    }
    return playdate->system->realloc(NULL, size);
}

void doom_free(void *ptr) {
    if (ptr != NULL) {
        playdate->system->realloc(ptr, 0);
    }
}

void *doom_open(const char *filename, const char *mode) {
    FileOptions options = strstr(mode, "w") ? kFileWrite : (kFileRead | kFileReadData);
    if (strstr(mode, "a")) options |= kFileAppend;
    return playdate->file->open(filename, options);
}

void doom_close(void *handle) {
    playdate->file->close(handle);
}

int doom_read(void *handle, void *buf, int count) {
    return playdate->file->read(handle, buf, count);
}

int doom_write(void *handle, const void *buf, int count) {
    return playdate->file->write(handle, buf, count);
}

int doom_seek(void *handle, int offset, doom_seek_t origin) {
    return playdate->file->seek(handle, offset, origin);
}

int doom_tell(void *handle) {
    return playdate->file->tell(handle);
}

int doom_eof(void *handle) {
    int old_pos = playdate->file->tell(handle);
    playdate->file->seek(handle, 0, SEEK_END);
    int new_pos = playdate->file->tell(handle);
    playdate->file->seek(handle, old_pos, SEEK_SET);
    return old_pos == new_pos;
}

extern int updatetics;

int I_GetTime(void) {
    return (playdate->system->getCurrentTimeMilliseconds() * 35LL) / 1000LL;
}

void doom_exit(int code) {
    // Can't actually exit using Playdate API. This is the best we can do
    while (true) {
        __builtin_trap();
    }
    __builtin_unreachable();
}

char *doom_getenv(const char *var) {
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

// Alternate keybinds used when in the menus.
static const doom_key_t keybinds_2[] = {
    DOOM_KEY_UNKNOWN,
    DOOM_KEY_UNKNOWN,
    DOOM_KEY_UNKNOWN,
    DOOM_KEY_UNKNOWN,
    DOOM_KEY_BACKSPACE,
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
        brightness[i] = (red * 299 + green * 587 + blue * 114) / 15001;
    }
}

int M_OnMainMenu(void);

extern int menuactive;
extern uint8_t *screens[5];

// Cycle to the next or previous weapon.
static void cycle_weapon(bool forward) {
    player_t *player = &players[0];
    // If we're pending a weapon change, use that as the basis, otherwise use
    // what we're currently wielding.
    weapontype_t weapon;
    if (player->pendingweapon == wp_nochange) {
        weapon = player->readyweapon;
    } else {
        weapon = player->pendingweapon;
    }
    // Go through the list in the direction specified until we find a weapon we own.
    do {
        if (forward) {
            weapon = (weapon + 1) % NUMWEAPONS;
        } else {
            weapon = (weapon + (NUMWEAPONS - 1)) % NUMWEAPONS;
        }
    } while (!player->weaponowned[weapon]);
    // Request a weapon change.
    player->pendingweapon = weapon;
}

static float lastAngle = 0.0f;
static bool wasDocked = true;
static _Atomic bool running_update = false;

static int update(void *userdata) {
    running_update = true;

    PDButtons pressed, released;
    playdate->system->getButtonState(NULL, &pressed, &released);
    for (int i = 0; i < 6; i++) {
        PDButtons mask = 1 << i;
        if (pressed & mask) {
            if (keybinds_2[i] != DOOM_KEY_UNKNOWN && menuactive) {
                if (M_OnMainMenu() && keybinds_2[i] == DOOM_KEY_BACKSPACE) {
                    doom_key_down(DOOM_KEY_ESCAPE);
                } else {
                    doom_key_down(keybinds_2[i]);
                }
            } else {
                doom_key_down(keybinds_1[i]);
            }
        }

        if (released & mask) {
            if (keybinds_2[i] != DOOM_KEY_UNKNOWN && menuactive) {
                if (M_OnMainMenu() && keybinds_2[i] == DOOM_KEY_BACKSPACE) {
                    doom_key_up(DOOM_KEY_ESCAPE);
                } else {
                    doom_key_up(keybinds_2[i]);
                }
            } else {
                doom_key_up(keybinds_1[i]);
            }
        }
    }

    if (gamestate == GS_LEVEL) {
        bool isDocked = playdate->system->isCrankDocked();
        if (wasDocked && !isDocked) {
            lastAngle = playdate->system->getCrankAngle();
        }
        wasDocked = isDocked;
        if (!isDocked) {
            float newAngle = playdate->system->getCrankAngle();
            if (floorf(lastAngle / 90.0f) != floorf(newAngle / 90.0f)) {
                float change = playdate->system->getCrankChange();
                if (change > 0.0f) {
                    cycle_weapon(1);
                } else {
                    cycle_weapon(-1);
                }
            }
            lastAngle = newAngle;
        }
    }

    doom_update();
    const uint8_t *pal_buffer = screens[0];
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
    playdate->graphics->markUpdatedRows(20, 219);
    playdate->system->drawFPS(0, 0);

    running_update = false;

    return 1;
}

extern signed short mixbuffer_left[1024];
extern signed short mixbuffer_right[1024];
void I_UpdateSound(void);

extern void OPL_NextEvent(uint32_t event_int);
extern void OPL_Sample(int16_t *out);

#define OPL_VOLUME_SCALE 16

static int audio_callback(void* context, int16_t* left, int16_t* right, int len) {
    static int samples_left_for_frame = 0;
    static int buffer_index = 2048;

    while (len > 0) {
        if (__builtin_expect(samples_left_for_frame == 0, 0)) {
            for (;;) {
                uint32_t midi = doom_tick_midi();
                if (midi == 0) break;

                OPL_NextEvent(midi);
            }

            samples_left_for_frame = 315;
        }

        if (__builtin_expect(buffer_index == 2048, 0)) {
            I_UpdateSound();
            buffer_index = 0;
        }

        // Get music
        int16_t buf[2];
        OPL_Sample(buf);
        *left = buf[0] * OPL_VOLUME_SCALE;
        *right = buf[1] * OPL_VOLUME_SCALE;
        // Add sfx, stretch out audio to match the hardware 44.1 khz
        int index = buffer_index >> 2;
        *left += mixbuffer_left[index];
        *right += mixbuffer_right[index];
        // Advance buffers
        ++left;
        ++right;
        ++buffer_index;
        --len;
        --samples_left_for_frame;
    }

    return 1;
}

extern doom_boolean I_OPL_InitMusic(void);

void onDoomMenu(void *userdata) {
    if (!menuactive) {
        doom_key_down(DOOM_KEY_ESCAPE);
        doom_key_up(DOOM_KEY_ESCAPE);
    }
}

void I_Quit(void);

static char *argv[] = { "doom", NULL };

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg) {
    if (event == kEventInit) {
        playdate = pd;

        // match DOOM tic rate
        playdate->display->setRefreshRate(35.0f);
        playdate->system->addMenuItem("doom menu", onDoomMenu, NULL);
        doom_init(1, argv, 0);
        updatetics = I_GetTime();

        I_OPL_InitMusic();
        playdate->sound->addSource(audio_callback, NULL, 1);
        pd->system->setUpdateCallback(update, NULL);
    } else if (event == kEventTerminate) {
        // Clean up when terminating.
        I_Quit();
    }

    return 0;
}
