MAIN:
	CALL  *READLINE				; Calling Readline label
	CALL  *BUFFERB
	HALT

BUFFERB:
	LMEM  %C   0x0500			; Setting a register to MM for comparision
	LIMM  %A   0x00				; Setting a register to 0 to chase the first one
BUFFERBJMP:
	MOV   %MO  %A				; Setting memory offset
	LMEM  %B   0x0501			; Getting the character at buffer position
	LIMM  %MO  0x00				; Setting memory offset to 0 to write correctly
	STORE %B   0x00C0			; Store the B register to 0x00C1 which is the serial
	INC   %A					; Incrementing MM position
	CMP   %A   %C				; We compare the A register against C wich is set to 0
	JNE   *BUFFERBJMP
	LIMM  %A   0x00				; Storing 0x00 to register A to store 0 at memory position
	STORE %A   0x0500			; Writing 0x05 0x01 with register A to reset the position incremented by the BUFFERC label
	MOV   %MM  %A
	RET

BUFFERC:
	LMEM  %A   0x0500			; Get the current MM position
	MOV   %MO  %A				; Setting memory offset
	STORE %B   0x0501			; Storing the character at buffer position
	INC   %A					; Incrementing the MM position
	LIMM  %MO  0x00				; Setting memory offset to 0 to write correctly
	STORE %A   0x0500			; Storing MM position to the saver
	RET

READLINE:
	LIMM  %GA  0x0A				; Loading newline character for comparison
READLINEJMP:
	LMEM  %B   0x00C0			; Listening to keyboard and load it to global register B
	CMP   %GA  %B				; Comparing both global registers so if the user types in a new line character we stop listening and return
	STORE %B   0x00C0			; If they dont equal we print the character
	CALL  *BUFFERC
	JNE   *READLINEJMP			; If they dont equal we jump to READLINEJMP
	RET
