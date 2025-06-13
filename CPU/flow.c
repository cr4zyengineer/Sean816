/*
 * Headers
 *
 */
#include "../memory.h"
#include "../cpu.h"
#include <stdio.h>

void cpu_jmp(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading first 8bits of address
    memory_read(core->pc++, &core->tb);     // Reading second 8bits of address

    core->pc =  (core->ta << 8) | core->tb;
}

void cpu_cmp(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading first register target
    memory_read(core->pc++, &core->tb);     // Reading second register target

    if(*core->reg[core->ta] == *core->reg[core->tb]) // JE would suceed
        core->cmp = 0;
    else if(*core->reg[core->ta] > *core->reg[core->tb]) // JG, JNE would succeed
        core->cmp = 1;
    else if(*core->reg[core->ta] < *core->reg[core->tb]) // JL, JNE would suceed
        core->cmp = 2;
}

void cpu_je(cpu_core_t *core)
{
    if(core->cmp != 0)
    {
        core->pc = core->pc + 2;
        return;
    }

    memory_read(core->pc++, &core->ta);     // Reading first 8bits of address
    memory_read(core->pc++, &core->tb);     // Reading second 8bits of address

    core->pc = (core->ta << 8) | core->tb;
}

void cpu_jne(cpu_core_t *core)
{
    if(core->cmp == 0)
    {
        core->pc = core->pc + 2;
        return;
    }

    memory_read(core->pc++, &core->ta);     // Reading first 8bits of address
    memory_read(core->pc++, &core->tb);     // Reading second 8bits of address

    core->pc = (core->ta << 8) | core->tb;
}

void cpu_jg(cpu_core_t *core)
{
    if(core->cmp != 1)
    {
        core->pc = core->pc + 2;
        return;
    }

    memory_read(core->pc++, &core->ta);     // Reading first 8bits of address
    memory_read(core->pc++, &core->tb);     // Reading second 8bits of address

    core->pc = (core->ta << 8) | core->tb;
}

void cpu_jl(cpu_core_t *core)
{
    if(core->cmp != 2)
    {
        core->pc = core->pc + 2;
        return;
    }

    memory_read(core->pc++, &core->ta);     // Reading first 8bits of address
    memory_read(core->pc++, &core->tb);     // Reading second 8bits of address

    core->pc = (core->ta << 8) | core->tb;
}
