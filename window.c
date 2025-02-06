/* ---------- window.c ------------- */

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "dfast.h"

WINDOW inFocus = NULLWND;

int foreground, background;   /* current video colors */

static void InsertTitle(WINDOW, char *);
static void DisplayTitle(WINDOW, RECT);

/* --------- create a window ------------ */
WINDOW CreateWindow(
    CLASS class,              /* class of this window       */
    char *ttl,                /* title or NULL              */
    int left, int top,        /* upper left coordinates     */
    int height, int width,    /* dimensions                 */
    void *extension,          /* pointer to additional data */
    WINDOW parent,            /* parent of this window      */
    int (*wndproc)(struct window *,enum messages,PARAM,PARAM),int attrib)
                              /* window attribute           */
{
    WINDOW wnd = malloc(sizeof(struct window));
    if (wnd != NULLWND)    {
        int base;
        /* ----- coordinates -1, -1 = center the window ---- */
        if (left == -1)
            wnd->rc.lf = (SCREENWIDTH-width)/2;
        else
            wnd->rc.lf = left;
        if (top == -1)
            wnd->rc.tp = (SCREENHEIGHT-height)/2;
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
static void InsertTitle(WINDOW wnd, char *ttl)
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
        if (x1 < SCREENWIDTH && y1 < SCREENHEIGHT)
            wputch(wnd, c, x, y);
    }
}

static char line[161];

/* ----- clip line if it extends below the bottom of the
             parent window ------ */
static int clipbottom(WINDOW wnd, int y)
{
    if (!TestAttribute(wnd, NOCLIP))    {
        WINDOW wnd1 = GetParent(wnd);
        while (wnd1 != NULLWND)    {
            if (GetClientTop(wnd)+y > GetBottom(wnd1))
                return TRUE;
            wnd1 = GetParent(wnd1);
        }
    }
    return GetClientTop(wnd)+y > SCREENHEIGHT;
}

/* ------ clip the portion of a line that extends past the
                     right margin of the parent window ----- */
void clipline(WINDOW wnd, int x, char *ln)
{
    WINDOW pwnd = GetParent(wnd);
    int x1 = strlen(ln);
    int i = 0;

    if (!TestAttribute(wnd, NOCLIP))    {
        while (pwnd != NULLWND)    {
            x1 = GetRight(pwnd) - GetLeft(wnd) - x;
            pwnd = GetParent(pwnd);
        }
    }
    else if (GetLeft(wnd) + x > SCREENWIDTH)
        x1 = SCREENWIDTH-GetLeft(wnd) - x;
    /* --- adjust the clipping offset for color controls --- */
    if (x1 < 0)
        x1 = 0;
    while (i < x1)    {
        if ((unsigned char) ln[i] == CHANGECOLOR)
            i += 3, x1 += 3;
        else if ((unsigned char) ln[i] == RESETCOLOR)
            i++, x1++;
        else
            i++;
    }
    ln[x1] = '\0';
}

/* ------ write a line to video window client area ------ */
void writeline(WINDOW wnd, char *str, int x, int y, int pad)
{
    char wline[120];

    if (TestAttribute(wnd, HASBORDER))    {
        x++;
        y++;
    }
    if (!clipbottom(wnd, y))    {
        char *cp;
        int len;
        int dif;

        memset(wline, 0, sizeof wline);
        len = LineLength(str);
        dif = strlen(str) - len;
        strncpy(wline, str, ClientWidth(wnd) + dif);
        if (pad)    {
            cp = wline+strlen(wline);
            while (len++ < ClientWidth(wnd)-x)
                *cp++ = ' ';
        }
        clipline(wnd, x, wline);
        wputs(wnd, wline, x, y);
    }
}

/* -- write a line to video window (including the border) -- */
void writefull(WINDOW wnd, char *str, int y)
{
    if (!clipbottom(wnd, y))    {
        strcpy(line, str);
        clipline(wnd, 0, line);
        wputs(wnd, line, 0, y);
    }
}

