/* ----------- message.h ------------ */

#ifndef MESSAGES_H
#define MESSGAES_H

#define MAXMESSAGES 50
#define DELAYTICKS 1
#define FIRSTDELAY 7
#define DOUBLETICKS 5

typedef enum messages {
    /* ------------- process communication messages --------- */
    START,                  /* start message processing       */
    STOP,                   /* stop message processing        */
    COMMAND,                /* send a command to a window     */
    /* ------------- window management messages ------------- */
    CREATE_WINDOW,          /* create a window                */
    SHOW_WINDOW,            /* show a window                  */
    HIDE_WINDOW,            /* hide a window                  */
    CLOSE_WINDOW,           /* delete a window                */
    SETFOCUS,               /* set and clear the focus        */
    PAINT,                  /* paint the window's data space  */
    BORDER,                 /* paint the window's border      */
    TITLE,                  /* display the window's title     */
    MOVE,                   /* move the window                */
    SIZE,                   /* change the window's size       */
    MAXIMIZE,               /* maximize the window            */
    MINIMIZE,               /* minimize the window            */
    RESTORE,                /* restore the window             */
    INSIDE_WINDOW,          /* test x/y inside a window       */
    /* ------------- clock messages ------------------------- */
    CLOCKTICK,              /* the clock ticked               */
    CAPTURE_CLOCK,          /* capture clock into a window    */
    RELEASE_CLOCK,          /* release clock to the system    */
    /* ------------- keyboard and screen messages ----------- */
    KEYBOARD,               /* key was pressed                */
    CAPTURE_KEYBOARD,       /* capture keyboard into a window */
    RELEASE_KEYBOARD,       /* release keyboard to system     */
    KEYBOARD_CURSOR,        /* position the keyboard cursor   */
    CURRENT_KEYBOARD_CURSOR,/* read the cursor position       */
    HIDE_CURSOR,            /* hide the keyboard cursor       */
    SHOW_CURSOR,            /* display the keyboard cursor    */
    SAVE_CURSOR,            /* save the cursor's configuration*/
    RESTORE_CURSOR,         /* restore the saved cursor       */
    SHIFT_CHANGED,          /* the shift status changed       */
    /* ------------- mouse messages ------------------------- */
    MOUSE_INSTALLED,        /* test for mouse installed       */
    RIGHT_BUTTON,           /* right button pressed           */
    LEFT_BUTTON,            /* left button pressed            */
    DOUBLE_CLICK,           /* right button double-clicked    */
    MOUSE_MOVED,            /* mouse changed position         */
    BUTTON_RELEASED,        /* mouse button released          */
    CURRENT_MOUSE_CURSOR,   /* get mouse position             */
    MOUSE_CURSOR,           /* set mouse position             */
    SHOW_MOUSE,             /* make mouse cursor visible      */
    HIDE_MOUSE,             /* hide mouse cursor              */
    WAITMOUSE,              /* wait until button released     */
    TESTMOUSE,              /* test any mouse button pressed  */
    CAPTURE_MOUSE,          /* capture mouse into a window    */
    RELEASE_MOUSE,          /* release the mouse to system    */
    /* ------------- text box messages ---------------------- */
    ADDTEXT,                /* add text to the text box       */
    CLEARTEXT,              /* clear the edit box             */
    SETTEXT,                /* set address of text buffer     */
    SCROLL,                 /* vertical scroll of text box    */
    HORIZSCROLL,            /* horizontal scroll of text box  */
    /* ------------- edit box messages ---------------------- */
    EB_GETTEXT,             /* get text from an edit box      */
    EB_PUTTEXT,             /* put text into an edit box      */
    /* ------------- menubar messages ----------------------- */
    BUILDMENU,              /* build the menu display         */
    SELECTION,              /* menubar selection              */
    /* ------------- popdown messages ----------------------- */
    BUILD_SELECTIONS,       /* build the menu display         */
    CLOSE_POPDOWN,          /* tell parent popdown is closing */
    /* ------------- list box messages ---------------------- */
    LB_SELECTION,           /* sent to parent on selection    */
    LB_CHOOSE,              /* sent when user chooses         */
    LB_CURRENTSELECTION,    /* return the current selection   */
    LB_GETTEXT,             /* return the text of selection   */
    LB_SETSELECTION,        /* sets the listbox selection     */
    /* ------------- dialog box messages -------------------- */
    INITIATE_DIALOG,        /* begin a dialog                 */
    ENTERFOCUS,             /* tell DB control got focus      */
    LEAVEFOCUS,             /* tell DB control lost focus     */
    ENDDIALOG               /* end a dialog                   */
} MESSAGE;

/* --------- message prototypes ----------- */
void init_messages(void);
void PostMessage(WINDOW, MESSAGE, PARAM, PARAM);
int SendMessage(WINDOW, MESSAGE, PARAM, PARAM);
int dispatch_message(void);
int TestCriticalError(void);

#endif


