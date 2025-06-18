main:
	mhml *hellostring
	call  *printf
	halt

hellostring:
	str "Hello, World!\n\n"

printf:
	limm  c    0x00				; NULL termination
printfloop:
	llh   b						; Loading character at the memory address
	store b   0x00C0			; Writing the character to the serial device
	inc   ml					; Incrementing the high bit of the memory address
	cmp   b   c					; Comparing the character and check if its a null termination
	jne   *printfloop			; If its not do a reloop and if it is return to the caller
	ret