/* ---------------- classdef.c ---------------- */

#include <stdio.h>
#include "dfast.h"

/* Add class definitions to this table.
 * Add the class symbol to the CLASS list in dflat.h
 */

CLASSDEFS classdefs[] = {
    {   /* ---------- NORMAL Window Class ----------- */
        NORMAL,
        -1,
        &cfg.clr.NormalFG, &cfg.clr.NormalBG,
        NULL, NULL,
        &cfg.clr.NormalFG, &cfg.clr.NormalBG,
        NormalProc
    },
    {   /* ---------- APPLICATION Window Class ----------- */
        APPLICATION,
        NORMAL,
        &cfg.clr.ApplicationFG, &cfg.clr.ApplicationBG,
        NULL, NULL,
        &cfg.clr.ApplicationFG, &cfg.clr.ApplicationBG,
        ApplicationProc,
        VISIBLE | SAVESELF | CONTROLBOX | TITLEBAR | HASBORDER
    },
    {   /* ------------ TEXTBOX Window Class -------------- */
        TEXTBOX,
        NORMAL,
        &cfg.clr.TextBoxFG, &cfg.clr.TextBoxBG,
        &cfg.clr.TextBoxSelFG, &cfg.clr.TextBoxSelBG,
        &cfg.clr.TextBoxFrameFG, &cfg.clr.TextBoxFrameBG,
        TextBoxProc
    },
    {   /* ------------- LISTBOX Window class ------------- */
        LISTBOX,
        TEXTBOX,
        &cfg.clr.ListBoxFG, &cfg.clr.ListBoxBG,
        &cfg.clr.ListBoxSelFG, &cfg.clr.ListBoxSelBG,
        &cfg.clr.ListBoxFrameFG, &cfg.clr.ListBoxFrameBG,
        ListBoxProc
    },
    {   /* ------------- EDITBOX Window Class -------------- */
        EDITBOX,
        TEXTBOX,
        &cfg.clr.EditBoxFG, &cfg.clr.EditBoxBG,
        &cfg.clr.EditBoxSelFG, &cfg.clr.EditBoxSelBG,
        &cfg.clr.EditBoxFrameFG, &cfg.clr.EditBoxFrameBG,
        EditBoxProc
    },
    {   /* ------------- MENUBAR Window Class --------------- */
        MENUBAR,
        NORMAL,
        &cfg.clr.MenuBarFG, &cfg.clr.MenuBarBG,
        &cfg.clr.MenuBarSelFG, &cfg.clr.MenuBarSelBG,
        NULL, NULL,
        MenuBarProc,
        VISIBLE
    },
    {   /* ------------- POPDOWNMENU Window Class ----------- */
        POPDOWNMENU,
        LISTBOX,
        &cfg.clr.PopDownFG, &cfg.clr.PopDownBG,
        &cfg.clr.PopDownSelFG, &cfg.clr.PopDownSelBG,
        NULL, NULL,
        PopDownProc,
        SAVESELF | NOCLIP | HASBORDER
    },
    {   /* ----------- BUTTON Window Class --------------- */
        BUTTON,
        TEXTBOX,
        &cfg.clr.ButtonFG, &cfg.clr.ButtonBG,
        &cfg.clr.ButtonSelFG, &cfg.clr.ButtonSelBG,
        NULL, NULL,
        ButtonProc,
        SHADOW
    },
    {   /* ------------- DIALOG Window Class -------------- */
        DIALOG,
        NORMAL,
        &cfg.clr.DialogFG, &cfg.clr.DialogBG,
        NULL, NULL,
        &cfg.clr.DialogFG, &cfg.clr.DialogBG,
        DialogProc,
        SHADOW | MOVEABLE | SAVESELF | CONTROLBOX | HASBORDER
    },
    {   /* ------------ ERRORBOX Window Class ----------- */
        ERRORBOX,
        DIALOG,
        &cfg.clr.ErrorBoxFG, &cfg.clr.ErrorBoxBG,
        NULL, NULL,
        &cfg.clr.ErrorBoxFG, &cfg.clr.ErrorBoxBG,
        DialogProc,
        SHADOW | HASBORDER
    },
    {   /* --------- MESSAGEBOX Window Class ------------- */
        MESSAGEBOX,
        DIALOG,
        &cfg.clr.MessageBoxFG, &cfg.clr.MessageBoxBG,
        NULL, NULL,
        &cfg.clr.MessageBoxFG, &cfg.clr.MessageBoxBG,
        DialogProc,
        SHADOW | HASBORDER
    },
    {   /* ----------- HELPBOX Window Class --------------- */
        HELPBOX,
        DIALOG,
        &cfg.clr.HelpBoxFG, &cfg.clr.HelpBoxBG,
        NULL, NULL,
        &cfg.clr.HelpBoxFG, &cfg.clr.HelpBoxBG,
        DialogProc,
        SHADOW | HASBORDER
    },
    {   /* -------------- DUMMY Window Class ---------------- */
        DUMMY,
        -1,
        &cfg.clr.DummyFG, &cfg.clr.DummyBG,
        NULL, NULL,
        &cfg.clr.DummyFG, &cfg.clr.DummyBG,
        NULL,
        HASBORDER
    }
};

/* ------- return the offset of a class into the class
                 definition table ------ */
int FindClass(CLASS class)
{
    int i;
    for (i = 0; i < sizeof(classdefs) / sizeof(CLASSDEFS); i++)
        if (class == classdefs[i].class)
            return i;
    return 0;
}


