// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//        Gamma correction LUT stuff.
//        Functions to draw patches (by post) directly to screen.
//        Functions to blit a block to the screen.
//
//-----------------------------------------------------------------------------

#include "doom_config.h"

#include "i_system.h"
#include "r_local.h"
#include "doomdef.h"
#include "doomdata.h"
#include "m_bbox.h"
#include "m_swap.h"
#include "v_video.h"


// Each screen is [SCREENWIDTH*SCREENHEIGHT]; 
byte* screens[5];

int dirtybox[4];

int usegamma;


//
// V_MarkRect 
// 
void V_MarkRect(int x, int y, int width, int height)
{
    M_AddToBox(dirtybox, x, y);
    M_AddToBox(dirtybox, x + width - 1, y + height - 1);
}


//
// V_CopyRect 
// 
void V_CopyRect(int srcx,
                int srcy,
                int srcscrn,
                int width,
                int height,
                int destx,
                int desty,
                int destscrn)
{
    byte* src;
    byte* dest;

#ifdef RANGECHECK 
    if (srcx<0
        || srcx + width >SCREENWIDTH
        || srcy<0
        || srcy + height>SCREENHEIGHT
        || destx<0 || destx + width >SCREENWIDTH
        || desty<0
        || desty + height>SCREENHEIGHT
        || (unsigned)srcscrn > 4
        || (unsigned)destscrn > 4)
    {
        I_Error("Error: Bad V_CopyRect");
    }
#endif 
    V_MarkRect(destx, desty, width, height);

    src = screens[srcscrn] + SCREENWIDTH * srcy + srcx;
    dest = screens[destscrn] + SCREENWIDTH * desty + destx;

    for (; height > 0; height--)
    {
        memcpy(dest, src, width);
        src += SCREENWIDTH;
        dest += SCREENWIDTH;
    }
}


//
// V_DrawPatch
// Masks a column based masked pic to the screen. 
//
void V_DrawPatch(int x, int y, int scrn, patch_t* patch)
{
    int count;
    int col;
    column_t* column;
    byte* desttop;
    byte* dest;
    byte* source;
    int w;

    y -= SHORT(patch->topoffset);
    x -= SHORT(patch->leftoffset);
#ifdef RANGECHECK 
    if (x<0
        || x + SHORT(patch->width) >SCREENWIDTH
        || y<0
        || y + SHORT(patch->height)>SCREENHEIGHT
        || (unsigned)scrn > 4)
    {
        //doom_print("Patch at %d,%d exceeds LFB\n", x, y);
        doom_print("Patch at ");
        doom_print(doom_itoa(x, 10));
        doom_print(",");
        doom_print(doom_itoa(y, 10));
        doom_print(" exceeds LFB\n");
        // No I_Error abort - what is up with TNT.WAD?
        doom_print("V_DrawPatch: bad patch (ignored)\n");
        return;
    }
#endif 

    if (!scrn)
        V_MarkRect(x, y, SHORT(patch->width), SHORT(patch->height));

    col = 0;
    desttop = screens[scrn] + y * SCREENWIDTH + x;

    w = SHORT(patch->width);

    for (; col < w; x++, col++, desttop++)
    {
        column = (column_t*)((byte*)patch + LONG(patch->columnofs[col]));

        // step through the posts in a column 
        while (column->topdelta != 0xff)
        {
            source = (byte*)column + 3;
            dest = desttop + column->topdelta * SCREENWIDTH;
            count = column->length;

            while (count--)
            {
                *dest = *source++;
                dest += SCREENWIDTH;
            }
            column = (column_t*)((byte*)column + column->length
                                 + 4);
        }
    }
}


//
// V_DrawPatchFlipped 
// Masks a column based masked pic to the screen.
// Flips horizontally, e.g. to mirror face.
//
void V_DrawPatchFlipped(int x, int y, int scrn, patch_t* patch)
{
    int count;
    int col;
    column_t* column;
    byte* desttop;
    byte* dest;
    byte* source;
    int w;

    y -= SHORT(patch->topoffset);
    x -= SHORT(patch->leftoffset);
#ifdef RANGECHECK 
    if (x<0
        || x + SHORT(patch->width) >SCREENWIDTH
        || y<0
        || y + SHORT(patch->height)>SCREENHEIGHT
        || (unsigned)scrn > 4)
    {
        //doom_print("Patch origin %d,%d exceeds LFB\n", x, y);
        doom_print("Patch origin ");
        doom_print(doom_itoa(x, 10));
        doom_print(",");
        doom_print(doom_itoa(y, 10));
        doom_print(" exceeds LFB\n");
        I_Error("Error: Bad V_DrawPatch in V_DrawPatchFlipped");
    }
#endif 

    if (!scrn)
        V_MarkRect(x, y, SHORT(patch->width), SHORT(patch->height));

    col = 0;
    desttop = screens[scrn] + y * SCREENWIDTH + x;

    w = SHORT(patch->width);

    for (; col < w; x++, col++, desttop++)
    {
        column = (column_t*)((byte*)patch + LONG(patch->columnofs[w - 1 - col]));

        // step through the posts in a column 
        while (column->topdelta != 0xff)
        {
            source = (byte*)column + 3;
            dest = desttop + column->topdelta * SCREENWIDTH;
            count = column->length;

            while (count--)
            {
                *dest = *source++;
                dest += SCREENWIDTH;
            }
            column = (column_t*)((byte*)column + column->length
                                 + 4);
        }
    }
}


