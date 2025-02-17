/* ---------- window.c ------------- */

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "dflat.h"

WINDOW inFocus = NULLWND;

int foreground = LIGHTGRAY;   /* current video colors */
int background = BLACK;

static void near InsertTitle(WINDOW, char *);
static void near DisplayTitle(WINDOW, RECT);
static RECT near AdjustRect(WINDOW, RECT);

/* --------- create a window ------------ */
WINDOW CreateWindow(
    CLASS class,              /* class of this window       */
    char *ttl,                /* title or NULL              */
    int left, int top,        /* upper left coordinates     */
    int height, int width,    /* dimensions                 */
    void *extension,          /* pointer to additional data */
    WINDOW parent,            /* parent of this window      */
    int (*wndproc)(struct window *,enum messages,PARAM,PARAM),
    int attrib)               /* window attribute           */
{
    WINDOW wnd = malloc(sizeof(struct window));
    if (wnd != NULLWND)    {
        int base;
        /* ----- coordinates -1, -1 = center the window ---- */
        if (left == -1)
            wnd->rc.lf = (screenwidth-width)/2;
        else
            wnd->rc.lf = left;
        if (top == -1)
            wnd->rc.tp = (screenheight-height)/2;
        else
            wnd->rc.tp = top;
        wnd->attrib = attrib;
        if (ttl != NULL)
            AddAttribute(wnd, TITLEBAR);
        if (wndproc == NULL)
            wnd->wndproc = classdefs[FindClass(class)].wndproc;
        else
            wnd->wndproc = wndproc;
        /* ---- derive attributes of base classes ---- */
        base = class;
        while (base != -1)    {
            int tclass = FindClass(base);
            AddAttribute(wnd, classdefs[tclass].attrib);
            base = classdefs[tclass].base;
        }
        if (parent && !TestAttribute(wnd, NOCLIP))    {
            /* -- keep upper left within borders of parent -- */
            wnd->rc.lf = max(wnd->rc.lf, GetClientLeft(parent));
            wnd->rc.tp = max(wnd->rc.tp, GetClientTop(parent) +
                             (TestAttribute(parent, HASMENUBAR) ? 1 : 0));
        }
        wnd->class = class;
        wnd->extension = extension;
        wnd->rc.rt = GetLeft(wnd)+width-1;
        wnd->rc.bt = GetTop(wnd)+height-1;
        wnd->ht = height;
        wnd->wd = width;
        wnd->title = ttl;
        if (ttl != NULL)
            InsertTitle(wnd, ttl);
        wnd->next = wnd->prev = wnd->dFocus = NULLWND;
        wnd->parent = parent;
        wnd->videosave = NULL;
        wnd->condition = ISRESTORED;
        wnd->restored_attrib = 0;
        wnd->RestoredRC = wnd->rc;
        wnd->PrevKeyboard = wnd->PrevMouse = NULL;
        wnd->DeletedText = NULL;
        SendMessage(wnd, CREATE_WINDOW, 0, 0);
        if (isVisible(wnd))
            SendMessage(wnd, SHOW_WINDOW, 0, 0);
    }
    return wnd;
}

/* -------- add a title to a window --------- */
void AddTitle(WINDOW wnd, char *ttl)
{
    InsertTitle(wnd, ttl);
    SendMessage(wnd, BORDER, 0, 0);
}

/* ----- insert a title into a window ---------- */
static void near InsertTitle(WINDOW wnd, char *ttl)
{
    if ((wnd->title = malloc(strlen(ttl)+1)) != NULL)
        strcpy(wnd->title, ttl);
}

/* ------- write a character to a window at x,y ------- */
void PutWindowChar(WINDOW wnd, int x, int y, int c)
{
    int x1 = GetClientLeft(wnd)+x;
    int y1 = GetClientTop(wnd)+y;

    if (isVisible(wnd))    {
        if (!TestAttribute(wnd, NOCLIP))    {
            WINDOW wnd1 = GetParent(wnd);
            while (wnd1 != NULLWND)    {
                /* --- clip character to parent's borders --- */
                if (x1 < GetClientLeft(wnd1)   ||
                    x1 > GetClientRight(wnd1)  ||
                    y1 > GetClientBottom(wnd1) ||
                    y1 < GetClientTop(wnd1)    ||
                    (y1 < GetTop(wnd1)+2 &&
                            TestAttribute(wnd1, HASMENUBAR)))
                        return;
                wnd1 = GetParent(wnd1);
            }
        }
        if (x1 < screenwidth && y1 < screenheight)
            PutVideoChar(x1, y1, c);
    }
}

