/* ----------- console.c ---------- */

#include "dflat.h"

/* ----- cursor position and shape ------ */
struct curpos_t {
    unsigned shape;
    union {
        unsigned xy;
        struct {
            unsigned char x;
            unsigned char y;
        } cp;
    } pos;
};

/* ----- table of alt keys for finding shortcut keys ----- */
static int altconvert[] = {
    ALT_A,ALT_B,ALT_C,ALT_D,ALT_E,ALT_F,ALT_G,ALT_H,
    ALT_I,ALT_J,ALT_K,ALT_L,ALT_M,ALT_N,ALT_O,ALT_P,
    ALT_Q,ALT_R,ALT_S,ALT_T,ALT_U,ALT_V,ALT_W,ALT_X,
    ALT_Y,ALT_Z,ALT_0,ALT_1,ALT_2,ALT_3,ALT_4,ALT_5,
    ALT_6,ALT_7,ALT_8,ALT_9
};

unsigned video_mode;
unsigned video_page;
unsigned video_card;

unsigned char SCREENWIDTH;
unsigned char SCREENHEIGHT;

static int near cursorpos[MAXSAVES];
static int near cursorshape[MAXSAVES];
static int csp;

void SwapCursorStack(void)
{
	if (csp > 1)	{
		swap(cursorpos[csp-2], cursorpos[csp-1]);
		swap(cursorshape[csp-2], cursorshape[csp-1]);
	}
}

#ifdef __SMALLER_C__
#else
#ifndef MSC
#ifndef WATCOM
#define ZEROFLAG 0x40
/* ---- Test for keystroke ---- */
BOOL keyhit(void)
{
    _AH = 1;
    geninterrupt(KEYBRD);
    return (_FLAGS & ZEROFLAG) == 0;
}
#endif
#endif
#endif

/* ---- Read a keystroke ---- */
int getkey(void)
{
    int c;
    while (keyhit() == FALSE)
        ;
    if (((c = bioskey(0)) & 0xff) == 0)
        c = (c >> 8) | 0x1080;
    else
        c &= 0xff;
    return c & 0x10ff;
}

/* ---------- read the keyboard shift status --------- */
int getshift(void)
{
    asm {
        mov ah, 2
        int KEYBRD
        xor ah, ah
    }
}

static volatile int far *clk = MK_FP(0x40,0x6c);
/* ------- macro to wait one clock tick -------- */
#define wait()          \
{                       \
    int now = *clk;     \
    while (now == *clk) \
        ;               \
}

/* -------- sound a buzz tone ---------- */
void beep(void)
{
    wait();
    outp(0x43, 0xb6);               /* program the frequency */
    outp(0x42, (int) (COUNT % 256));
    outp(0x42, (int) (COUNT / 256));
    outp(0x61, inp(0x61) | 3);      /* start the sound */
    wait();
    outp(0x61, inp(0x61) & ~3);     /* stop the sound  */
}

/* -------- get the video mode and page from BIOS -------- */
void videomode(void)
{
    asm {
        mov ah, 0x0f
        int VIDEO
        and bx, 0xff00
        mov video_page, bx
        and ax, 0x7f
        mov video_mode, ax
    /* test for VGA */
        mov ax, 0x1a00
        int VIDEO
        cmp al, 0x1a
        jne testEGA
        cmp bl, 6
        jna testEGA
        mov video_card, VGA_VIDEO
        jmp testDone
    }
testEGA:
    asm {
        /* test for EGA */
        mov ah, 0x12
        mov bl, 0x10
        int VIDEO
        cmp bl, 0x10
        je testMDA
        mov video_card, EGA_VIDEO
        jmp testDone
    }
testMDA:
    /* test for MDA/CGA */
    if (video_mode == 7) {
        video_card = MDA_VIDEO;
    } else {
        video_card = CGA_VIDEO;
    }
testDone:
    SCREENWIDTH = (peekb(0x40,0x4a) & 255);
    SCREENHEIGHT = (video_card > MDA_VIDEO ? peekb(0x40,0x84)+1 : 25);
}

