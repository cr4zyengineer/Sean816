/*
 * Headers
 *
 */
#include "../memory.h"
#include "../cpu.h"

static void cpu_math_arg_get(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading register
    memory_read(core->pc++, &core->tb);     // Reading second register
}

/*
 * Unsigned ALU operations
 */
void cpu_add(cpu_core_t *core)
{
    cpu_math_arg_get(core);
    *core->reg[core->ta] += *core->reg[core->tb];
}

void cpu_sub(cpu_core_t *core)
{
    cpu_math_arg_get(core);
    *core->reg[core->ta] -= *core->reg[core->tb];
}

void cpu_mul(cpu_core_t *core)
{
    cpu_math_arg_get(core);
    *core->reg[core->ta] *= *core->reg[core->tb];
}

void cpu_div(cpu_core_t *core)
{
    cpu_math_arg_get(core);
    *core->reg[core->ta] /= *core->reg[core->tb];
}

void cpu_inc(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);
    *core->reg[core->ta] += 1;
}

void cpu_dec(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);
    *core->reg[core->ta] -= 1;
}
