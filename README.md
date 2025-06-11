# How to make a CPU from scratch and emulate it?

I made this paper because I feel that there is movement missing in this topic and this should definitely be better researched in the future, as emulation techniques already evolve its more on a enterprise level than a personal-use level. I wanna document how to craft a brand new CPU architecture from scratch. In my documentary I will craft a single threaded CPU. The next generation of `Sean16` which was the first CPU i ever created on my own, tho this was more of a experiment and more of a messy proof of concept. It will be called `Sean816` which will be a 8 bit CPU with 16 bit addressing.


## What is a CPU?

A CPU also known as central processing unit is what executes the code running on your computer, every app that you run on your computer is executed by the CPU. You can read this for example just because your CPU is doing its job and processed this web page. Basically this program, for example the browser you reading this from is loaded into memory, into your random access memory. Your CPU has many many register, one of them is the `PC` register which is called the "program counter", this keeps track of the current offset in memory that is executed by the CPU currently. Then there is the `SP` register which is called the "stack pointer", this register keeps track of the address of stack memory. These registers are called special purpose registers. Then we got general purpose register, they are used to store values and operate with them, like mathematical operations and simulare. Now what does a CPU execute. A CPU operates using instructions. Basically codes that tell the CPU what to do. To make it more uncomplicated I will show you how to do it in a easy byte code level.. here a example for registers.

## What does the bit amount of the CPU do, why `Sean816`?

the bit amount is important, it basically means with how many bits the CPU can operate in a single operation like ``