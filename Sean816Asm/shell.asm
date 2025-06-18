main:
	mhml *welcome
	call *printf
shellloop:
	mhml *prompt				; Prompt the user
	call *printf
	mhml *buffer
	call *fgets
	mov  a   mh 				; Compare init
	mov  b   ml
	mhml *exit_str				; Compare with exit command
	mov  c   mh
	mov  d   ml
	call *strcmp
	je   *shellexit
	mhml *help_str				; Compare with help command
	mov  c   mh
	mov  d   ml
	call *strcmp
	je   *shellhelp
	mhml *unknowncommand
	call *printf
	jmp  *shellwaypoint
shellexit:
	halt
	jmp  *shellwaypoint
shellhelp:
	mhml *helpcontent
	call *printf
	jmp  *shellwaypoint
shellwaypoint:
	jmp *shellloop

helper_printnl:
	limm  a 0x0A
	store a 0x00C0
	ret
helper_printsp:
	limm  a 0x20
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
	limm  c    0x00				; NULL termination
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
	limm  c    0x0A				; Loading newline character for comparison
fgetsloop:
	lmem  b    0x00C0			; Loading keyboard input from serial device
	store b    0x00C0			; Print the keyboard input back to serial device
	slh   b						; Store the character to the memory address
	inc   ml					; Increment high bit
	cmp   c    b				; Check if its a newline character
	jne   *fgetsloop			; If it is a newline character do not continue loop
fgetsend:
	dec   ml
	limm  b    0x00
	slh   b
	ret

strcmp:
	limm  e    0x00				; NULL termination
strcmploop:
	mov   mh   a
	mov   ml   b
	llh   ra
	mov   mh   c
	mov   ml   d
	llh   rb
	inc   b
	inc   d
	cmp   ra   e
	je    *strcmpend
	cmp   rb   e
	je    *strcmpend
	cmp   ra   rb
	je    *strcmploop
strcmpend:
	ret

;
; printint
;
; NOTE: Prints a registers value up to 0xFF
;
; a is the register
;
printint:
	limm b 0x00
	limm c 0x0A
	limm d 0x00
	limm e 0x64
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
	limm  c 0x30
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
	limm a  0x1B
	limm b  0x5B
	limm c  0x33
	limm d  0x4A
	limm f  0x48
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

;
; Data section
;
welcome:
	str "Welcome to Sean816Shell\n"

unknowncommand:
	str "No such command\n"

helpcontent:
	str "exit - exits shell\nhelp - prints all help options\n"

prompt:
	str "$ "

exit_str:
	str "exit"

help_str:
	str "help"

buffer:
	str ""