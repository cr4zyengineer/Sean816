# Makefile - Sean816

all: Sean816AsmS Sean816EmuS

Sean816AsmS:
	make -C Sean816Asm

Sean816EmuS:
	make -C Device/Template
	make -C Device/Serial
	clang *.c cpu/*.c -o sean816
	./sean816
