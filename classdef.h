/* ---------------- classdef.h --------------- */

#ifndef CLASSDEF_H
#define CLASSDEF_H

typedef struct classdefs {
    CLASS class;                        /* window class      */
    CLASS base;                         /* base window class */
    char *fg,*bg,*sfg,*sbg,*ffg,*fbg;   /* colors            */
    int (*wndproc)(struct window *,enum messages,PARAM,PARAM);
    int attrib;
} CLASSDEFS;

extern CLASSDEFS classdefs[];

#define SHADOW      0x0001
#define MOVEABLE    0x0002
#define SIZEABLE    0x0004
#define HASMENUBAR  0x0008
#define VSCROLLBAR  0x0010
#define HSCROLLBAR  0x0020
#define VISIBLE     0x0040
#define SAVESELF    0x0080
#define TITLEBAR    0x0100
#define CONTROLBOX  0x0200
#define MINMAXBOX   0x0400
#define NOCLIP      0x0800
#define READONLY    0x1000
#define MULTILINE   0x2000
#define HASBORDER   0x4000

int FindClass(CLASS);
#define DerivedClass(class) (classdefs[FindClass(class)].base)

#endif


