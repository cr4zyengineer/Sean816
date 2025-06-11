MAIN:
    LIMM %A  0x07   ; Y coord
    LIMM %B  0x05   ; X coord
    LIMM %C  0x01   ; State
    CALL *DRAWPX    ; Drawing the mid point
NOESCAPE:
    JMP *NOESCAPE

DRAWPX:             ; Draws a pixel. %A is Y coord, %B is X coord and %C is the on state
    STORE %B 0x00E0
    STORE %A 0x00E1
    STORE %C 0x00E2
    RET