/* -------- display a window's title --------- */
static void DisplayTitle(WINDOW wnd, RECT rc)
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
        line[RectRight(rc)+1] = '\0';
        writeline(wnd, line+RectLeft(rc),
                    RectLeft(rc), -1, FALSE);
    }
}

/* --- display right border shadow character of a window --- */
static void near shadow_char(WINDOW wnd, int y)
{
    int fg = foreground;
    int bg = background;
    int x = WindowWidth(wnd);
    int c = videochar(GetLeft(wnd)+x, GetTop(wnd)+y+1);

    if (TestAttribute(wnd, SHADOW) == 0)
        return;
    foreground = SHADOWFG;
    background = BLACK;
    PutWindowChar(wnd, x-1, y, c);
    foreground = fg;
    background = bg;
}

/* --- display the bottom border shadow line for a window --- */
static void near shadowline(WINDOW wnd, RECT rc)
{
    int i;
    int y = GetBottom(wnd)+1;
    if ((TestAttribute(wnd, SHADOW)) == 0)
        return;
    if (!clipbottom(wnd, WindowHeight(wnd)))    {
        int fg = foreground;
        int bg = background;
        for (i = 0; i < WindowWidth(wnd); i++)
            line[i] = videochar(GetLeft(wnd)+i+1, y);
        line[i] = '\0';
        foreground = SHADOWFG;
        background = BLACK;
        clipline(wnd, 1, line);
        line[RectRight(rc)+3] = '\0';
        wputs(wnd, line+RectLeft(rc), 1+RectLeft(rc),
            WindowHeight(wnd));
        foreground = fg;
        background = bg;
    }
}

/* ------- display a window's border ----- */
void RepaintBorder(WINDOW wnd, RECT *rcc)
{
    int y;
    int lin, side, ne, nw, se, sw;
    RECT rc, clrc;

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
    clrc = rc;
    /* -------- adjust the client rectangle ------- */
    if (RectLeft(rc) == 0)
        --clrc.rt;
    else
        --clrc.lf;
    if (RectTop(rc) == 0)
        --clrc.bt;
    else
        --clrc.tp;
    RectRight(clrc) = min(RectRight(clrc), WindowWidth(wnd)-3);
    RectBottom(clrc) =
                     min(RectBottom(clrc), WindowHeight(wnd)-3);
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
    line[WindowWidth(wnd)] = '\0';
    /* ---------- window title ------------ */
    if (RectTop(rc) == 0)
        if (TestAttribute(wnd, TITLEBAR))
            DisplayTitle(wnd, clrc);
    foreground = FrameForeground(wnd);
    background = FrameBackground(wnd);
    /* -------- top frame corners --------- */
    if (RectTop(rc) == 0)    {
        if (RectLeft(rc) == 0)
            PutWindowChar(wnd, -1, -1, nw);
        if (RectRight(rc) >= WindowWidth(wnd)-1)
            PutWindowChar(wnd, WindowWidth(wnd)-2, -1, ne);

        if (TestAttribute(wnd, TITLEBAR) == 0)    {
            /* ----------- top line ------------- */
            memset(line,lin,WindowWidth(wnd)-1);
            line[RectRight(clrc)+1] = '\0';
            if (strlen(line+RectLeft(clrc)) > 1 ||
                            TestAttribute(wnd, SHADOW) == 0)
                writeline(wnd, line+RectLeft(clrc),
                        RectLeft(clrc), -1, FALSE);
        }
    }
    /* ----------- window body ------------ */
    for (y = 0; y < ClientHeight(wnd); y++)    {
        int ch;
        if (y >= RectTop(clrc) && y <= RectBottom(clrc))    {
            if (RectLeft(rc) == 0)
                PutWindowChar(wnd, -1, y, side);
            if (RectRight(rc) >= ClientWidth(wnd))    {
                if (TestAttribute(wnd, VSCROLLBAR))
                    ch = (    y == 0 ? UPSCROLLBOX      :
                              y == WindowHeight(wnd)-3  ?
                                   DOWNSCROLLBOX        :
                              y == wnd->VScrollBox      ?
                                   SCROLLBOXCHAR        :
                                   SCROLLBARCHAR );
                else
                    ch = side;
                PutWindowChar(wnd, WindowWidth(wnd)-2, y, ch);
            }
            if (RectRight(rc) == WindowWidth(wnd))
                shadow_char(wnd, y);
        }
    }
    if (RectBottom(rc) >= WindowHeight(wnd)-1)    {
        /* -------- bottom frame corners ---------- */
        if (RectLeft(rc) == 0)
            PutWindowChar(wnd, -1, WindowHeight(wnd)-2, sw);
        if (RectRight(rc) >= WindowWidth(wnd)-1)
            PutWindowChar(wnd, WindowWidth(wnd)-2,
                WindowHeight(wnd)-2, se);
        /* ----------- bottom line ------------- */
        memset(line,lin,WindowWidth(wnd)-1);
        if (TestAttribute(wnd, HSCROLLBAR))    {
            line[0] = LEFTSCROLLBOX;
            line[WindowWidth(wnd)-3] = RIGHTSCROLLBOX;
            memset(line+1, SCROLLBARCHAR, WindowWidth(wnd)-4);
            line[wnd->HScrollBox] = SCROLLBOXCHAR;
        }
        line[RectRight(clrc)+1] = '\0';
        if (strlen(line+RectLeft(clrc)) > 1 ||
                        TestAttribute(wnd, SHADOW) == 0)
            writeline(wnd,
                line+RectLeft(clrc),
                RectLeft(clrc),
                WindowHeight(wnd)-2,
                FALSE);
        if (RectRight(rc) == WindowWidth(wnd))
            shadow_char(wnd, WindowHeight(wnd)-2);
    }
    if (RectBottom(rc) == WindowHeight(wnd))
        /* ---------- bottom shadow ------------- */
        shadowline(wnd, clrc);
}

