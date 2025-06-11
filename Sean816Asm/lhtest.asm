MAIN:
    LIMM %ML 0x00  ; Loading the offsets of the serial to the memory 16bit address helper registers
    LIMM %MH 0xC0
    LIMM %B  0x0A  ; To compare
MAINJMP:
    LLH  %A        ; Reading the userinput from the address that is the keyboard input
    SLH  %A        ; Writing the userinput back to the serial
    CMP  %A  %B
    JNE  *MAINJMP
    HALT
