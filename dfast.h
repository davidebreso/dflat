/* ------------- dflat.h ----------- */

#ifndef WINDOW_H
#define WINDOW_H

#define TRUE 1
#define FALSE 0

#include "system.h"
#include "rect.h"
#include "keys.h"
/*****
#include "menu.h"
#include "commands.h"
#include "config.h"
#include "dialbox.h"
*****/

/* ------ integer type for message parameters ----- */
typedef long PARAM;
typedef enum window_class    {
    NORMAL,
    APPLICATION,
    TEXTBOX,
    LISTBOX,
    EDITBOX,
    MENUBAR,
    POPDOWNMENU,
    BUTTON,
    DIALOG,
    ERRORBOX,
    MESSAGEBOX,
    HELPBOX,
    TEXT,
    RADIOBUTTON,
    DUMMY
} CLASS;
typedef struct window {
    CLASS class;           /* window class                  */
    char *title;           /* window title                  */
    struct window *parent; /* parent window                 */
    int (*wndproc)
        (struct window *, enum messages, PARAM, PARAM);
    /* ---------------- window dimensions ----------------- */
    RECT rc;               /* window coordinates
                                            (0/0 to 79/24)  */
    int ht, wd;            /* window height and width       */
    RECT RestoredRC;       /* restored condition rect       */
    /* -------------- linked list pointers ---------------- */
    struct window *next;        /* next window on screen    */
    struct window *prev;        /* previous window on screen*/
    struct window *nextbuilt;   /* next window built        */
    struct window *prevbuilt;   /* previous window built    */

    int attrib;                 /* Window attributes        */
    char *videosave;            /* video save buffer        */
    int condition;              /* Restored, Maximized,
                                           Minimized        */
    void *extension;            /* -> menus, dialog box, etc*/
    struct window *PrevMouse;
    struct window *PrevKeyboard;
    /* ----------------- text box fields ------------------ */
    int wlines;     /* number of lines of text              */
    int wtop;       /* text line that is on the top display */
    char *text;     /* window text                          */
    int textlen;    /* text length                          */
    int wleft;      /* left position in window viewport     */
    int textwidth;  /* width of longest line in textbox     */
    int BlkBegLine; /* beginning line of marked block       */
    int BlkBegCol;  /* beginning column of marked block     */
    int BlkEndLine; /* ending line of marked block          */
    int BlkEndCol;  /* ending column of marked block        */
    int HScrollBox; /* position of horizontal scroll box    */
    int VScrollBox; /* position of vertical scroll box      */
    /* ------------------ list box field ------------------ */
    int selection;  /* current selection                    */
    /* ----------------- edit box fields ------------------ */
    int CurrCol;    /* Current column                       */
    char *CurrLine; /* Current line                         */
    int WndRow;     /* Current window row                   */
    int TextChanged; /* TRUE if text has changed            */
    char *DeletedText; /* for undo                          */
    int DeletedLength; /*  "   "                            */
    /* ---------------- dialog box fields ----------------- */
    struct window *dFocus; /* control that has the focus    */
    int ReturnCode;        /* return code from a dialog box */
} * WINDOW;

/****
#include "message.h"
#include "classdef.h"
****/
#include "video.h"

