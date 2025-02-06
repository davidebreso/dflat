/* ---------------- config.h -------------- */

#ifndef CONFIG_H
#define CONFIG_H

#define DFLAT_APPLICATION "memopad"

struct colors {
    /* ------------ colors ------------ */
    char ApplicationFG,  ApplicationBG;
    char NormalFG,       NormalBG;
    char ButtonFG,       ButtonBG;
    char ButtonSelFG,    ButtonSelBG;
    char DialogFG,       DialogBG;
    char ErrorBoxFG,     ErrorBoxBG;
    char MessageBoxFG,   MessageBoxBG;
    char HelpBoxFG,      HelpBoxBG;
    char InFocusTitleFG, InFocusTitleBG;
    char TitleFG,        TitleBG;
    char DummyFG,        DummyBG;
    char TextBoxFG,      TextBoxBG;
    char TextBoxSelFG,   TextBoxSelBG;
    char TextBoxFrameFG, TextBoxFrameBG;
    char ListBoxFG,      ListBoxBG;
    char ListBoxSelFG,   ListBoxSelBG;
    char ListBoxFrameFG, ListBoxFrameBG;
    char EditBoxFG,      EditBoxBG;
    char EditBoxSelFG,   EditBoxSelBG;
    char EditBoxFrameFG, EditBoxFrameBG;
    char MenuBarFG,      MenuBarBG;
    char MenuBarSelFG,   MenuBarSelBG;
    char PopDownFG,      PopDownBG;
    char PopDownSelFG,   PopDownSelBG;
    char InactiveSelFG;
    char ShortCutFG;
};

/* ----------- configuration parameters ----------- */
typedef struct config {
    char mono;         /* True for B/W screens on any monitor */
    int InsertMode;    /* Editor insert mode                  */
    int Tabs;          /* Editor tab stops                    */
    int WordWrap;      /* True to word wrap editor            */
    struct colors clr; /* Colors                              */
} CONFIG;

extern CONFIG cfg;
extern struct colors color, bw;

void LoadConfig(void);
void SaveConfig(void);

#endif

