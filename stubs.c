/* ------------- stubs.c ------------- */

#include <conio.h>
#include "dflat.h"

int ApplicationProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    return 0;
}

int NormalProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    return 0;
}

int TextBoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    return 0;
}

int ListBoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    return 0;
}

int EditBoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    return 0;
}

int MenuBarProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    return 0;
}

int PopDownProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    return 0;
}

int ButtonProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    return 0;
}

int DialogProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    return 0;
}

int FrameForeground(WINDOW wnd)
{
    return LIGHTGRAY;
}

int FrameBackground(WINDOW wnd)
{
    return BLUE;
}

void SetStandardColor(WINDOW wnd)
{
    return;
}

void HelpFunction(void)
{
    return;
}

int TestErrorMessage(char *msg)
{
    return FALSE;
}

WINDOW inWindow(int x, int y)
{
    return NULLWND;
}

RECT SetRect(int lf, int tp, int rt, int bt)
{
    RECT rc;

    RectLeft(rc) = lf;
    RectTop(rc) = tp;
    RectRight(rc) = rt;
    RectBottom(rc) = bt;

    return rc;
}

int SendMessage(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    return TRUE;
}