enum Condition     {
    ISRESTORED, ISMINIMIZED, ISMAXIMIZED
};
/* ------- window methods ----------- */
#define WindowHeight(w)      ((w)->ht)
#define WindowWidth(w)       ((w)->wd)
/*****
#define BorderAdj(w,n)       (TestAttribute(w,HASBORDER)?n:0)
*****/
#define BorderAdj(w,n)       (0)
#define ClientWidth(w)       (WindowWidth(w)-BorderAdj(w,2))
#define ClientHeight(w)      (WindowHeight(w)-BorderAdj(w,2))
#define WindowRect(w)        ((w)->rc)
#define GetTop(w)            (RectTop(WindowRect(w)))
#define GetBottom(w)         (RectBottom(WindowRect(w)))
#define GetLeft(w)           (RectLeft(WindowRect(w)))
#define GetRight(w)          (RectRight(WindowRect(w)))
#define GetClientTop(w)      (GetTop(w)+BorderAdj(w,1))
#define GetClientBottom(w)   (GetBottom(w)-BorderAdj(w,1))
#define GetClientLeft(w)     (GetLeft(w)+BorderAdj(w,1))
#define GetClientRight(w)    (GetRight(w)-BorderAdj(w,1))
#define GetParent(w)         ((w)->parent)
#define GetTitle(w)          ((w)->title)
#define NextWindow(w)        ((w)->next)
#define PrevWindow(w)        ((w)->prev)
#define NextWindowBuilt(w)   ((w)->nextbuilt)
#define PrevWindowBuilt(w)   ((w)->prevbuilt)
#define GetClass(w)          ((w)->class)
#define GetAttribute(w)      ((w)->attrib)
#define AddAttribute(w,a)    (GetAttribute(w) |= a)
#define ClearAttribute(w,a)  (GetAttribute(w) &= ~(a))
#define TestAttribute(w,a)   (GetAttribute(w) & (a))
#define isVisible(w)         (GetAttribute(w) & VISIBLE)
#define SetVisible(w)        (GetAttribute(w) |= VISIBLE)
#define ClearVisible(w)      (GetAttribute(w) &= ~VISIBLE)
#define gotoxy(w,x,y) cursor(w->rc.lf+(x)+1,w->rc.tp+(y)+1)
WINDOW CreateWindow(CLASS,char *,int,int,int,int,void*,WINDOW,
        int (*)(struct window *,enum messages,PARAM,PARAM),int);
void AddTitle(WINDOW, char *);
void RepaintBorder(WINDOW, RECT *);
void ClearWindow(WINDOW, RECT *, int);
void clipline(WINDOW, int, char *);
void writeline(WINDOW, char *, int, int, int);
void writefull(WINDOW, char *, int);
void SetNextFocus(WINDOW,int);
void PutWindowChar(WINDOW, int, int, int);
void GetVideoBuffer(WINDOW);
void RestoreVideoBuffer(WINDOW);
int LineLength(char *);
#define DisplayBorder(wnd) RepaintBorder(wnd, NULL)
#define DefaultWndProc(wnd,msg,p1,p2)    \
    classdefs[FindClass(wnd->class)].wndproc(wnd,msg,p1,p2)
#define BaseWndProc(class,wnd,msg,p1,p2)    \
    classdefs[DerivedClass(class)].wndproc(wnd,msg,p1,p2)
