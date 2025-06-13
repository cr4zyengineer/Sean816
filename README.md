# Sean816 (Documentation under construction!)

> Sean816 is not a finished CPU architecture, its still in development. Goal of this project is research and education!

## Introduction

I made this project to learn more about CPUs and how they internally operate and to demonstrate how they work. Im happy to write this documentation for you guys. It was always my dream to write a working virtual CPU that is efficient. I already did it once in my previous 8bit CPU with 8bit addressing which limited many things. I dont feel comfortable yet adding full 16bit support to the Sean CPU series. Tho my 16bit CPU will likely be called Sean1616 as it will be 16bit CPU with 16bit addressing and to not be mistaken with my missnamed Sean16 CPU which was my first virtual CPU(SoC) but was basically only 8bit. I will explain how to craft such virtual CPU in this documentary, but also how to create a MMU(MemoryManagementUnit) and how to create a RAM as both of these things are necessary, aswell as how to map IO memory.

## Register

The CPU features in total 30 register of which 27 are directly read and writable.

<table>
    <tr>
        <td>Register Class</td>
        <td>Registers</td>
        <td>Details</td>
    </tr>
    <tr>
        <td>Special</td>
        <td><code>PC</code>,<code>BP</code>,<code>SP</code></td>
        <td>Registers to keep track of what the CPU is executing and jumping to.</td>
    </tr>
    <tr>
        <td>Memory</td>
        <td><code>MO (Deprecated)</code>,<code>ML</code>,<code>MH</code></td>
        <td>Registers to be able to write to arbitary memory addresses.</td>
    </tr>
    <tr>
        <td>Return</td>
        <td><code>RA</code>,<code>RB</code>,<code>RC</code>,<code>RD</code>,<code>RE</code>,<code>RF</code>,<code>RG</code>,<code>RH</code></td>
        <td>Registers to get the returning functions registers.</td>
    </tr>
    <tr>
        <td>Global</td>
        <td><code>GA</code>,<code>GB</code>,<code>GC</code>,<code>GD</code>,<code>GE</code>,<code>GF</code>,<code>GG</code>,<code>GH</code></td>
        <td>Registers that are globally fixed in the core and wont change during the entire exeuction context, unless wanted.</td>
    </tr>
    <tr>
        <td>General</td>
        <td><code>A(Accumulator)</code>,<code>B</code>,<code>C</code>,<code>D</code>,<code>E</code>,<code>F</code>,<code>G</code>,<code>H</code></td>
        <td>Registers that are for any purpose the person wishes too. The general registers..</td>
    </tr>
</table>

## Instruction Set

#### Execution

<table>
    <tr>
        <td>Instruction</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Details</td>
    </tr>
    <tr>
        <td>HLT</td>
        <td colspan="4"></td>
        <td>Stops code execution.</td>
    </tr>
</table>

#### Data

<table>
    <tr>
        <td>Instruction</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Details</td>
    </tr>
    <tr>
        <td>LOAD</td>
        <td>Mode</td>
        <td>Register</td>
        <td>Imm8</td>
        <td>Imm8</td>
        <td>Loads a intermediate or memory from a memory address to register. You only need the second register when you load from memory. 0x00 is the intermediate mode and 0x01 is the memory mode.</td>
    </tr>
    <tr>
        <td>STORE</td>
        <td>Register</td>
        <td colspan="2">Address</td>
        <td></td>
        <td>Stores a register to a memory address. The address is contained of two intermediate 8bit values.</td>
    </tr>
    <tr>
        <td>LOADLH</td>
        <td>Register</td>
        <td colspan="3"></td>
        <td>Loads memory from a memory address(resulting from register ML and MH) to a register.</td>
    </tr>
    <tr>
        <td>STORELH</td>
        <td>Register</td>
        <td colspan="3"></td>
        <td>Stores a register to a memory address(resulting from register ML and MH).</td>
    </tr>
    <tr>
        <td>MOV</td>
        <td>Register</td>
        <td>Register</td>
        <td colspan="2"></td>
        <td>Moves the value from the second passed register to the first(Note that the value is not removed from the 2nd passed register or swapped).</td>
    </tr>
</table>

#### Arithmetic