static char line[161];

/* ------ write a line to video window client area ------ */
void writeline(WINDOW wnd, char *str, int x, int y, int pad)
{
    RECT rc;
    int len;

    RectLeft(rc) = GetClientLeft(wnd) + x;
    RectTop(rc) = RectBottom(rc) = GetClientTop(wnd) + y;
    RectRight(rc) = GetClientRight(wnd);

    rc = AdjustRect(wnd, rc);

    if (RectLeft(rc) > RectRight(rc) || RectTop(rc) > RectBottom(rc))
        return;

    PutVideoStr(RectLeft(rc), RectTop(rc), str, RectWidth(rc), pad);
}

/* -------- display a window's title --------- */
static void near DisplayTitle(WINDOW wnd, RECT rc)
{
    int tlen = min(strlen(wnd->title), WindowWidth(wnd)-2);
    int tend = WindowWidth(wnd)-4;

    if (SendMessage(wnd, TITLE, 0, 0))    {
        if (wnd == inFocus)    {
            foreground = cfg.clr.InFocusTitleFG;
            background = cfg.clr.InFocusTitleBG;
        }
        else    {
            foreground = cfg.clr.TitleFG;
            background = cfg.clr.TitleBG;
        }
        memset(line,' ',WindowWidth(wnd)-2);
        if (wnd->condition != ISMINIMIZED)
            strncpy(line + ((WindowWidth(wnd)-2 - tlen) / 2),
                wnd->title, tlen);
        line[WindowWidth(wnd)-2] = '\0';
        if (TestAttribute(wnd, CONTROLBOX))
            line[1] = CONTROLBOXCHAR;
        if (TestAttribute(wnd, MINMAXBOX))    {
            switch (wnd->condition)    {
                case ISRESTORED:
                    line[tend+1] = MAXPOINTER;
                    line[tend]   = MINPOINTER;
                    break;
                case ISMINIMIZED:
                    line[tend+1] = MAXPOINTER;
                    break;
                case ISMAXIMIZED:
                    line[tend]   = MINPOINTER;
                    line[tend+1] = RESTOREPOINTER;
                    break;
                default:
                    break;
            }
        }
        RectLeft(rc) = max(RectLeft(rc), GetLeft(wnd) + 1);
        RectRight(rc) = min(RectRight(rc), GetRight(wnd) - 1);
        PutVideoStr(RectLeft(rc), GetTop(wnd),
                    line + (RectLeft(rc) - GetLeft(wnd) - 1),
                    RectWidth(rc), FALSE);
    }
}

/* ---- display a window's vertical scroll bar ---- */
static void near DisplayVScrollBar(WINDOW wnd, RECT rc)
{
    if (RectTop(rc) == GetTop(wnd) + 1) {
        PutVideoChar(RectRight(rc), RectTop(rc), UPSCROLLBOX);
        RectTop(rc)++;
    }
    if (RectBottom(rc) == GetBottom(wnd) - 1) {
        PutVideoChar(RectRight(rc), RectBottom(rc), DOWNSCROLLBOX);
        RectBottom(rc)--;
    }
    if (RectTop(rc) <= RectBottom(rc))
        FillVideoRect(rc, SCROLLBARCHAR);
    if (wnd->VScrollBox) {
        int y = GetTop(wnd) + 1 + wnd->VScrollBox;
        if (RectTop(rc) <= y && y <= RectBottom(rc))
            PutVideoChar(RectRight(rc), y, SCROLLBOXCHAR);
    }
}

