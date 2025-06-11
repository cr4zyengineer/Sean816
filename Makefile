# Makefile - Sean816

all: Sean816AsmS Sean816EmuS

Sean816AsmS:
	make -C Sean816Asm

Sean816EmuS:
	make -C Device/Template
	make -C Device/Serial
	make -C Device/LazyDisplay
	clang *.c cpu/*.c -o Sean816
	./Sean816 -device Device/Template/TemplateDevice.so -device Device/Serial/Serial.so -device Device/LazyDisplay/LazyDisplay.so Sean816Asm/test.bin
