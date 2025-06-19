/*
 * Headers
 *
 */
#include "../memory.h"
#include "../cpu.h"

void cpu_and(cpu_core_t *core)
{
    cpu_core_get_args(core, 2);

    *core->reg[core->targ[0]] &= *core->reg[core->targ[1]];
}

void cpu_or(cpu_core_t *core)
{
    cpu_core_get_args(core, 2);

    *core->reg[core->targ[0]] |= *core->reg[core->targ[1]];
}

void cpu_xor(cpu_core_t *core)
{
    cpu_core_get_args(core, 2);

    *core->reg[core->targ[0]] ^= *core->reg[core->targ[1]];
}

void cpu_not(cpu_core_t *core)
{
    cpu_core_get_args(core, 1);

    *core->reg[core->targ[0]] = ~(*core->reg[core->targ[0]]);
}
