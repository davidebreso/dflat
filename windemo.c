/* ------------------ windemo.c -------------------- */

#include <stdio.h>
#include <stdlib.h>

#include "dflat.h"

static void test_window(void);

void main()
{
    /* Intialize video and console routines */
    get_videomode();
    resetmouse();
    cfg.mono = FALSE;
    cfg.clr = color;

    /* Test window driver */
    test_window();
}

void test_window(void)
{
    WINDOW wnd;

    wnd = CreateWindow(APPLICATION, "Demo", 10, 5, 15, 60, NULL,
                       NULLWND, NULL, SHADOW | HSCROLLBAR | VSCROLLBAR);

    if (wnd == NULLWND) {
        printf("Error while creating window!\n");
        return;
    }

    GetVideoBuffer(wnd);

    wnd->VScrollBox = 3;
    wnd->HScrollBox = 10;
    RepaintBorder(wnd, NULL);
    ClearWindow(wnd, NULL, ' ');
    getkey();

    RestoreVideoBuffer(wnd);
    getkey();
}

