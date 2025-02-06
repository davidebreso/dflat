/* ------------- config.c ------------- */

#include <stdio.h>
#include <conio.h>
#include "dfast.h"

/* ----- default colors for color video system ----- */
struct colors color = {
    LIGHTGRAY, BLUE,  /* Application   */
    LIGHTGRAY, BLACK, /* Normal        */
    BLACK, CYAN,      /* Button        */
    WHITE, CYAN,      /* ButtonSel     */
    LIGHTGRAY, BLUE,  /* Dialog        */
    YELLOW, RED,      /* ErrorBox      */
    BLACK, LIGHTGRAY, /* MessageBox    */
    BLACK, LIGHTGRAY, /* HelpBox       */
    WHITE, CYAN,      /* InFocusTitle  */
    BLACK, CYAN,      /* Title         */
    GREEN, LIGHTGRAY, /* Dummy         */
    BLACK, LIGHTGRAY, /* TextBox       */
    LIGHTGRAY, BLACK, /* TextBoxSel    */
    LIGHTGRAY, BLUE,  /* TextBoxFrame  */
    BLACK, LIGHTGRAY, /* ListBox       */
    LIGHTGRAY, BLACK, /* ListBoxSel    */
    LIGHTGRAY, BLUE,  /* ListBoxFrame  */
    BLACK, LIGHTGRAY, /* EditBox       */
    LIGHTGRAY, BLACK, /* EditBoxSel    */
    LIGHTGRAY, BLUE,  /* EditBoxFrame  */
    BLACK, LIGHTGRAY, /* MenuBar       */
    BLACK, CYAN,      /* MenuBarSel    */
    BLACK, CYAN,      /* PopDown       */
    BLACK, LIGHTGRAY, /* PopDownSel    */
    DARKGRAY,         /* InactiveSelFG */
    RED               /* ShortCutFG    */
};

/* ----- default colors for mono video system ----- */
struct colors bw = {
    LIGHTGRAY, BLACK, /* Application   */
    LIGHTGRAY, BLACK, /* Normal        */
    BLACK, LIGHTGRAY, /* Button        */
    WHITE, LIGHTGRAY, /* ButtonSel     */
    LIGHTGRAY, BLACK, /* Dialog        */
    LIGHTGRAY, BLACK, /* ErrorBox      */
    LIGHTGRAY, BLACK, /* MessageBox    */
    BLACK, LIGHTGRAY, /* HelpBox       */
    BLACK, LIGHTGRAY, /* InFocusTitle  */
    BLACK, LIGHTGRAY, /* Title         */
    BLACK, LIGHTGRAY, /* Dummy         */
    LIGHTGRAY, BLACK, /* TextBox       */
    BLACK, LIGHTGRAY, /* TextBoxSel    */
    LIGHTGRAY, BLACK, /* TextBoxFrame  */
    LIGHTGRAY, BLACK, /* ListBox       */
    BLACK, LIGHTGRAY, /* ListBoxSel    */
    LIGHTGRAY, BLACK, /* ListBoxFrame  */
    LIGHTGRAY, BLACK, /* EditBox       */
    BLACK, LIGHTGRAY, /* EditBoxSel    */
    LIGHTGRAY, BLACK, /* EditBoxFrame  */
    LIGHTGRAY, BLACK, /* MenuBar       */
    BLACK, LIGHTGRAY, /* MenuBarSel    */
    BLACK, LIGHTGRAY, /* PopDown       */
    LIGHTGRAY, BLACK, /* PopDownSel    */
    DARKGRAY,         /* InactiveSelFG */
    WHITE             /* ShortCutFG    */
};

/* ------ default configuration values ------- */
CONFIG cfg = {
    FALSE,           /* mono                  */
    TRUE,            /* Editor Insert Mode    */
    4,               /* Editor tab stops      */
    TRUE             /* Editor word wrap      */
};

/* ------ load a configuration file from disk ------- */
void LoadConfig(void)
{
    FILE *fp = fopen(DFLAT_APPLICATION ".cfg", "rb");
    if (fp != NULL)    {
        fread(&cfg, sizeof(CONFIG), 1, fp);
        fclose(fp);
    }
}

/* ------ save a configuration file to disk ------- */
void SaveConfig(void)
{
    FILE *fp = fopen(DFLAT_APPLICATION ".cfg", "wb");
    if (fp != NULL)    {
        /*****
        cfg.InsertMode = GetCommandToggle(ID_INSERT);
        cfg.WordWrap = GetCommandToggle(ID_WRAP);
        *****/
        fwrite(&cfg, sizeof(CONFIG), 1, fp);
        fclose(fp);
    }
}

