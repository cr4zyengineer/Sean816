main:
	call  *readline				; Calling Readline label
	call  *printf
	halt

bufferb:
	lmem  c    0x0500			; Setting a register to MM for comparision
	limm  a    0x00				; Setting a register to 0 to chase the first one
bufferbloop:
	mov   mo   a 				; Setting memory offset
	lmem  b    0x0501			; Getting the character at buffer position
	limm  mo   0x00				; Setting memory offset to 0 to write correctly
	store b    0x00C0			; Store the B register to 0x00C1 which is the serial
	inc   a						; Incrementing MM position
	cmp   a   c					; We compare the A register against C wich is set to 0
	jne   *bufferbloop
	limm  a    0x00				; Storing 0x00 to register A to store 0 at memory position
	store a    0x0500			; Writing 0x05 0x01 with register A to reset the position incremented by the BUFFERC label
	mov   mo   a
	ret

printf:
	limm  c    0x00				; NULL termination
	limm  ml   0x05
	limm  mh   0x01
printfloop:
	llh   b						; Setting B to the memory addresses value containing a character that is offsetted by MO
	store b   0x00C0			; Writing the character to the serial
	inc   mh					; Incrementing A
	cmp   b   c					; We compare A and C
	jne   *printfloop			; If they dont equal we jump to the beginning of the loop
	ret

bufferc:
	lmem  mo   0x0500			; Get the current MM position
	store b   0x0501			; Storing the character at buffer position
	inc   mo					; Incrementing the MM position
	mov   a   mo				; Moving MM register to the A register
	limm  mo  0x00				; Setting memory offset to 0 to write correctly
	store a   0x0500			; Storing MM position to the saver
	ret

readline:
	limm  ga   0x0A				; Loading newline character for comparison
readlineloop:
	lmem  b    0x00C0			; Listening to keyboard and load it to global register B
	cmp   ga   b				; Comparing both global registers so if the user types in a new line character we stop listening and return
	store b   0x00C0			; If they dont equal we print the character
	call  *bufferc
	jne   *readlineloop			; If they dont equal we jump to READLINEJMP
	ret
