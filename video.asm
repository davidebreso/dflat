 %TITLE "Low level console routines for D-Fast"

        IDEAL

        MODEL   large

;-----------------------------------------------------------------------
; Equates
;-----------------------------------------------------------------------
CHANGECOLOR     EQU     174             ; Prefix to change colors
RESETCOLOR      EQU     175             ; Reset colors to default

;------ RECT structure

STRUC   RECT
 left   dw      ?       ; Left coordinate
 top    dw      ?       ; Top coordinate
 right  dw      ?       ; Right coordinate
 bott   dw      ?       ; Bottom coordinate
ENDS    RECT


DATASEG

        PUBLIC  _foreground, _background

        EXTRN   _video_address:Word

_foreground     dw      7
_background     dw      0


;-----------------------------------------------------------------------
; Define code segment, shared with console.asm and mouse.asm
;-----------------------------------------------------------------------
SEGMENT CONSOLE_TEXT Word Public 'CODE'

ASSUME  CS:CONSOLE_TEXT

        PUBLIC  _getvideo, _storevideo
        PUBLIC  _GetVideoChar, _PutVideoChar, _PutVideoStr

        EXTRN   _hide_mousecursor:Proc, _show_mousecursor:Proc


%NEWPAGE
;-----------------------------------------------------------------------
; video_offset      Get the video offset from screen position
;                   AX = x position, BX = y position
;                   Returns offset in AX = (y * 160) + 2 * x
;-----------------------------------------------------------------------
PROC    video_offset    NEAR

        push    dx          ; Save DX
        push    ax          ; Save x coordinate

        mov     ax, 160     ; Figure the
        mul     bx          ;  y offset
        pop     bx          ; Restore x coordinate in BX
        shl     bx, 1       ; Multiply by 2
        add     ax, bx      ; Add to y offset

        pop     dx          ; Restore DX
        ret                 ; Return to caller
ENDP    video_offset
%NEWPAGE
;-----------------------------------------------------------------------
; void getvideo(RECT rc, void far *bf)  Read a rectangle of video memory
;                                       into a save buffer
;-----------------------------------------------------------------------
PROC    _getvideo

        ARG     rc:RECT, bf:DWord

        push    bp                      ; Save old bp pointer
        mov     bp, sp                  ; Access parameters
        push    di                      ; Save di
        push    si                      ; Save si
        call    _hide_mousecursor       ; Hide mouse cursor

        push    ds                      ; Save data segment register

        mov     ax, [rc.left]           ; Figure
        mov     bx, [rc.top]            ;  the
        call    video_offset            ;   video offset
        mov     si, ax                  ; SI = video offset
        mov     ds, [_video_address]    ; DS = video segment
        mov     cx, [rc.bott]           ; Figure
        sub     cx, [rc.top]            ;  the number
        inc     cx                      ;   of rows
        mov     dx, [rc.right]          ; Figure
        sub     dx, [rc.left]           ;  the number
        inc     dx                      ;   of columns
        cld                             ; Flag increment
        les     di, [bf]                ; ES:DI = save buffer
@@10:
        push    cx                      ; Save the number of rows
        push    si                      ; Save the video offset
        mov     cx, dx                  ; CX = number of columns
        rep     movsw                   ; Save one row of screen
        pop     si                      ; Restore video offset
        add     si, 160                 ;  and point it to next row
        pop     cx                      ; Restore row counter
        loop    @@10                    ; Repeat for next row

        pop     ds                      ; Restore data segment register

        call    _show_mousecursor       ; Show mouse cursor
        pop     si                      ; Restore si
        pop     di                      ; Restore di
        pop     bp                      ; Restore bp pointer
        ret                             ; Return to caller
ENDP    _getvideo
%NEWPAGE
;-----------------------------------------------------------------------
; void storevideo(RECT rc, void far *bf)    Write a rectangle of video
;                                           memory from a save buffer
;-----------------------------------------------------------------------
PROC    _storevideo

        ARG     rc:RECT, bf:DWord

        push    bp                      ; Save old bp pointer
        mov     bp, sp                  ; Access parameters
        push    di                      ; Save di
        push    si                      ; Save si
        call    _hide_mousecursor       ; Hide mouse cursor

        push    ds                      ; Save data segment register

        mov     ax, [rc.left]           ; Figure
        mov     bx, [rc.top]            ;  the
        call    video_offset            ;   video offset
        mov     di, ax                  ; DI = video offset
        mov     es, [_video_address]    ; ES = video segment
        mov     cx, [rc.bott]           ; Figure
        sub     cx, [rc.top]            ;  the number
        inc     cx                      ;   of rows
        mov     dx, [rc.right]          ; Figure
        sub     dx, [rc.left]           ;  the number
        inc     dx                      ;   of columns
        cld                             ; Flag increment
        lds     si, [bf]                ; DS:SI = save buffer
@@10:
        push    cx                      ; Save the number of rows
        push    di                      ; Save the video offset
        mov     cx, dx                  ; CX = number of columns
        rep     movsw                   ; Save one row of screen
        pop     di                      ; Restore video offset
        add     di, 160                 ;  and point it to next row
        pop     cx                      ; Restore row counter
        loop    @@10                    ; Repeat for next row

        pop     ds                      ; Restore data segment register

        call    _show_mousecursor       ; Show mouse cursor
        pop     si                      ; Restore si
        pop     di                      ; Restore di
        pop     bp                      ; Restore bp pointer
        ret                             ; Return to caller
