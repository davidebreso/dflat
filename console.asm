 %TITLE "Low level console routines for D-Fast"

        IDEAL

        MODEL   large

;-----------------------------------------------------------------------
; Equates
;-----------------------------------------------------------------------
VIDEO           EQU     010h            ; Video Interrupt
KEYBRD          EQU     016h            ; Keyboard interrupt
FREQ            EQU     440             ; Beep frequency in Hz
COUNT           EQU     1193180 / FREQ  ; Beep frequency count
DELAY           EQU     2               ; Beep length in clock ticks
MAXSAVES        EQU     50              ; Max number of saved cursors

;------ Tell assembler where the DailyTimer word is located

BIOSDataSeg     EQU     0040h           ; Segment address of BDA
DailyTimerOfs   EQU     006ch           ; Offset of timer counter

SEGMENT BIOSData at BIOSDataSeg
        ORG     DailyTimerOfs
LABEL   DailyTimer      Word
ENDS


DATASEG

        PUBLIC  _video_mode, _video_page, _video_address

csave           dw      0
cursorpos       dw      MAXSAVES    dup(?)
cursorshape     dw      MAXSAVES    dup(?)
_video_page     dw      0
_video_mode     dw      ?
_video_address  dw      ?


;-----------------------------------------------------------------------
; Define code segment, shared with mouse.asm and video.asm
;-----------------------------------------------------------------------
SEGMENT CONSOLE_TEXT Word Public 'CODE'

ASSUME  CS:CONSOLE_TEXT

        PUBLIC  _get_videomode, _getkey, _getshift, _beep
        PUBLIC  _cursor, _curr_cursor, _savecursor, _restorecursor
        PUBLIC  _hidecursor, _unhidecursor, _set_cursor_type


%NEWPAGE
;-----------------------------------------------------------------------
; void get_videomode(void)      Init the video and console subsystem
;-----------------------------------------------------------------------
PROC    _get_videomode

        mov     ah, 0fh             ; Select GET VIDEOMODE function
        int     VIDEO               ;  and call Video BIOS
        and     ax, 07fh            ; Mask out video mode
        mov     [_video_mode], ax   ;  and save it
        and     bx, 0ff00h          ; Mask out video page
        mov     [_video_page], bx   ;  and save it
        cmp     ax, 7               ; Monochrome mode?
        jne     @@10                ;  no, jump
        mov     bx, 0b000h          ; Set monochrome video address
        jmp     @@99                ;  and jump to save address
@@10:
        add     bx, 0b800h          ; video_address is b800 + video_page
@@99:
        mov     [_video_address], bx    ; Save video_address
        ret                         ; Return to caller
ENDP    _get_videomode
%NEWPAGE
;-----------------------------------------------------------------------
; int getkey(void)          Read a keystroke
;-----------------------------------------------------------------------
PROC    _getkey

        xor     ah, ah              ; Select GET KEYSTROKE function
        int     KEYBRD              ;  and call keyboard BIOS
        or      al, al              ; If ASCII character
        jne     @@99                ;  then jump
        xchg    al, ah              ; Else put scancode in AL
        or      al, 080h            ;  and set bit 7 to 1
@@99:
        xor     ah, ah              ; Extend return value to 16 bits
        ret                         ; Return to caller
ENDP    _getkey
%NEWPAGE
;-----------------------------------------------------------------------
; int getshift(void)        Read the keyboard shift status
;-----------------------------------------------------------------------
PROC    _getshift

        mov     ah, 2               ; Select GET SHIFT FLAGS function
        int     KEYBRD              ;  and call keyboard BIOS
        xor     ah, ah              ; Extend return value to 16 bits
        ret                         ; Return to caller
ENDP    _getshift
%NEWPAGE
;-----------------------------------------------------------------------
; timer_wait    Wait for CX clock ticks
;-----------------------------------------------------------------------
PROC    timer_wait      NEAR

        push    ds              ; Save data segment register
        mov     ax, BIOSDataSeg ; Address BIOSDataSegment
        mov     ds, ax          ;  with ds

ASSUME  DS:BIOSData

        mov     ax, [DailyTimer]    ; Get timer count
        add     ax, cx              ;  add delay
@@10:
        cmp     ax, [DailyTimer]    ; Wait until
        ja      @@10                ;  delay expires

ASSUME  DS:DGROUP

        pop     ds              ; Restore data segment register
        ret                     ; Return to caller
ENDP    timer_wait
%NEWPAGE
;-----------------------------------------------------------------------
; void beep(void)           Sound a buzz tone
;-----------------------------------------------------------------------
PROC    _beep

        mov     cx, DELAY       ; Wait for
        call    timer_wait      ;  DELAY clock ticks

        mov     al, 0b6h        ; Set up the control word register
        out     043h, al        ; Perform the write
        jmp     $+2             ; Wait a bit
        mov     ax, COUNT       ; Select frequency
        out     042h, al        ; Send low byte of frequency
        jmp     $+2             ; Wait a bit
        mov     al, ah          ; Send high byte
        out     042h, al        ;  of frequency
        jmp     $+2             ; Wait a bit
        in      al, 61h         ; Read the keyboard controller status
        jmp     $+2             ; Wait a bit
        or      al, 3           ; Turn on 0 and 1 bit, enabling PC speaker
        out     61h, al         ; Save the new keyboard status

        mov     cx, DELAY       ; Wait for
        call    timer_wait      ;  DELAY clock ticks

        in      al, 61h         ; Get the keyboard controller status
        jmp     $+2             ; Wait a bit
        and     al, 0fch        ; Turn off 0 and 1 bit to stop sound
        out     61h, al         ; Write the new keyboard status

        ret                     ; Return to caller
