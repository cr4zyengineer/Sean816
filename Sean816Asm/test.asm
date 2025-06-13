main:
	limm  a    0x02				; Specifying 0x0200 as memory address
	limm  b    0x00
	call  *fgets				; Getting userinput
	call  *printf				; Printing it back out
	halt

; printf
;
; NOTE: Prints a nullterminated string out based on the passed memory address
;
; a is the first 8bit of the 16bit address
; b is the second 8bit of the 16bit address
printf:
	limm  c    0x00				; NULL termination
	mov   ml   a				; Moving parameters to memory registers
	mov   mh   b
printfloop:
	llh   b						; Loading character at the memory address
	store b   0x00C0			; Writing the character to the serial device
	inc   mh					; Incrementing the high bit of the memory address
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
	limm  ga   0x0A				; Loading newline character for comparison
	mov   ml   a				; Moving parameters to memory registers
	mov   mh   b
fgetsloop:
	lmem  b    0x00C0			; Loading keyboard input from serial device
	store b    0x00C0			; Print the keyboard input back to serial device
	slh   b						; Store the character to the memory address
	inc   mh					; Increment high bit
	cmp   ga   b				; Check if its a newline character
	jne   *fgetsloop			; If it is a newline character do not continue loop
	ret


;
; fpassgets takes the memory address input
;
; NOTE: Gets password input and stores it to passed memory address
;
; a is the first 8bit of the 16bit address
; b is the second 8bit of the 16bit address
fpassgets:
	limm  ga   0x0A				; Loading newline character for comparison
	mov   ml   a				; Moving parameters to memory registers
	mov   mh   b
fpassgetsloop:
	lmem  b    0x00C0			; Loading keyboard input from serial device
	slh   b						; Store the character to the memory address
	inc   mh					; Increment high bit
	cmp   ga   b				; Check if its a newline character
	jne   *fpassgetsloop		; If it is a newline character do not continue loop
	ret