ENDP    _storevideo
%NEWPAGE
;-----------------------------------------------------------------------
; int GetVideoChar(int x, int y)        Read character and attribute
;                                       from video memory
;-----------------------------------------------------------------------
PROC    _GetVideoChar

        ARG     x:Word, y:Word

        push    bp                      ; Save old bp pointer
        mov     bp, sp                  ; Access parameters
        push    si                      ; Save SI register
        call    _hide_mousecursor       ; Hide mouse cursor
        push    ds                      ; Save data segment register

        mov     ax, [x]                 ; Figure
        mov     bx, [y]                 ;  the
        call    video_offset            ;   video offset
        mov     si, ax                  ; SI = video offset
        mov     ds, [_video_address]    ; DS = video segment
        lodsw                           ; Read character and attribute

        pop     ds                      ; Restore data segment register
        push    ax                      ; Save return value
        call    _show_mousecursor       ; Show mouse cursor
        pop     ax                      ; Restore return value
        pop     si                      ; Restore SI register
        pop     bp                      ; Restore bp pointer
        ret                             ; Return to caller
ENDP    _GetVideoChar
%NEWPAGE
;-----------------------------------------------------------------------
; void PutVideoChar(int x, int y, int c)    Write charater and attribute
;                                           to video memory
;-----------------------------------------------------------------------
PROC    _PutVideoChar

        ARG     x:Word, y:Word, c: Word

        push    bp                      ; Save old bp pointer
        mov     bp, sp                  ; Access parameters
        push    di                      ; Save DI register
        call    _hide_mousecursor       ; Hide mouse cursor

        mov     ax, [x]                 ; Figure
        mov     bx, [y]                 ;  the
        call    video_offset            ;   video offset
        mov     di, ax                  ; DI = video offset
        mov     es, [_video_address]    ; ES = video segment
        mov     ax, [c]                 ; AX = charater to write
        stosw                           ; Write character and attribute

        call    _show_mousecursor       ; Show mouse cursor
        pop     di                      ; Restore DI register
        pop     bp                      ; Restore bp pointer
        ret                             ; Return to caller
ENDP    _PutVideoChar
%NEWPAGE
;-----------------------------------------------------------------------
; void PutVideoStr(int x, int y, char *string, int len, int pad)
;         Write up to len characters of formatted string to video memory
;         Fill the line with spaces if pad is not zero
;-----------------------------------------------------------------------
PROC    _PutVideoStr

        ARG     x:Word, y:Word, string: Dword, len:Word, pad:Word

        push    bp                      ; Save old bp pointer
        mov     bp, sp                  ; Access parameters
        push    di                      ; Save DI register
        push    si                      ; Save SI register
        call    _hide_mousecursor       ; Hide mouse cursor
        push    ds                      ; Save data segment register

        mov     ax, [x]                 ; Figure
        mov     bx, [y]                 ;  the
        call    video_offset            ;   video offset
        mov     di, ax                  ; DI = video offset
        mov     es, [_video_address]    ; ES = video segment
        mov     ah, [byte ptr _background]   ; Load background color
        mov     cl, 4                   ;  to
        shl     ah, cl                  ;   high nibble of attribute
        or      ah, [byte ptr _foreground]  ; add foreground color
        mov     bl, ah                  ; Save default attribute for later
        mov     cx, [len]               ; CX = character count
        jcxz    @@99                    ; Return if count = 0
        lds     si, [string]            ; DS:SI = string pointer

@@10:
        lodsb                           ; Load next char of string in AL
        or      al, al                  ; If char is NULL
        jz      @@40                    ;  then jump to padding
        cmp     al, CHANGECOLOR         ; Is change color prefix?
        je      @@20                    ;  then jump
        cmp     al, RESETCOLOR          ; Is reset color?
        je      @@30                    ;  then jump
        stosw                           ; Else write char and attribute
        loop    @@10                    ; Loop until done
        jmp     @@99                    ; Done, jump to return
@@20:
        xchg    cx, dx                  ; Save CX in DX
        lodsw                           ; Load color attributes in AX
        mov     cl, 4                   ; Shift background
        shl     ah, cl                  ;  to high nibble
        or      ah, al                  ;   and add foreground color
        xchg    cx, dx                  ; Restore CX
        jmp     @@10                    ; Continue with next char
@@30:
        mov     ah, bl                  ; Reset color attribute
        jmp     @@10                    ;  and continue

@@40:
        cmp     [pad], 0                ; Do we have to pad?
        jz      @@99                    ;  jump if not
        mov     al, ' '                 ; Use spaces to pad
        rep     stosw                   ; Pad the line
@@99:
        pop     ds                      ; Restore data segment register
        call    _show_mousecursor       ; Show mouse cursor
        pop     si                      ; Restore SI register
        pop     di                      ; Restore DI register
        pop     bp                      ; Restore bp pointer
        ret                             ; Return to caller
ENDP    _PutVideoStr

ENDS    CONSOLE_TEXT            ; End of Code Segment

        END                     ; End of module