#define NULLWND ((WINDOW) 0)
struct LinkedList    {
    WINDOW FirstWindow;
    WINDOW LastWindow;
};
extern struct LinkedList Focus;
extern struct LinkedList Built;
extern WINDOW inFocus;
extern WINDOW CaptureMouse;
extern WINDOW CaptureKeyboard;
extern int foreground, background;
extern int WindowMoving;
extern int WindowSizing;
extern int TextMarking;
extern char *Clipboard;
extern WINDOW SystemMenuWnd;
/* --------------- border characters ------------- */
#define FOCUS_NW       '\xc9'
#define FOCUS_NE       '\xbb'
#define FOCUS_SE       '\xbc'
#define FOCUS_SW       '\xc8'
#define FOCUS_SIDE     '\xba'
#define FOCUS_LINE     '\xcd'
#define NW             '\xda'
#define NE             '\xbf'
#define SE             '\xd9'
#define SW             '\xc0'
#define SIDE           '\xb3'
#define LINE           '\xc4'
#define LEDGE          '\xc3'
#define REDGE          '\xb4'
#define SHADOWFG       DARKGRAY
/* ------------- scroll bar characters ------------ */
#define UPSCROLLBOX    '\x1e'
#define DOWNSCROLLBOX  '\x1f'
#define LEFTSCROLLBOX  '\x11'
#define RIGHTSCROLLBOX '\x10'
#define SCROLLBARCHAR  176
#define SCROLLBOXCHAR  178
#define CHECKMARK      251      /* menu item toggle         */
/* ----------------- title bar characters ----------------- */
#define CONTROLBOXCHAR '\xf0'
#define MAXPOINTER     24      /* maximize token            */
#define MINPOINTER     25      /* minimize token            */
#define RESTOREPOINTER 18      /* restore token             */
/* --------------- text control characters ---------------- */
#define APPLCHAR     176    /* fills application window     */
#define SHORTCUTCHAR '~'    /* prefix: shortcut key display */
#define CHANGECOLOR  174    /* prefix to change colors      */
#define RESETCOLOR   175    /* reset colors to default      */
/* ---- standard window message processing prototypes ----- */
/******
int ApplicationProc(WINDOW, MESSAGE, PARAM, PARAM);
int NormalProc(WINDOW, MESSAGE, PARAM, PARAM);
int TextBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int ListBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int EditBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int MenuBarProc(WINDOW, MESSAGE, PARAM, PARAM);
int PopDownProc(WINDOW, MESSAGE, PARAM, PARAM);
int ButtonProc(WINDOW, MESSAGE, PARAM, PARAM);
int DialogProc(WINDOW, MESSAGE, PARAM, PARAM);
int SystemMenuProc(WINDOW, MESSAGE, PARAM, PARAM);
int HelpBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int MessageBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
******/
/* ------------- normal box prototypes ------------- */
int isWindow(WINDOW);
WINDOW inWindow(int, int);
int WndForeground(WINDOW);
int WndBackground(WINDOW);
int FrameForeground(WINDOW);
int FrameBackground(WINDOW);
int SelectForeground(WINDOW);
int SelectBackground(WINDOW);
void SetStandardColor(WINDOW);
void SetReverseColor(WINDOW);
void SetClassColors(CLASS);
WINDOW GetFirstChild(WINDOW);
WINDOW GetNextChild(WINDOW);
WINDOW GetLastChild(WINDOW);
WINDOW GetPrevChild(WINDOW);
#define HitControlBox(wnd, p1, p2)     \
    (TestAttribute(wnd, TITLEBAR)   && \
     TestAttribute(wnd, CONTROLBOX) && \
     p1 == 2 && p2 == 0)
/* -------- text box prototypes ---------- */
char *TextLine(WINDOW, int);
void WriteTextLine(WINDOW, RECT *, int, int);
void SetTextBlock(WINDOW, int, int, int, int);
#define BlockMarked(wnd) (  wnd->BlkBegLine ||    \
                            wnd->BlkEndLine ||    \
                            wnd->BlkBegCol  ||    \
                            wnd->BlkEndCol)
#define ClearBlock(wnd) wnd->BlkBegLine = wnd->BlkEndLine =  \
                        wnd->BlkBegCol  = wnd->BlkEndCol = 0;
#define GetText(w)        ((w)->text)
/* --------- menu prototypes ---------- */
/******
int CopyCommand(char *, char *, int, int);
void PrepOptionsMenu(void *, struct Menu *);
void PrepEditMenu(void *, struct Menu *);
void PrepWindowMenu(void *, struct Menu *);
void BuildSystemMenu(WINDOW);
******/
/* ------------- edit box prototypes ----------- */
#define isMultiLine(wnd)     TestAttribute(wnd, MULTILINE)
/* --------- message box prototypes -------- */
void MessageBox(char *, char *);
void ErrorMessage(char *);
int TestErrorMessage(char *);
int YesNoBox(char *);
int MsgHeight(char *);
int MsgWidth(char *);
/* ------------- dialog box prototypes -------------- */
/******
int DialogBox(DBOX *, int (*)(struct window *,
                        enum messages, PARAM, PARAM));
int DlgOpenFile(char *, char *);
int DlgSaveAs(char *);
void GetDlgListText(WINDOW, char *, enum commands);
int DlgDirList(WINDOW, char *, enum commands,
                            enum commands, unsigned);
int RadioButtonSetting(DBOX *, enum commands);
void PushRadioButton(DBOX *, enum commands);
void PutItemText(WINDOW, enum commands, char *);
void GetItemText(WINDOW, enum commands, char *, int);
*****/
/* ------------- help box prototypes ------------- */
void HelpFunction(void);
void LoadHelpFile(void);
#define swap(a,b){int x=a;a=b;b=x;}

#endif


