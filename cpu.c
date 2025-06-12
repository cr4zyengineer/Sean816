#include "cpu.h"
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/*
 * Instructions
 */
void cpu_hlt(cpu_core_t *core);

void cpu_load(cpu_core_t *core);
void cpu_store(cpu_core_t *core);
void cpu_loadlh(cpu_core_t *core);
void cpu_storelh(cpu_core_t *core);
void cpu_mov(cpu_core_t *core);

void cpu_add(cpu_core_t *core);
void cpu_sub(cpu_core_t *core);
void cpu_mul(cpu_core_t *core);
void cpu_div(cpu_core_t *core);
void cpu_inc(cpu_core_t *core);
void cpu_dec(cpu_core_t *core);
void cpu_sadd(cpu_core_t *core);
void cpu_ssub(cpu_core_t *core);
void cpu_smul(cpu_core_t *core);
void cpu_sdiv(cpu_core_t *core);

void cpu_jmp(cpu_core_t *core);
void cpu_cmp(cpu_core_t *core);
void cpu_scmp(cpu_core_t *core);
void cpu_je(cpu_core_t *core);
void cpu_jne(cpu_core_t *core);
void cpu_jg(cpu_core_t *core);
void cpu_jl(cpu_core_t *core);

void cpu_push(cpu_core_t *core);
void cpu_pop(cpu_core_t *core);
void cpu_call(cpu_core_t *core);
void cpu_ret(cpu_core_t *core);

/*
 * Opcode table
 */
instruction_t opcode_table[UINT8_MAX] = {
    cpu_hlt,

    cpu_load,
    cpu_store,
    cpu_loadlh,
    cpu_storelh,
    cpu_mov,

    cpu_add,
    cpu_sub,
    cpu_mul,
    cpu_div,
    cpu_inc,
    cpu_dec,
    cpu_sadd,
    cpu_ssub,
    cpu_smul,
    cpu_sdiv,

    cpu_jmp,
    cpu_cmp,
    cpu_scmp,
    cpu_je,
    cpu_jne,
    cpu_jg,
    cpu_jl,

    cpu_push,
    cpu_pop,
    cpu_call,
    cpu_ret,
};

/*
 * Creates a CPU Core
 */
cpu_core_t* cpu_create_core(void)
{
    cpu_core_t *core = NULL;
    core = malloc(sizeof(cpu_core_t));

    core->pc = MEMORY_MAPPED_IO_REGION_SIZE;    // Setting up program counter and stack pointer
    core->sp = UINT16_MAX;
    core->mo = 0;
    core->ml = 0;
    core->mh = 0;
    core->cmp = 0;

    //for(int i = 0x00; i < 257; i++)
    //    core->reg[i] = &core->a;

    core->reg[0x00] = &core->a;     // Mapping registers so they are easier to get, like the opcode table thingy
    core->reg[0x01] = &core->b;
    core->reg[0x02] = &core->c;
    core->reg[0x03] = &core->d;
    core->reg[0x04] = &core->e;
    core->reg[0x05] = &core->f;
    core->reg[0x06] = &core->g;
    core->reg[0x07] = &core->h;
    core->reg[0x08] = &core->ra;
    core->reg[0x09] = &core->rb;
    core->reg[0x0A] = &core->rc;
    core->reg[0x0B] = &core->rd;
    core->reg[0x0C] = &core->re;
    core->reg[0x0D] = &core->rf;
    core->reg[0x0E] = &core->rg;
    core->reg[0x0F] = &core->rh;
    core->reg[0x10] = &core->ga;
    core->reg[0x11] = &core->gb;
    core->reg[0x12] = &core->gc;
    core->reg[0x13] = &core->gd;
    core->reg[0x14] = &core->ge;
    core->reg[0x15] = &core->gf;
    core->reg[0x16] = &core->gg;
    core->reg[0x17] = &core->gh;
    core->reg[0x18] = &core->mo;
    core->reg[0x19] = &core->ml;
    core->reg[0x1A] = &core->mh;

    /*for(uint8_t i = 0; i < 0x1A; i++)
        *core->reg[i] = 0x00;*/

    return core;
}

/*
 * Executes a cpu core;
 */
void cpu_exec_core(cpu_core_t *core)
{
    // Lets go...
    uint8_t instruction_id = 0x00;
    instruction_t instruction = NULL;
    while(1)
    {
        memory_read(core->pc++, &instruction_id);        // Read the instruction
        instruction = opcode_table[instruction_id];      // Preventing execution of unassigned opcodes
        if(instruction)
            instruction(core);              // Execute instruction
    }
}