/* ---- display a window's horizontal scroll bar ---- */
static void near DisplayHScrollBar(WINDOW wnd, RECT rc)
{
    if (RectLeft(rc) == GetLeft(wnd) + 1) {
        PutVideoChar(RectLeft(rc), RectTop(rc), LEFTSCROLLBOX);
        RectLeft(rc)++;
    }
    if (RectRight(rc) == GetRight(wnd) - 1) {
        PutVideoChar(RectRight(rc), RectTop(rc), RIGHTSCROLLBOX);
        RectRight(rc)--;
    }
    if (RectLeft(rc) <= RectRight(rc))
        FillVideoRect(rc, SCROLLBARCHAR);
    if (wnd->HScrollBox) {
        int x = GetLeft(wnd) + 1 + wnd->HScrollBox;
        if (RectLeft(rc) <= x && x <= RectRight(rc))
            PutVideoChar(x, RectTop(rc), SCROLLBOXCHAR);
    }
}

/* -- adjust a window's rectangle to clip it to its parent -- */
static RECT near AdjustRect(WINDOW wnd, RECT rc)
{
    if (!TestAttribute(wnd, NOCLIP))    {
        WINDOW pwnd = GetParent(wnd);
        if (pwnd != NULLWND)    {
            RectTop(rc) = max(RectTop(rc),
                        GetClientTop(pwnd));
            RectLeft(rc) = max(RectLeft(rc),
                        GetClientLeft(pwnd));
            RectRight(rc) = min(RectRight(rc),
                        GetClientRight(pwnd));
            RectBottom(rc) = min(RectBottom(rc),
                        GetClientBottom(pwnd));
        }
    }
    RectRight(rc) = min(RectRight(rc), screenwidth-1);
    RectBottom(rc) = min(RectBottom(rc), screenheight-1);
    return rc;
}

/* ------- display a window's border ----- */
void RepaintBorder(WINDOW wnd, RECT *rcc)
{
    int y;
    int lin, side, ne, nw, se, sw;
    RECT rc, brc;

    if (!TestAttribute(wnd, HASBORDER))
        return;
    if (rcc == NULL)    {
        rc = SetRect(0, 0, WindowWidth(wnd)-1,
                WindowHeight(wnd)-1);
        if (TestAttribute(wnd, SHADOW))    {
            rc.rt++;
            rc.bt++;
        }
    }
    else
        rc = *rcc;
    /* -------- compute absolute rectangle ------- */
    RectLeft(rc) += GetLeft(wnd);
    RectRight(rc) += GetLeft(wnd);
    RectTop(rc) += GetTop(wnd);
    RectBottom(rc) += GetTop(wnd);
    rc = AdjustRect(wnd, rc);

    if (RectLeft(rc) > RectRight(rc) || RectTop(rc) > RectBottom(rc))
        return;

    if (wnd == inFocus)    {
        lin  = FOCUS_LINE;
        side = FOCUS_SIDE;
        ne   = FOCUS_NE;
        nw   = FOCUS_NW;
        se   = FOCUS_SE;
        sw   = FOCUS_SW;
    }
    else    {
        lin  = LINE;
        side = SIDE;
        ne   = NE;
        nw   = NW;
        se   = SE;
        sw   = SW;
    }
    /* ---------- window title ------------ */
    if (RectTop(rc) == GetTop(wnd))
        if (RectLeft(rc) < GetRight(wnd))
            if (TestAttribute(wnd, TITLEBAR))
                DisplayTitle(wnd, rc);
    /* -------- top frame corners --------- */
    foreground = FrameForeground(wnd);
    background = FrameBackground(wnd);
    if (RectTop(rc) == GetTop(wnd)) {
        if (RectLeft(rc) == GetLeft(wnd))
            PutVideoChar(GetLeft(wnd), GetTop(wnd), nw);
        if (RectLeft(rc) < RectRight(rc))    {
            if (RectRight(rc) >= GetRight(wnd))
                PutVideoChar(GetRight(wnd), GetTop(wnd), ne);

            if (TestAttribute(wnd, TITLEBAR) == 0)    {
                /* ----------- top line ------------- */
                brc.tp = brc.bt = GetTop(wnd);
                brc.lf = max(GetLeft(wnd) + 1, RectLeft(rc));
                brc.rt = min(GetRight(wnd) - 1, RectRight(rc));
                FillVideoRect(brc, lin);
            }
        }
    }
    /* ----------- window body ------------ */
    brc.tp = max(GetClientTop(wnd), RectTop(rc));
    brc.bt = min(GetClientBottom(wnd), RectBottom(rc));
    if (RectLeft(rc) == GetLeft(wnd)) {
        brc.lf = brc.rt = GetLeft(wnd);
        FillVideoRect(brc, side);
    }
    if (RectRight(rc) >= GetRight(wnd)) {
        brc.lf = brc.rt = GetRight(wnd);
        if (TestAttribute(wnd, VSCROLLBAR)) {
            DisplayVScrollBar(wnd, brc);
        } else {
            FillVideoRect(brc, side);
        }
    }
    if (RectRight(rc) > GetRight(wnd)) {
        brc.lf = brc.rt = GetRight(wnd) + 1;
        brc.bt = min(GetBottom(wnd), RectBottom(rc));
        ColorVideoRect(brc, clr(SHADOWFG, BLACK));
    }
    if (RectTop(rc) < RectBottom(rc) &&
            RectBottom(rc) >= GetBottom(wnd))    {
        /* -------- bottom frame corners ---------- */
        if (RectLeft(rc) == GetLeft(wnd))
            PutVideoChar(GetLeft(wnd), GetBottom(wnd), sw);
        if (RectRight(rc) >= GetRight(wnd))
            PutVideoChar(GetRight(wnd), GetBottom(wnd), se);
        /* ----------- bottom line ------------- */
        brc.tp = brc.bt = GetBottom(wnd);
        brc.lf = max(GetLeft(wnd) + 1, RectLeft(rc));
        brc.rt = min(GetRight(wnd) - 1, RectRight(rc));
        if (TestAttribute(wnd, HSCROLLBAR)) {
            DisplayHScrollBar(wnd, brc);
        } else {
            FillVideoRect(brc, lin);
        }
    }
    if (RectBottom(rc) > GetBottom(wnd)) {
        /* ---------- bottom shadow ------------- */
        brc.tp = brc.bt = GetBottom(wnd) + 1;
        brc.lf = max(GetLeft(wnd) + 1, RectLeft(rc));
        brc.rt = min(GetRight(wnd) + 1, RectRight(rc));
        ColorVideoRect(brc, clr(SHADOWFG, BLACK));
    }
}

