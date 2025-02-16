/* ---------------- video.h ----------------- */

#ifndef VIDEO_H
#define VIDEO_H

#include "rect.h"

void getvideo(RECT, void far *);
void storevideo(RECT, void far *);
extern unsigned video_mode;
extern unsigned video_page;
extern unsigned video_address;
int GetVideoChar(int, int);
void PutVideoChar(int, int, int);
int PutVideoStr(int, int, char *, int);
void FillVideoRect(RECT, int);
void ColorVideoRect(RECT, int);
void get_videomode(void);

#define clr(fg,bg) ((fg)|((bg)<<4))
#define ismono() (video_mode == 7)
#define istext() (video_mode < 4)
#define videochar(x,y) (GetVideoChar(x,y) & 255)

#endif

