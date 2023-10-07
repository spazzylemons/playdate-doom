#if defined(WIN32)
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include "DOOM.h"

#include "doom_config.h"

#include "d_main.h"
#include "doomdef.h"
#include "doomtype.h"
#include "i_system.h"
#include "m_argv.h"
#include "m_misc.h"

#include <string.h>
#include <stdint.h>


extern byte* screens[5];
extern unsigned char screen_palette[256 * 3];
extern doom_boolean is_wiping_screen;
extern default_t defaults[];
extern int numdefaults;
// extern signed short mixbuffer[2048];


static unsigned char* screen_buffer = 0;
static int button_states[3] = { 0 };
static char itoa_buf[20];


char error_buf[260];
int doom_flags = 0;


void D_DoomLoop(void);
void D_UpdateWipe(void);
void I_UpdateSound(void);
unsigned long I_TickSong(void);

int doom_strcasecmp(const char* str1, const char* str2)
{
    int ret = 0;

    while (!(ret = toupper(*(unsigned char*)str1) - toupper(*(unsigned char*)str2)) && *str1)
        ++str1, ++str2;

    if (ret < 0)
        ret = -1;
    else if (ret > 0)
        ret = 1;

    return (ret);
}


int doom_strncasecmp(const char* str1, const char* str2, int n)
{
    int ret = 0;
    int count = 1;

    while (!(ret = toupper(*(unsigned char*)str1) - toupper(*(unsigned char*)str2)) && *str1 && count++ < n)
        ++str1, ++str2;

    if (ret < 0)
        ret = -1;
    else if (ret > 0)
        ret = 1;

    return (ret);
}


int doom_atox(const char* str)
{
    int i = 0;
    int c;

    while ((c = *str++) != 0)
    {
        i *= 16;
        if (c >= '0' && c <= '9')
            i += c - '0';
        else
            i += c - 'A' + 10;
    }

    return i;
}


const char* doom_itoa(int k, int radix)
{
    int i = k < 0 ? -k : k;
    if (i == 0)
    {
        itoa_buf[0] = '0';
        itoa_buf[1] = '\0';
        return itoa_buf;
    }

    int idx = k < 0 ? 1 : 0;
    int j = i;
    while (j)
    {
        j /= radix;
        idx++;
    }
    itoa_buf[idx] = '\0';

    if (radix == 10)
    {
        while (i)
        {
            itoa_buf[--idx] = '0' + (i % 10);
            i /= 10;
        }
    }
    else
    {
        while (i)
        {
            int k = (i & 0xF);
            if (k >= 10)
                itoa_buf[--idx] = 'A' + ((i & 0xF) - 10);
            else
                itoa_buf[--idx] = '0' + (i & 0xF);
            i >>= 4;
        }
    }

    if (k < 0) itoa_buf[0] = '-';

    return itoa_buf;
}


const char* doom_ctoa(char c)
{
    itoa_buf[0] = c;
    itoa_buf[1] = '\0';
    return itoa_buf;
}


const char* doom_ptoa(void* p)
{
    int idx = 0;
    uintptr_t i = (uintptr_t)p;

    itoa_buf[idx++] = '0';
    itoa_buf[idx++] = 'x';

    while (i)
    {
        int k = (i & 0xF);
        if (k >= 10)
            itoa_buf[idx++] = 'A' + ((i & 0xF) - 10);
        else
            itoa_buf[idx++] = '0' + (i & 0xF);
        i >>= 4;
    }

    itoa_buf[idx] = '\0';
    return itoa_buf;
}


int doom_fprint(void* handle, const char* str)
{
    return doom_write(handle, str, strlen(str));
}


static default_t* get_default(const char* name)
{
    for (int i = 0; i < numdefaults; ++i)
    {
        if (strcmp(defaults[i].name, name) == 0) return &defaults[i];
    }
    return 0;
}