/* ------ position the cursor ------ */
void cursor(int x, int y)
{
    asm {
        mov ah, SETCURSOR
        mov dl, byte ptr x
        mov dh, byte ptr y
        mov bx, video_page
        int VIDEO
    }
}

/* ------ get cursor shape and position ------ */
static struct curpos_t near getcursor(void)
{
    asm {
        mov ah, READCURSOR
        mov bx, video_page
        int VIDEO
        xchg ax, cx
    }
    /* Return value:
     *   cursor position in dh, dl
     *   cursor shape in ax
     */
}

/* ------- get the current cursor position ------- */
void curr_cursor(int *x, int *y)
{
    struct curpos_t c;
    c = getcursor();
    *x = c.pos.cp.x;
    *y = c.pos.cp.y;
}

/* ------ save the current cursor configuration ------ */
void savecursor(void)
{
    if (csp < MAXSAVES)    {
        struct curpos_t c;
        c = getcursor();
        cursorshape[csp] = c.shape;
        cursorpos[csp] = c.pos.xy;
        csp++;
    }
}

/* ---- restore the saved cursor configuration ---- */
void restorecursor(void)
{
    if (csp)    {
        --csp;
        asm {
            lea si, cursorpos       /* SI = base address of array */
            mov bx, csp             /* BX = array index */
            shl bx, 1               /*   converted to byte count */
            mov dx, [si + bx]       /* DX = cursorpos[csp] */
            mov ah, SETCURSOR
            mov bx, video_page
            int VIDEO
        }
        set_cursor_type(cursorshape[csp]);
    }
}

/* ------ make a normal cursor ------ */
void normalcursor(void)
{
    set_cursor_type(0x0607);
}

/* ------ hide the cursor ------ */
void hidecursor(void)
{
    struct curpos_t c;
    c = getcursor();
    set_cursor_type(c.shape | HIDECURSOR);
}

/* ------ unhide the cursor ------ */
void unhidecursor(void)
{
    struct curpos_t c;

    c = getcursor();
    set_cursor_type(c.shape & ~HIDECURSOR);
}

/* ---- use BIOS to set the cursor type ---- */
void set_cursor_type(unsigned t)
{
    asm {
        mov ah, SETCURSORTYPE
        mov bx, video_page
        mov cx, t
        int VIDEO
    }
}

static void Scan350(void)
{
    asm {
        mov ax, 0x1201
        mov bl, 0x30
        int VIDEO
	    mov ah, 0x0f
        int VIDEO
	    xor ah, ah
        int VIDEO
    }
}

static void Scan400(void)
{
    asm {
        mov ax, 0x1202
        mov bl, 0x30
        int VIDEO
	    mov ah, 0x0f
        int VIDEO
	    xor ah, ah
        int VIDEO
    }
}

/* ---------- set 25 line mode ------- */
void Set25(void)
{
    if (isVGA())	{
        Scan400();
        asm {
            mov ax, 0x1114
            xor bl, bl
            int VIDEO
       }
	} else {
        asm {
            mov ax, 0x1111
            xor bl, bl
            int VIDEO
       }
    }
    SCREENHEIGHT = 25;
}

/* ---------- set 43 line mode ------- */
void Set43(void)
{
    if (isVGA())
        Scan350();
    asm {
        mov ax, 0x1112
        xor bl, bl
        int VIDEO
    }
    SCREENHEIGHT = 43;
}

/* ---------- set 50 line mode ------- */
void Set50(void)
{
    if (isVGA())
        Scan400();
    asm {
        mov ax, 0x1112
        xor bl, bl
        int VIDEO
    }
    SCREENHEIGHT = 50;
}

/* ------ convert an Alt+ key to its letter equivalent ----- */
int AltConvert(int c)
{
	int i, a = 0;
	for (i = 0; i < 36; i++)
		if (c == altconvert[i])
			break;
	if (i < 26)
		a = 'a' + i;
	else if (i < 36)
		a = '0' + i - 26;
	return a;
}

#if MSC | WATCOM
int getdisk(void)
{
	unsigned int cd;
	_dos_getdrive(&cd);
	cd -= 1;
	return cd;
}
#endif
