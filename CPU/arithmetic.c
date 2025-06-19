/*
 * Headers
 *
 */
#include "../memory.h"
#include "../cpu.h"

void cpu_add(cpu_core_t *core)
{
    cpu_core_get_args(core, 2);
    *core->targ[0] += *core->targ[1];
}

void cpu_sub(cpu_core_t *core)
{
    cpu_core_get_args(core, 2);
    *core->targ[0] -= *core->targ[1];
}

void cpu_mul(cpu_core_t *core)
{
    cpu_core_get_args(core, 2);
    *core->targ[0] *= *core->targ[1];
}

void cpu_div(cpu_core_t *core)
{
    cpu_core_get_args(core, 2);
    *core->targ[0] /= *core->targ[1];
}

void cpu_inc(cpu_core_t *core)
{
    cpu_core_get_args(core, 1);
    *core->targ[0] += 1;
}

void cpu_dec(cpu_core_t *core)
{
    cpu_core_get_args(core, 1);
    *core->targ[0] -= 1;
}