void doom_set_resolution(int width, int height)
{
    if (width <= 0 || height <= 0) return;
    // SCREENWIDTH = width;
    // SCREENHEIGHT = height;
}


void doom_set_default_int(const char* name, int value)
{
    default_t* def = get_default(name);
    if (!def) return;
    def->defaultvalue = value;
}


void doom_set_default_string(const char* name, const char* value)
{
    default_t* def = get_default(name);
    if (!def) return;
    def->default_text_value = (char*)value;
}


void doom_init(int argc, char** argv, int flags)
{
    screen_buffer = doom_malloc(SCREENWIDTH * SCREENHEIGHT);

    myargc = argc;
    myargv = argv;
    doom_flags = flags;

    D_DoomMain();
}


void doom_update(void)
{
    if (is_wiping_screen)
        D_UpdateWipe();
    else
        D_DoomLoop();
}


const unsigned char* doom_get_framebuffer(int channels)
{
    int i, len;

    memcpy(screen_buffer, screens[0], SCREENWIDTH * SCREENHEIGHT);

    extern doom_boolean menuactive;
    extern gamestate_t gamestate; 
    extern doom_boolean automapactive;
    extern int crosshair;

    // Draw crosshair
    if (crosshair && 
        !menuactive &&
        gamestate == GS_LEVEL &&
        !automapactive)
    {
        int y;
        extern int setblocks;
        if (setblocks == 11) y = SCREENHEIGHT / 2 + 8;
        else y = SCREENHEIGHT / 2 - 8;
        for (i = 0; i < 2; ++i)
        {
            screen_buffer[SCREENWIDTH / 2 - 2 - i + y * SCREENWIDTH] = 4;
            screen_buffer[SCREENWIDTH / 2 + 2 + i + y * SCREENWIDTH] = 4;
        }
        for (i = 0; i < 2; ++i)
        {
            screen_buffer[SCREENWIDTH / 2 + (y - 2 - i) * SCREENWIDTH] = 4;
            screen_buffer[SCREENWIDTH / 2 + (y + 2 + i) * SCREENWIDTH] = 4;
        }
    }

    return screen_buffer;
}


unsigned long doom_tick_midi(void)
{
    return I_TickSong();
}


short* doom_get_sound_buffer(void)
{
    I_UpdateSound();
    return 0;
}


void doom_key_down(doom_key_t key)
{
    event_t event;
    event.type = ev_keydown;
    event.data1 = (int)key;
    D_PostEvent(&event);
}


void doom_key_up(doom_key_t key)
{
    event_t event;
    event.type = ev_keyup;
    event.data1 = (int)key;
    D_PostEvent(&event);
}


void doom_button_down(doom_button_t button)
{
    button_states[button] = 1;

    event_t event;
    event.type = ev_mouse;
    event.data1 =
        (button_states[0]) |
        (button_states[1] ? 2 : 0) |
        (button_states[2] ? 4 : 0);
    event.data2 = event.data3 = 0;
    D_PostEvent(&event);
}


void doom_button_up(doom_button_t button)
{
    button_states[button] = 0;

    event_t event;
    event.type = ev_mouse;
    event.data1 =
        (button_states[0]) |
        (button_states[1] ? 2 : 0) |
        (button_states[2] ? 4 : 0);

    event.data1 =
        event.data1
        ^ (button_states[0] ? 1 : 0)
        ^ (button_states[1] ? 2 : 0)
        ^ (button_states[2] ? 4 : 0);

    event.data2 = event.data3 = 0;
    D_PostEvent(&event);
}


void doom_mouse_move(int delta_x, int delta_y)
{
    event_t event;

    event.type = ev_mouse;
    event.data1 =
        (button_states[0]) |
        (button_states[1] ? 2 : 0) |
        (button_states[2] ? 4 : 0);
    event.data2 = delta_x;
    event.data3 = -delta_y;

    if (event.data2 || event.data3)
    {
        D_PostEvent(&event);
    }
}
