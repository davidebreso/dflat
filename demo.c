/* ------------------ demo.c -------------------- */

#include <stdio.h>
#include <stdlib.h>

#include "dfast.h"

static void test_console(void);
static void test_mouse(void);
static void test_video(void);

void main()
{
    /* Intialize video and console routines */
    get_videomode();

    /* Test console functions */
    test_console();

    /* Test mouse functions */
    test_mouse();

    /* Test video functions */
    test_video();
}

void test_console(void)
{
    int x, y, c;

    printf("Video mode is %d\n", video_mode);
    printf("Video address is %04x\n", video_address);
    printf("Video page is %04x\n", video_page);

    curr_cursor(&x, &y);
    printf("Cursor positions was %d %d\n", x, y);

    printf("Press a key to show character and shift status.\n"
           "Press ESC to exit loop\n\n\n");
    curr_cursor(&x, &y);
    hidecursor();
    do {
        cursor(x, y - 2);
        c = getkey();
        printf("Key: %04x\nShift status: %04x\n", c, getshift());
    } while (c != ESC);
    unhidecursor();

    savecursor();
    printf("Cursor position and shape saved\n");
    set_cursor_type(0x060F);
    printf("Press Any Key to restore cursor...");
    getkey();
    restorecursor();
    printf("Cursor position and shape restored      \n\n\n");
    beep();
    beep();
    printf("Beep! Beep!\n");
}

void test_mouse(void)
{
    int x, y, cx, cy;

    resetmouse();
    if (!mouse_installed) {
        printf("Mouse not installed\n");
        return;
    }
    normalcursor();
    printf("Mouse installed\n\n\n");
    curr_cursor(&cx, &cy);
    set_mouseposition(79, 10);
    show_mousecursor();
    hidecursor();
    do {
        get_mouseposition(&x, &y);
        cursor(cx, cy - 2);
        printf("Position: %4d %4d\n", x, y);
        printf("Mouse buttons: %4d\n", mousebuttons());
    } while (!button_releases());
    hide_mousecursor();
    unhidecursor();
    printf("\nPress a key to continue...");
    getkey();
}

void test_video()
{
    int x, y, c, *buffer;
    RECT rc;

    buffer = malloc(SCREENWIDTH * SCREENHEIGHT * 2);
    if (buffer == NULL) {
        printf("Unable to store video buffer, aborting!\n");
        return;
    }

    rc.lf = 0; rc.tp = 0; rc.rt = 79; rc.bt = 24;
    getvideo(rc, buffer);

    c = 0x0740;
    for (x = 0; x < SCREENWIDTH; x++) {
        PutVideoChar(x, 0, c);
        c += 0x0101;
        y = GetVideoChar(x, 0);
        PutVideoChar(x, 1, y);
    }

    PutVideoStr(40, 13, "This string should appear ®\x01ftruncated¯ at (40, 13)", 40);

    cursor(0, 23);
    printf("Press Any Key to restore saved video buffer...");
    getkey();
    storevideo(rc, buffer);
    printf("\nPress Any Key to continue...");
    getkey();
}

