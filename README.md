# Sean816 (Documentation under construction!)

> Sean816 is not a finished CPU architecture, its still in development. Goal of this project is research and education!

## Introduction

I made this project to learn more about CPUs and how they internally operate and to demonstrate how they work. Im happy to write this documentation for you guys. It was always my dream to write a working virtual CPU that is efficient. I already did it once in my previous 8bit CPU with 8bit addressing which limited many things. I dont feel comfortable yet adding full 16bit support to the Sean CPU series. Tho my 16bit CPU will likely be called Sean1616 as it will be 16bit CPU with 16bit addressing and to not be mistaken with my missnamed Sean16 CPU which was my first virtual CPU(SoC) but was basically only 8bit. I will explain how to craft such virtual CPU in this documentary, but also how to create a MMU(MemoryManagementUnit) and how to create a RAM as both of these things are necessary, aswell as how to map IO memory.

## Todo

#### CPU

- [ ] A FileSystem Device in ./Devices for more abilities
- [ ] MemoryManagementUnit (The MMU is responsible for example to create virtual address spaces for other processes)
- [ ] Ring based protection levels (This will be required for kernels and stuff to correctly do things, but first we need MMU)

##### Compiler

- [ ] A lot of fixes, hahaha

##### General

- [ ] Write a linker from scratch that builds a kind of header with text region and stuff, my idea is to make the assembler generate a kind of symbol table, the assembler shall output a object file with a header, the linker reads the headers of multiple object files and creates based on them a Sean816 full blown executable, this will be needed for preinitilized memory like strings and stuff. For example when you have C code that has `printf("Hello, World!\n");`, The string is not really in the call, its a pointer redirected from the data section the linker created in the final executable.
