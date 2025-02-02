/* ---------------- video.h ----------------- */

#ifndef VIDEO_H
#define VIDEO_H

#include "rect.h"

void getvideo(RECT, void far *);
void storevideo(RECT, void far *);
extern unsigned video_mode;
extern unsigned video_page;
extern unsigned video_address;
void wputch(WINDOW, int, int, int);
int GetVideoChar(int, int);
void PutVideoChar(int, int, int);
void PutVideoStr(int, int, char *, int);
void get_videomode(void);
void wputs(WINDOW, void *, int, int);

#define clr(fg,bg) ((fg)|((bg)<<4))
#define vad(x,y) ((y)*160+(x)*2)
#define ismono() (video_mode == 7)
#define istext() (video_mode < 4)
#define videochar(x,y) (GetVideoChar(x,y) & 255)

#endif

