/*
 * Headers
 *
 */
#include "../cpu.h"
#include "../memory.h"
#include <stdio.h>

static void cpu_prvt_push(cpu_core_t *core, uint8_t value)
{
    memory_write(--core->sp, value);
}

static void cpu_prvt_pop(cpu_core_t *core, uint8_t *value)
{
    memory_read(core->sp++, value);
}

static void cpu_prvt_push16(cpu_core_t *core, uint16_t value)
{
    cpu_prvt_push(core, value & 0xFF);
    cpu_prvt_push(core, (value >> 8) & 0xFF);
}

static void cpu_prvt_pop16(cpu_core_t *core, uint16_t *value)
{
    uint8_t lo, hi;
    cpu_prvt_pop(core, &lo); // LSB
    cpu_prvt_pop(core, &hi); // MSB
    *value = ((uint16_t)lo << 8) | hi;
}

void cpu_push(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);    // Reading the value that is getting pushed
    cpu_prvt_push(core, *core->reg[core->ta]);
}

void cpu_pop(cpu_core_t *core)
{
    // NOTE: Deny popping over to the caller snapshooted memory and tempering it by checking that we dont overstep it
    if(core->bp >= core->sp) return;

    memory_read(core->pc++, &core->ta);    // Reading the value that is getting pushed
    cpu_prvt_pop(core, core->reg[core->ta]);
}

void cpu_call(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading first 8bits of address
    memory_read(core->pc++, &core->tb);     // Reading second 8bits of address

    cpu_prvt_push16(core, core->bp);     // save old base pointer
    cpu_prvt_push16(core, core->pc);     // save return address
    cpu_prvt_push(core, core->mo);
    cpu_prvt_push(core, core->a);        // save registers
    cpu_prvt_push(core, core->b);
    cpu_prvt_push(core, core->c);
    cpu_prvt_push(core, core->d);
    cpu_prvt_push(core, core->e);
    cpu_prvt_push(core, core->f);
    cpu_prvt_push(core, core->g);
    cpu_prvt_push(core, core->h);
    core->bp = core->sp;                 // new stack frame base

    // TODO: If needed we need to add arguments to the calls. the issue is how do we indicate intermediate and register selection, shall it only be register selecttions!?
    // NOTE: Not really a Todo, more of an "Add the logic if current logic is sufficient"

    core->pc =  (core->ta << 8) | core->tb;
}

void cpu_ret(cpu_core_t *core)
{
    core->ra = core->a;             // Copy over register values to the return register copy
    core->rb = core->b;
    core->rc = core->c;
    core->rd = core->d;
    core->re = core->e;
    core->rf = core->f;
    core->rg = core->g;
    core->rh = core->h;

    core->sp = core->bp;            // Moving stack pointer back to back pointer
    cpu_prvt_pop(core, &core->h);   // Popping stack back for each register
    cpu_prvt_pop(core, &core->g);
    cpu_prvt_pop(core, &core->f);
    cpu_prvt_pop(core, &core->e);
    cpu_prvt_pop(core, &core->d);
    cpu_prvt_pop(core, &core->c);
    cpu_prvt_pop(core, &core->b);
    cpu_prvt_pop(core, &core->a);
    cpu_prvt_pop(core, &core->mo);
    cpu_prvt_pop16(core, &core->pc);
    cpu_prvt_pop16(core, &core->bp);
}
