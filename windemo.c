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

    writeline(wnd, "This string should appear ®\x0f\x01truncated¯ at (20, 1)",
        20, 1, FALSE);
    writeline(wnd, "®\x0e\x00" "This is ®\x0e\x02not¯ truncated!",
        20, 2, FALSE);
    writeline(wnd, "This should be ®\x0e\x02padded!",
        20, 3, TRUE);
    getkey();

    RestoreVideoBuffer(wnd);
    getkey();
}

