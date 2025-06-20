/*
 * Headers
 *
 */
#include "../memory.h"
#include "../cpu.h"
#include <stdio.h>

void cpu_jmp(cpu_core_t *core)
{
    cpu_core_get_args(core, 2);

    core->pc = (*core->targ[1] << 8) | *core->targ[0];
}

void cpu_cmp(cpu_core_t *core)
{
    cpu_core_get_args(core, 2);

    if(*core->targ[0] == *core->targ[1])
        core->cmp = 0;
    else if(*core->targ[0] > *core->targ[1])
        core->cmp = 1;
    else
        core->cmp = 2;
}

void cpu_je(cpu_core_t *core)
{
    if(core->cmp != 0)
    {
        core->pc = core->pc + 2;
        return;
    }

    cpu_core_get_args(core, 2);

    core->pc = (*core->targ[1] << 8) | *core->targ[0];
}

void cpu_jne(cpu_core_t *core)
{
    if(core->cmp == 0)
    {
        core->pc = core->pc + 2;
        return;
    }

    cpu_core_get_args(core, 2);

    core->pc = (*core->targ[1] << 8) | *core->targ[0];
}

void cpu_jg(cpu_core_t *core)
{
    if(core->cmp != 1)
    {
        core->pc = core->pc + 2;
        return;
    }

    cpu_core_get_args(core, 2);

    core->pc = (*core->targ[1] << 8) | *core->targ[0];
}

void cpu_jl(cpu_core_t *core)
{
    if(core->cmp != 2)
    {
        core->pc = core->pc + 2;
        return;
    }

    cpu_core_get_args(core, 2);

    core->pc = (*core->targ[1] << 8) | *core->targ[0];
}
