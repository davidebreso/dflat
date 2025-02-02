%TITLE "Low level mouse routines for D-Fast"

        IDEAL

        MODEL   large

;-----------------------------------------------------------------------
; Equates
;-----------------------------------------------------------------------
MOUSE           EQU     033h            ; Mouse interrupt
MOUSE_ISR       EQU     4 * MOUSE       ; Offset of mouse ISR

DATASEG

        PUBLIC  _mouse_installed

_mouse_installed        dw      0       ; Set to 1 if mouse is available

;-----------------------------------------------------------------------
; Define code segment, shared with console.asm
;-----------------------------------------------------------------------
SEGMENT CONSOLE_TEXT Word Public 'CODE'

ASSUME  CS:CONSOLE_TEXT

        PUBLIC  _resetmouse
        PUBLIC  _mousebuttons, _button_releases
        PUBLIC  _get_mouseposition, _set_mouseposition
        PUBLIC  _show_mousecursor, _hide_mousecursor

%NEWPAGE
;-----------------------------------------------------------------------
; void resetmouse(void)         Reset the mouse
;-----------------------------------------------------------------------
PROC    _resetmouse

        xor     ax, ax              ; Point ES
        mov     es, ax              ;  to vector table
        les     bx, [es:mouse_isr]  ; Load ISR address in ES:BX
        mov     ax, es              ; If ISR address
        or      ax, bx              ;  is 0000:0000h
        jz      @@99                ;   then disable mouse and return
        xor     ax, ax              ; Prepare to disable mouse
        mov     dl, [es:bx]         ; Load first opcode of ISR in DL
        cmp     dl, 0cfh            ; Is it IRET?
        je      @@99                ;   then disable mouse and return
        int     MOUSE               ; else reset mouse
        and     ax, ax              ;  and jump if
        jz      @@99                ;   mouse is not available
        mov     ax, 1               ; Set mouse available
@@99:
        mov     [_mouse_installed], ax  ; Save local mouse flag
        ret                         ; Return to caller
ENDP    _resetmouse
%NEWPAGE
;-----------------------------------------------------------------------
; int mousebuttons(void)    Return TRUE if mouse buttons are pressed
;-----------------------------------------------------------------------
PROC    _mousebuttons

        xor     ax, ax              ; Prepare to return FALSE
        cmp     [_mouse_installed], 0   ; Check if mouse exists
        je      @@99                ;  and return false if not

        mov     ax, 3               ; RETURN STATUS function
        int     MOUSE               ; Call mouse interrupt
        mov     ax, bx              ; Prepare to return
        and     ax, 3               ;  left and right button status
@@99:
        ret                         ; Return to caller
ENDP    _mousebuttons
%NEWPAGE
;-----------------------------------------------------------------------
; int button_releases(void)     Return TRUE if a mouse button
;                               has been released
;-----------------------------------------------------------------------
PROC    _button_releases

        xor     ax, ax                  ; Prepare to return FALSE
        cmp     [_mouse_installed], 0   ; If mouse does not exist
        je      @@99                    ;  then return

        mov     ax, 6           ; select RETURN BUTTON RELEASE function
        xor     bx, bx          ;  select left button
        int     MOUSE           ;   and call mouse interrupt
        mov     ax, bx          ; Set return value
@@99:
        ret                     ; Return to caller
ENDP    _button_releases
%NEWPAGE
;-----------------------------------------------------------------------
; void get_mouseposition(int *x, int *y)    Return mouse coordinates
;-----------------------------------------------------------------------
PROC    _get_mouseposition

        ARG     x:Dword, y:Dword

        push    bp                      ; Save old bp pointer
        mov     bp, sp                  ; Access parameters
        cmp     [_mouse_installed], 0   ; If mouse does not exist
        je      @@99                    ;  then return

        mov     ax, 3                   ; Select RETURN STATUS function
        int     MOUSE                   ;  and call mouse interrupt
        shr     cx, 1                   ; Divide by 8
        shr     cx, 1                   ;  to figure
        shr     cx, 1                   ;   the column position
        les     bx, [x]                 ; Load x address in ES:BX
        mov     [es:bx], cx             ;  and save column position
        shr     dx, 1                   ; Divide by 8
        shr     dx, 1                   ;  to figure
        shr     dx, 1                   ;   the row position
        les     bx, [y]                 ; Load y address in ES:BX
        mov     [es:bx], dx             ;  and save row position
@@99:
        pop     bp                      ; Restore saved bp
        ret                             ; Return to caller
ENDP    _get_mouseposition
%NEWPAGE
;-----------------------------------------------------------------------
; void set_mouseposition(int x, int y)      Position the mouse cursor
;-----------------------------------------------------------------------
PROC    _set_mouseposition

        ARG     x:Word, y:Word

        push    bp                      ; Save old bp pointer
        mov     bp, sp                  ; Access parameters
        cmp     [_mouse_installed], 0   ; If mouse does not exist
        je      @@99                    ;  then return

        mov     ax, 4                   ; Select POSITION CURSOR function
        mov     cx, [x]                 ; Set
        shl     cx, 1                   ;  the
        shl     cx, 1                   ;   column
        shl     cx, 1                   ;    position
        mov     dx, [y]                 ; Set
        shl     dx, 1                   ;  the
        shl     dx, 1                   ;   row
        shl     dx, 1                   ;    position
        int     MOUSE                   ; Call mouse interrupt
@@99:
        pop     bp                      ; Restore saved bp
        ret                             ; Return to caller
ENDP    _set_mouseposition
%NEWPAGE
;-----------------------------------------------------------------------
; void show_mousecursor(void)       Display the mouse cursor
;-----------------------------------------------------------------------
PROC    _show_mousecursor

        cmp     [_mouse_installed], 0   ; If mouse does not exist
        je      @@99                    ;  then return

        mov     ax, 1               ; select SHOW MOUSE CURSOR function
        int     MOUSE               ;  and call mouse interrupt
@@99:
        ret                         ; Return to caller
ENDP    _show_mousecursor
%NEWPAGE
;-----------------------------------------------------------------------
; void hide_mousecursor(void)       Hide the mouse cursor
;-----------------------------------------------------------------------
PROC    _hide_mousecursor

        cmp     [_mouse_installed], 0   ; If mouse does not exist
        je      @@99                    ;  then return

        mov     ax, 2               ; select HIDE MOUSE CURSOR function
        int     MOUSE               ;  and call mouse interrupt
@@99:
        ret                         ; Return to caller
ENDP    _hide_mousecursor

ENDS    CONSOLE_TEXT            ; End of Code Segment

        END                     ; End of module