void V_DrawPatchRectDirect(int x, int y, int scrn, patch_t* patch, int src_x, int src_w)
{
    int count;
    int col;
    column_t* column;
    byte* desttop;
    byte* dest;
    byte* source;
    int w;

    y -= SHORT(patch->topoffset);
    x -= SHORT(patch->leftoffset);
#ifdef RANGECHECK 
    if (x<0
        || x + SHORT(src_w) >SCREENWIDTH
        || y<0
        || y + SHORT(patch->height)>SCREENHEIGHT
        || (unsigned)scrn > 4)
    {
        //doom_print("Patch at %d,%d exceeds LFB\n", x, y);
        doom_print("Patch at ");
        doom_print(doom_itoa(x, 10));
        doom_print(",");
        doom_print(doom_itoa(y, 10));
        doom_print(" exceeds LFB\n");
        // No I_Error abort - what is up with TNT.WAD?
        doom_print("V_DrawPatch: bad patch (ignored)\n");
        return;
    }
#endif 

    if (!scrn)
        V_MarkRect(x, y, SHORT(src_w), SHORT(patch->height));

    col = 0;
    desttop = screens[scrn] + y * SCREENWIDTH + x;

    w = SHORT(src_w);

    for (; col < w; x++, col++, desttop++)
    {
        column = (column_t*)((byte*)patch + LONG(patch->columnofs[col + src_x]));

        // step through the posts in a column 
        while (column->topdelta != 0xff)
        {
            source = (byte*)column + 3;
            dest = desttop + column->topdelta * SCREENWIDTH;
            count = column->length;

            while (count--)
            {
                *dest = *source++;
                dest += SCREENWIDTH;
            }
            column = (column_t*)((byte*)column + column->length
                                 + 4);
        }
    }
}


//
// V_DrawPatchDirect
// Draws directly to the screen on the pc. 
//
void V_DrawPatchDirect(int x, int y, int scrn, patch_t* patch)
{
    V_DrawPatch(x, y, scrn, patch);
}


//
// V_DrawBlock
// Draw a linear block of pixels into the view buffer.
//
void V_DrawBlock(int x, int y, int scrn, int width, int height, byte* src)
{
    byte* dest;

#ifdef RANGECHECK 
    if (x<0
        || x + width >SCREENWIDTH
        || y<0
        || y + height>SCREENHEIGHT
        || (unsigned)scrn > 4)
    {
        I_Error("Error: Bad V_DrawBlock");
    }
#endif 

    V_MarkRect(x, y, width, height);

    dest = screens[scrn] + y * SCREENWIDTH + x;

    while (height--)
    {
        memcpy(dest, src, width);
        src += width;
        dest += SCREENWIDTH;
    }
}


//
// V_GetBlock
// Gets a linear block of pixels from the view buffer.
//
void V_GetBlock(int x, int y, int scrn, int width, int height, byte* dest)
{
    byte* src;

#ifdef RANGECHECK 
    if (x<0
        || x + width >SCREENWIDTH
        || y<0
        || y + height>SCREENHEIGHT
        || (unsigned)scrn > 4)
    {
        I_Error("Error: Bad V_DrawBlock");
    }
#endif 

    src = screens[scrn] + y * SCREENWIDTH + x;

    while (height--)
    {
        memcpy(dest, src, width);
        src += SCREENWIDTH;
        dest += width;
    }
}


//
// V_Init
// 
void V_Init(void)
{
    int i;
    byte* base;

    // stick these in low dos memory on PCs

    base = I_AllocLow(SCREENWIDTH * SCREENHEIGHT * 4);

    for (i = 0; i < 4; i++)
        screens[i] = base + i * SCREENWIDTH * SCREENHEIGHT;
}
