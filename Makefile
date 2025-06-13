# Makefile - Sean816

all: Sean816AsmS Sean816EmuS Sean816EmuRunS
run: Sean816AsmS Sean816EmuRunS

Sean816AsmS:
	make -C Sean816Asm

Sean816EmuS:
	make -C Device/Serial
	clang *.c CPU/*.c ROM/*.c MMU/*.c -o Sean816

Sean816EmuRunS:
	./Sean816 -device Device/Serial/Serial.so Sean816Asm/test.bin
