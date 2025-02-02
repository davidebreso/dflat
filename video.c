/* --------------------- video.c -------------------- */
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <conio.h>
#include "dfast.h"

/* -------- write a character to a window ------- */
void wputch(WINDOW wnd, int c, int x, int y)
{
    int x1 = GetClientLeft(wnd)+x;
    int y1 = GetClientTop(wnd)+y;
    if (x1 < SCREENWIDTH && y1 < SCREENHEIGHT)    {
        PutVideoChar(x1, y1, (c & 255) |
                (clr(foreground, background) << 8));
    }
}

/* ------- write a string to a window ---------- */
void wputs(WINDOW wnd, void *s, int x, int y)
{
    int x1 = GetLeft(wnd)+x;
    int y1 = GetTop(wnd)+y;
    if (x1 < SCREENWIDTH && y1 < SCREENHEIGHT)    {
        int len;
        // clipline(wnd, x, s);
        len = GetRight(wnd) + 1;
        if (len > SCREENWIDTH)
            len = SCREENWIDTH;
        len -= x1;
        PutVideoStr(x1, y1, s, len);
    }
}

