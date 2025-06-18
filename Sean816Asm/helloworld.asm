main:
	mhml *hellostring
	call  *printf
	mhml *hellostring1
	call  *printf
	mhml *hellostring2
	call  *printf
	mhml *meow
	call  *printf
	halt

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

printf:
	limm  c    0x00				; NULL termination
printfloop:
	llh   b						; Loading character at the memory address
	store b   0x00C0			; Writing the character to the serial device
	inc   ml					; Incrementing the high bit of the memory address
	cmp   b   c					; Comparing the character and check if its a null termination
	jne   *printfloop			; If its not do a reloop and if it is return to the caller
	ret

hellostring:
	str "Hello, World!"

meow:
	str "Meow!"

hellostring1:
	str "Hello, World!"

hellostring2:
	str "Hello, World!"