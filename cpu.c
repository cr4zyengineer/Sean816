#include "cpu.h"
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

extern uint8_t mem[MEMORY_SIZE];

/*
 * Instructions
 */
void cpu_load(cpu_core_t *core);
void cpu_store(cpu_core_t *core);
void cpu_mhml(cpu_core_t *core);
void cpu_loadlh(cpu_core_t *core);
void cpu_storelh(cpu_core_t *core);
void cpu_mov(cpu_core_t *core);

void cpu_add(cpu_core_t *core);
void cpu_sub(cpu_core_t *core);
void cpu_mul(cpu_core_t *core);
void cpu_div(cpu_core_t *core);
void cpu_inc(cpu_core_t *core);
void cpu_dec(cpu_core_t *core);

void cpu_jmp(cpu_core_t *core);
void cpu_cmp(cpu_core_t *core);
void cpu_je(cpu_core_t *core);
void cpu_jne(cpu_core_t *core);
void cpu_jg(cpu_core_t *core);
void cpu_jl(cpu_core_t *core);

void cpu_push(cpu_core_t *core);
void cpu_pop(cpu_core_t *core);
void cpu_call(cpu_core_t *core);
void cpu_ret(cpu_core_t *core);

void cpu_and(cpu_core_t *core);
void cpu_or(cpu_core_t *core);
void cpu_xor(cpu_core_t *core);
void cpu_not(cpu_core_t *core);

/*
 * Opcode table
 */
instruction_t opcode_table[UINT8_MAX] = {
    NULL,

    cpu_load,
    cpu_store,
    cpu_mhml,
    cpu_loadlh,
    cpu_storelh,
    cpu_mov,

    cpu_add,
    cpu_sub,
    cpu_mul,
    cpu_div,
    cpu_inc,
    cpu_dec,

    cpu_jmp,
    cpu_cmp,
    cpu_je,
    cpu_jne,
    cpu_jg,
    cpu_jl,

    cpu_push,
    cpu_pop,
    cpu_call,
    cpu_ret,

    cpu_and,
    cpu_or,
    cpu_xor,
    cpu_not
};

/*
 * Creates a CPU Core
 */
cpu_core_t* cpu_create_core(void)
{
    cpu_core_t *core = NULL;
    core = malloc(sizeof(cpu_core_t));

    core->pc = MEMORY_MAPPED_IO_REGION_SIZE;    // Setting up program counter and stack pointer
    core->sp = MEMORY_SIZE;
    core->ml = 0;
    core->mh = 0;
    core->cmp = 0;

    for(int i = 0x00; i < 0xFF; i++)
        core->regtable[i] = &core->a;

    core->regtable[0x00] = &core->a;     // Mapping registers so they are easier to get, like the opcode table thingy
    core->regtable[0x01] = &core->b;
    core->regtable[0x02] = &core->c;
    core->regtable[0x03] = &core->d;
    core->regtable[0x04] = &core->e;
    core->regtable[0x05] = &core->f;
    core->regtable[0x06] = &core->g;
    core->regtable[0x07] = &core->h;
    core->regtable[0x08] = &core->ra;
    core->regtable[0x09] = &core->rb;
    core->regtable[0x0A] = &core->rc;
    core->regtable[0x0B] = &core->rd;
    core->regtable[0x0C] = &core->re;
    core->regtable[0x0D] = &core->rf;
    core->regtable[0x0E] = &core->rg;
    core->regtable[0x0F] = &core->rh;
    core->regtable[0x10] = &core->ga;
    core->regtable[0x11] = &core->gb;
    core->regtable[0x12] = &core->gc;
    core->regtable[0x13] = &core->gd;
    core->regtable[0x14] = &core->ge;
    core->regtable[0x15] = &core->gf;
    core->regtable[0x16] = &core->gg;
    core->regtable[0x17] = &core->gh;
    core->regtable[0x18] = &core->ml;
    core->regtable[0x19] = &core->mh;
    core->regtable[0x1A] = &core->cmp;
    core->regtable[0x1B] = ((uint8_t*)&core->sp) + 1;
    core->regtable[0x1C] = ((uint8_t*)&core->sp);

    for(uint8_t i = 0; i < 0x1A; i++)
        *core->regtable[i] = 0x00;

    return core;
}

/*
 * Executes a cpu core;
 */
void cpu_exec_core(cpu_core_t *core)
{
    while(1)
    {
        // Decode instruction
        core->instruction = mem[core->pc++];
        core->operandsig[0] = (core->instruction >> 5) & 1;
        core->operandsig[1] = (core->instruction >> 6) & 1;
        core->operandsig[2] = (core->instruction >> 7) & 1;
        core->instruction = core->instruction & 0b11111;

        // Execute instruction
        if((core->instruction != OP_HLT || core->instruction < 0x1A) && opcode_table[core->instruction])
            opcode_table[core->instruction](core);
        else
            return;
    }
}

/*
 * Helper function to get as many args as needed by a instruction
 */
void cpu_core_get_args(cpu_core_t *core, uint8_t count)
{
    for(uint8_t i = 0; i < count; i++)
        if(core->operandsig[i] == true)
            core->targ[i] = core->regtable[mem[core->pc++]];
        else
            core->targ[i] = &mem[core->pc++];
}