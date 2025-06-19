/*
 * Headers
 *
 */
#include "../memory.h"
#include "../cpu.h"
#include "endian.h"
#include <unistd.h>

void cpu_load(cpu_core_t *core)
{
    cpu_core_get_args(core, 3);

    if(core->targ[0] != 0x00)
    {
        cpu_core_get_args(core, 1);
        memory_read(gather16Bit(core->targ[2], core->targ[3]) + core->mo, core->reg[core->targ[1]]);
    } else
        *core->reg[core->targ[1]] = core->targ[2];
}

void cpu_store(cpu_core_t *core)
{
    cpu_core_get_args(core, 3);

    memory_write(gather16Bit(core->targ[1], core->targ[2]) + core->mo, *core->reg[core->targ[0]]);
}

void cpu_mhml(cpu_core_t *core)
{
    cpu_core_get_args(core, 2);

    core->ml = core->targ[0];
    core->mh = core->targ[1];
}

void cpu_loadlh(cpu_core_t *core)
{
    cpu_core_get_args(core, 1);

    memory_read(gather16Bit(core->ml, core->mh), core->reg[core->targ[0]]);
}

void cpu_storelh(cpu_core_t *core)
{
    cpu_core_get_args(core, 1);

    memory_write(gather16Bit(core->ml, core->mh), *core->reg[core->targ[0]]);
}

void cpu_mov(cpu_core_t *core)
{
    cpu_core_get_args(core, 2);

    *core->reg[core->targ[0]] = *core->reg[core->targ[1]];
}