/* ------ clear the data space of a window -------- */
void ClearWindow(WINDOW wnd, RECT *rcc, int clrchar)
{
    if (isVisible(wnd))    {
        int y;
        RECT rc;

        if (rcc == NULL)
            rc = SetRect(0, 0, ClientWidth(wnd)-1,
                ClientHeight(wnd)-1);
        else
            rc = *rcc;
        SetStandardColor(wnd);
        memset(line, clrchar, RectWidth(rc));
        line[RectWidth(rc)] = '\0';
        for (y = RectTop(rc); y <= RectBottom(rc); y++)
            writeline(wnd, line, RectLeft(rc), y, FALSE);
    }
}

/* -- adjust a window's rectangle to clip it to its parent -- */
static RECT near AdjustRect(WINDOW wnd)
{
    RECT rc = wnd->rc;
    if (TestAttribute(wnd, SHADOW))    {
        RectBottom(rc)++;
        RectRight(rc)++;
    }
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
    RectRight(rc) = min(RectRight(rc), SCREENWIDTH-1);
    RectBottom(rc) = min(RectBottom(rc), SCREENHEIGHT-1);
    RectLeft(rc) = min(RectLeft(rc), SCREENWIDTH-1);
    RectTop(rc) = min(RectTop(rc), SCREENHEIGHT-1);
    return rc;
}

/* --- get the video memory that is to be used by a window -- */
void GetVideoBuffer(WINDOW wnd)
{
    RECT rc;
    int ht;
    int wd;

    rc = AdjustRect(wnd);
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
        RECT rc = AdjustRect(wnd);
        storevideo(rc, wnd->videosave);
        free(wnd->videosave);
        wnd->videosave = NULL;
    }
}

/* ------- compute the logical line length of a window ------ */
int LineLength(char *ln)
{
    int len = strlen(ln);
    char *cp = ln;
    while ((cp = strchr(cp, CHANGECOLOR)) != NULL)    {
        cp++;
        len -= 3;
    }
    cp = ln;
    while ((cp = strchr(cp, RESETCOLOR)) != NULL)    {
        cp++;
        --len;
    }
    return len;
}

