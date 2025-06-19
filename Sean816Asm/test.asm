main:
	call  *clearscreen
	mhml  *poc_str
	mov   a    mh
	mov   b    ml
	call  *printf
	mov   a    0x00
	mov   c    0x00				; Counter
mainpreintloop:
	cmp   c    0x0A
	jne   *mainpreintsp
	call  *helper_printnl
	mov   c    0x00
	jmp   *mainintloop
mainpreintsp:
	call  *helper_printsp
	inc   c
mainintloop:
	call  *printint
	inc   a
	cmp   a    0xFF
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

poc_str:
	str "This is a proof of concept code of Sean816!\n"

;
; printf
;
; NOTE: Prints a nullterminated string out based on the passed memory address
;
; a is the first 8bit of the 16bit address
; b is the second 8bit of the 16bit address
printf:
	load  c	   b   a			; Loading character at the memory address
	store c    0x00C0			; Writing the character to the serial device
	inc   b 					; Incrementing the high bit of the memory address
	cmp   c    0x00				; Comparing the character and check if its a null termination
	jne   *printf				; If its not do a reloop and if it is return to the caller
	ret

;
; fgets takes the memory address input
;
; NOTE: Gets userinput and stores it to passed memory address
;
; a is the first 8bit of the 16bit address
; b is the second 8bit of the 16bit address
fgets:
	load  c    0x00C0			; Loading keyboard input from serial device
	store c    0x00C0			; Print the keyboard input back to serial device
	store c    b   a			; Store the character to the memory address
	inc   b 					; Increment high bit
	cmp   c    0x0A				; Check if its a newline character
	jne   *fgets 				; If it is a newline character do not continue loop
	store 0x00 b   a
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
	mov  c 0x00
printintloopbig:
	cmp  a 0x64
	jl   *printintloop
	sub  a 0x64
	inc  c
	jmp  *printintloopbig
printintloop:
	cmp  a 0x0A
	jl   *printend
	sub  a 0x0A
	inc  b
	jmp *printintloop
printend:
	add   c 0x30
	add   b 0x30
	add   a 0x30
	store c 0x00C0
	store b 0x00C0
	store a 0x00C0
	ret

;
; clearscreen takes no arguments
;
; NOTE: Clears out the entire serial, its a bit messy tho, will resolve when i add data section with string pointers
;
clearscreen:
	mhml 0x00C0
	slh  0x1B
	slh  0x5B
	slh  0x33
	slh  0x4A
	slh  0x1B
	slh  0x5B
	slh  0x32
	slh  0x4A
	slh  0x1B
	slh  0x5B
	slh  0x48
	ret