/* ------ clear the data space of a window -------- */
void ClearWindow(WINDOW wnd, RECT *rcc, int clrchar)
{
    if (isVisible(wnd))    {
        RECT rc;

        if (rcc == NULL) {
            rc = SetRect(0, 0, ClientWidth(wnd)-1,ClientHeight(wnd)-1);
        } else {
            rc = *rcc;
        }
        RectLeft(rc) += GetClientLeft(wnd);
        RectTop(rc) += GetClientTop(wnd);
        RectRight(rc) += GetClientLeft(wnd);
        RectBottom(rc) += GetClientTop(wnd);
        rc = AdjustRect(wnd, rc);
        if (RectLeft(rc) < screenwidth && RectTop(rc) < screenheight) {
            SetStandardColor(wnd);
            FillVideoRect(rc, clrchar);
        }
    }
}

/* --- get the video memory that is to be used by a window -- */
void GetVideoBuffer(WINDOW wnd)
{
    RECT rc;
    int ht;
    int wd;

    rc = wnd->rc;
    if (TestAttribute(wnd, SHADOW))    {
        RectBottom(rc)++;
        RectRight(rc)++;
    }
    rc = AdjustRect(wnd, rc);
    if (RectLeft(rc) >= screenwidth || RectTop(rc) >= screenheight)
        return;
    ht = RectBottom(rc) - RectTop(rc) + 1;
    wd = RectRight(rc) - RectLeft(rc) + 1;
    wnd->videosave = realloc(wnd->videosave, (ht * wd * 2));
    if (wnd->videosave != NULL)
        getvideo(rc, wnd->videosave);
}

/* --- restore the video memory that was used by a window --- */
void RestoreVideoBuffer(WINDOW wnd)
{
    if (wnd->videosave != NULL)    {
        RECT rc = wnd->rc;
        if (TestAttribute(wnd, SHADOW))    {
            RectBottom(rc)++;
            RectRight(rc)++;
        }
        rc = AdjustRect(wnd, rc);
        if (RectLeft(rc) >= screenwidth || RectTop(rc) >= screenheight)
            return;
        storevideo(rc, wnd->videosave);
        free(wnd->videosave);
        wnd->videosave = NULL;
    }
}