ENDP    _beep
%NEWPAGE
;-----------------------------------------------------------------------
; void cursor(int x, int y)         Position the screen cursor
;-----------------------------------------------------------------------
PROC    _cursor

        ARG     x:Byte, y:Byte

        push    bp                  ; Save old bp pointer
        mov     bp, sp              ; Access parameters

        mov     ah, 2               ; Select SET CURSOR POSITION function
        mov     bx, [_video_page]   ; Set video page
        mov     dl, [x]             ; Set column
        mov     dh, [y]             ; Set row
        int     VIDEO               ; Call Video BIOS

        pop     bp                  ; Restore saved bp
        ret                         ; Return to caller
ENDP    _cursor
%NEWPAGE
;-----------------------------------------------------------------------
; void curr_cursor(int *x, int *y)  Get the current cursor position
;-----------------------------------------------------------------------
PROC    _curr_cursor

        ARG     x:DWord, y:DWord

        push    bp                  ; Save old bp pointer
        mov     bp, sp              ; Access parameters

        mov     ah, 3               ; Select GET CURSOR function
        mov     bx, [_video_page]   ; Select video page
        int     VIDEO               ; Call Video BIOS
        xor     ax, ax              ; Set AX to zero
        xchg    al, dh              ; Set AX to row, and DX to column
        les     bx, [x]             ; Point ES:BX to x
        mov     [es:bx], dx         ;  and save column
        les     bx, [y]             ; Point ES:BX to y
        mov     [es:bx], ax         ;  and save row

        pop     bp                  ; Restore bp pointer
        ret                         ; Return to caller
ENDP    _curr_cursor
%NEWPAGE
;-----------------------------------------------------------------------
; void savecursor(void)     Save the current cursor configuration
;-----------------------------------------------------------------------
PROC    _savecursor

        cmp     [csave], MAXSAVES   ; There is space to save?
        jae     @@99                ;  if not, return

        mov     ah, 3               ; Select GET CURSOR function
        int     VIDEO               ; Call Video BIOS
        mov     bx, [csave]         ; BX is save index
        shl     bx, 1               ;  converted to byte count
        mov     [bx + cursorshape], cx      ; Save cursor shape
        mov     [bx + cursorpos], dx        ; Save cursor position
        inc     [csave]             ; Increment save index

@@99:
        ret                         ; Return to caller
ENDP    _savecursor
%NEWPAGE
;-----------------------------------------------------------------------
; void restorecursor(void)      Restore the saved cursor configuration
;-----------------------------------------------------------------------
PROC    _restorecursor

        cmp     [csave], 0      ; There are saved configurations?
        je      @@99            ;  if not, return

        dec     [csave]         ; Decrement save index
        mov     bx, [csave]     ; BX is saved configuration index
        shl     bx, 1           ;  converted to byte count
        push    bx              ; Save for later

        mov     ah, 2           ; Select SET CURSOR POSITION function
        mov     dx, [bx + cursorpos]    ; Set cursor position
        mov     bx, [_video_page]       ; Set video page
        int     VIDEO           ; Call Video BIOS

        pop     bx              ; Restore BX
        mov     ah, 1           ; Select SET CURSOR SHAPE function
        mov     cx, [bx + cursorshape]  ; Set cursor shape
        int     VIDEO           ; Call Video BIOS
@@99:
        ret                         ; Return to caller
ENDP    _restorecursor
%NEWPAGE
;-----------------------------------------------------------------------
; void hidecursor(void)     Hide the cursor
;-----------------------------------------------------------------------
PROC    _hidecursor

        mov     ah, 3               ; Select GET CURSOR function
        mov     bx, [_video_page]   ; Select video page
        int     VIDEO               ; Call Video BIOS
        or      ch, 20h             ; Set cursor to invisible
        mov     ah, 1               ; Select SET CURSOR SHAPE function
        int     VIDEO               ; Call Video BIOS
        ret                         ; Return to caller
ENDP    _hidecursor
%NEWPAGE
;-----------------------------------------------------------------------
; void unhidecursor(void)       Unhide the cursor
;-----------------------------------------------------------------------
PROC    _unhidecursor

        mov     ah, 3               ; Select GET CURSOR function
        mov     bx, [_video_page]   ; Select video page
        int     VIDEO               ; Call Video BIOS
        and     ch, 0dfh            ; Set cursor to visible
        mov     ah, 1               ; Select SET CURSOR SHAPE function
        int     VIDEO               ; Call Video BIOS
        ret                         ; Return to caller
ENDP    _unhidecursor
%NEWPAGE
;-----------------------------------------------------------------------
; void set_cursor_type(unsigned t)  Use BIOS to set the cursor type
;-----------------------------------------------------------------------
PROC    _set_cursor_type

        ARG     t:Word

        push    bp                  ; Save old bp pointer
        mov     bp, sp              ; Access parameters

        mov     ah, 1               ; Select SET CURSOR SHAPE function
        mov     bx, [_video_page]   ; Select video page
        mov     cx, [t]             ; Set cursor type
        int     VIDEO               ; Call Video BIOS

        pop     bp                  ; Restore bp pointer
        ret                         ; Return to caller
ENDP    _set_cursor_type

ENDS    CONSOLE_TEXT            ; End of Code Segment

        END                     ; End of module
