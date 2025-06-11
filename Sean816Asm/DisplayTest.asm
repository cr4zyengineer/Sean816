MAIN:
    LIMM %GA 0xFF
    LIMM %A  0x00
    LIMM %B  0x00
    LIMM %C  0x01
    JMP *MAINJMP
INCJMP:
    CALL *DRAWPX
    INC  %B
    LIMM %A 0x00
MAINJMP:                    ; Ill try filling the entire screen
    CALL *DRAWPX
    INC  %A
    CMP  %A %GA
    JE   *INCJMP
    CMP  %B %GA
    JNE  *MAINJMP
NOESCAPE:
    JMP *NOESCAPE

DRAWPX:
    LIMM %ML 0x00
    LIMM %MH 0xE0
    SLH  %A
    INC  %MH
    SLH  %B
    INC  %MH
    SLH  %C
    RET
