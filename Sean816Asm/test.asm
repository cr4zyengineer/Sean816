main:
	mov   a    0x00
	mov   b    0xFF

	mov   c    0x00				; Counter
	mov   d    0x0A				; Counter limit
mainpreintloop:
	cmp   c    d
	jne   *mainpreintsp
	call  *helper_printnl
	mov   c 0x00
	jmp   *mainintloop
mainpreintsp:
	call  *helper_printsp
	inc   c
mainintloop:
	call  *printint
	inc   a
	cmp   a    b
	jne   *mainpreintloop
mainreadline:
	call  *helper_printnl
	mov   a    0x02				; Specifying 0x0200 as memory address
	mov   b    0x00
	call  *fgets				; Getting userinput
	call  *printf				; Printing it back out
	halt

helper_printnl:
	mov   a 0x0A
	store a 0x00C0
	ret
helper_printsp:
	mov   a 0x20
	store a 0x00C0
	ret

;
; printf
;
; NOTE: Prints a nullterminated string out based on the passed memory address
;
; a is the first 8bit of the 16bit address
; b is the second 8bit of the 16bit address
printf:
	mov   c    0x00				; NULL termination
	mov   ml   b				; Moving parameters to memory registers
	mov   mh   a
printfloop:
	llh   b						; Loading character at the memory address
	store b   0x00C0			; Writing the character to the serial device
	inc   ml					; Incrementing the high bit of the memory address
	cmp   b   c					; Comparing the character and check if its a null termination
	jne   *printfloop			; If its not do a reloop and if it is return to the caller
	ret

;
; fgets takes the memory address input
;
; NOTE: Gets userinput and stores it to passed memory address
;
; a is the first 8bit of the 16bit address
; b is the second 8bit of the 16bit address
fgets:
	mov   c    0x0A				; Loading newline character for comparison
	mov   ml   b				; Moving parameters to memory registers
	mov   mh   a
fgetsloop:
	load  b    0x00C0			; Loading keyboard input from serial device
	store b    0x00C0			; Print the keyboard input back to serial device
	slh   b						; Store the character to the memory address
	inc   ml					; Increment high bit
	cmp   c    b				; Check if its a newline character
	jne   *fgetsloop			; If it is a newline character do not continue loop
fgetsend:
	mov   b    0x00
	slh   b
	ret

;
; printint
;
; NOTE: Prints a registers value up to 0xFF
;
; a is the register
;
printint:
	mov  b 0x00
	mov  c 0x0A
	mov  d 0x00
	mov  e 0x64
printintloopbig:
	cmp  a e
	jl   *printintloop
	sub  a e
	inc  d
	jmp  *printintloopbig
printintloop:
	cmp  a c
	jl   *printend
	sub  a c
	inc  b
	jmp *printintloop
printend:
	mhml  0x00C0
	mov   c 0x30
	add   d c
	slh   d
	add   b c
	slh   b
	add   a c
	slh   a
	ret

;
; clearscreen takes no arguments
;
; NOTE: Clears out the entire serial, its a bit messy tho, will resolve when i add data section with string pointers
;
clearscreen:
	mhml 0x00C0
	mov  a  0x1B
	mov  b  0x5B
	mov  c  0x33
	mov  d  0x4A
	mov  f  0x48
	slh  a
	slh  b
	slh  c
	slh  d
	dec  c
	slh  a
	slh  b
	slh  c
	slh  d
	slh  a
	slh  b
	slh  f
	ret