<table>
    <tr>
        <td>Instruction</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Details</td>
    </tr>
    <tr>
        <td>ADD</td>
        <td>Register</td>
        <td>Register</td>
        <td colspan="2"></td>
        <td>Additions the 1st and the 2nd passed register and stores the result in register A which is the Acumulator.</td>
    </tr>
    <tr>
        <td>SUB</td>
        <td>Register</td>
        <td>Register</td>
        <td colspan="2"></td>
        <td>Subtracts the 1st and the 2nd passed register and stores the result in register A which is the Acumulator.</td>
    </tr>
    <tr>
        <td>MUL</td>
        <td>Register</td>
        <td>Register</td>
        <td colspan="2"></td>
        <td>Multiplicates the 1st and the 2nd passed register and stores the result in register A which is the Acumulator.</td>
    </tr>
    <tr>
        <td>DIV</td>
        <td>Register</td>
        <td>Register</td>
        <td colspan="2"></td>
        <td>Divides the 1st and the 2nd passed register and stores the result in register A which is the Acumulator.</td>
    </tr>
    <tr>
        <td>INC</td>
        <td>Register</td>
        <td colspan="3"></td>
        <td>Increments the passed register.</td>
    </tr>
    <tr>
        <td>DEC</td>
        <td>Register</td>
        <td colspan="3"></td>
        <td>Decrements the passed register.</td>
    </tr>
</table>

#### Flow Control

<table>
    <tr>
        <td>Instruction</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Details</td>
    </tr>
    <tr>
        <td>JMP</td>
        <td colspan="2">Address</td>
        <td colspan="2"></td>
        <td>Jumps to the passed memory address.</td>
    </tr>
    <tr>
        <td>CMP</td>
        <td>Register</td>
        <td>Register</td>
        <td colspan="2"></td>
        <td>Compares the 1st and the 2nd register passed and sets the cmp flag in the cpu core executing the instruction.</td>
    </tr>
    <tr>
        <td>JE</td>
        <td colspan="2">Address</td>
        <td colspan="2"></td>
        <td>Jumps to the passed memory address in case the cmp flag indicates that the registers compared were equal.</td>
    </tr>
    <tr>
        <td>JNE</td>
        <td colspan="2">Address</td>
        <td colspan="2"></td>
        <td>Jumps to the passed memory address in case the cmp flag indicates that the registers compared were not equal.</td>
    </tr>
    <tr>
        <td>JG</td>
        <td colspan="2">Address</td>
        <td colspan="2"></td>
        <td>Jumps to the passed memory address in case the cmp flag indicates that the 1st register passed was bigger than the 2nd.</td>
    </tr>
    <tr>
        <td>JL</td>
        <td colspan="2">Address</td>
        <td colspan="2"></td>
        <td>Jumps to the passed memory address in case the cmp flag indicates that the 1st register passed was smaller than the 2nd.</td>
    </tr>
</table>

#### Stack

<table>
    <tr>
        <td>Instruction</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Arg</td>
        <td>Details</td>
    </tr>
    <tr>
        <td>PUSH</td>
        <td>Register</td>
        <td colspan="3"></td>
        <td>Pushes value of register passed to onto the stack.</td>
    </tr>
    <tr>
        <td>POP</td>
        <td>Register</td>
        <td colspan="3"></td>
        <td>Pops value of of the current stack to the register passed.</td>
    </tr>
    <tr>
        <td>CALL</td>
        <td colspan="2">Address</td>
        <td colspan="2"></td>
        <td>Jumps onto a passed address but with the differency that its a call, so its more isolated and change friendly to the CPU and you can return to the caller without massive constructions.</td>
    </tr>
    <tr>
        <td>RET</td>
        <td colspan="4"></td>
        <td>Returns back to caller.</td>
    </tr>
</table>

## Todo

#### CPU

- [x] Execution flow instructions
- [x] Data instructions (ie. loading, storing and moving data)
- [x] Arithmetic instructions (basically math)
- [x] Stack memory
- [x] Control flow instructions (Jumping to a memory address, calling a address, returning back to the caller)
- [x] Device loader (loads usermade devices specified as a command arg)
- [x] ROM (the first code the cpu always executes to i.e run a kernel image)
- [ ] Interruption handler (for the corresponding `INT` instruction, which will be necessary for the kernel to communicate with the BIOS(ROM) and vise versa, and for the userspace process to communicate with the kernel)
- [ ] Threading (It will be a 1 core 4 thread CPU)
- [ ] Real and protected mode (protected mode will be a 1 time register, once set there is no way to set it back and the currently running thread will always be in protected mode with restrictions set by the kernel image)
- [ ] MMU (Manages memory, adds virtual address spaces to cores so that userspace processes cannot access the memory)
- [ ] MMU instructions for the kernel to manage the page table added by the MMU

##### General

- [ ] Write a linker from scratch that builds a kind of header with text region and stuff, my idea is to make the assembler generate a kind of symbol table, the assembler shall output a object file with a header, the linker reads the headers of multiple object files and creates based on them a Sean816 full blown executable, this will be needed for preinitilized memory like strings and stuff. For example when you have C code that has `printf("Hello, World!\n");`, The string is not really in the call, its a pointer redirected from the data section the linker created in the final executable.
