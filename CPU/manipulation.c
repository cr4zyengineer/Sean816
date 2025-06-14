/*
 * Headers
 *
 */
#include "../memory.h"
#include "../cpu.h"

void cpu_and(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading register
    memory_read(core->pc++, &core->tb);     // Reading register
    *core->reg[core->ta] &= *core->reg[core->tb];
}

void cpu_or(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading register
    memory_read(core->pc++, &core->tb);     // Reading register
    *core->reg[core->ta] |= *core->reg[core->tb];
}

void cpu_xor(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading register
    memory_read(core->pc++, &core->tb);     // Reading register
    *core->reg[core->ta] ^= *core->reg[core->tb];
}

void cpu_not(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading register
    *core->reg[core->ta] = ~(*core->reg[core->ta]);
}
