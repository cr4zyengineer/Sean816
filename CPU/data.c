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
    memory_read(core->pc++, &core->ta);     // Reading the mode
    memory_read(core->pc++, &core->tb);     // Reading the target register
    memory_read(core->pc++, &core->tc);     // Reading the first value that used in both modes

    if(core->ta != 0x00)
    {
        memory_read(core->pc++, &core->td); // Second value needed for 16bit
        memory_read(gather16Bit(core->tc, core->td) + core->mo, core->reg[core->tb]);
    } else {
        *core->reg[core->tb] = core->tc;
    }
}

void cpu_store(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading the target register
    memory_read(core->pc++, &core->tb);     // Reading first 8bits of address
    memory_read(core->pc++, &core->tc);     // Reading second 8bits of address

    memory_write(gather16Bit(core->tb, core->tc) + core->mo, *core->reg[core->ta]);
}

void cpu_mhml(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading first 8bits of address
    memory_read(core->pc++, &core->tb);     // Reading second 8bits of address

    core->ml = core->ta;
    core->mh = core->tb;
}

void cpu_loadlh(cpu_core_t *core)
{
    memory_read(core->pc++, &core->tb);

    memory_read(gather16Bit(core->ml, core->mh), core->reg[core->tb]);
}

void cpu_storelh(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);

    memory_write(gather16Bit(core->ml, core->mh), *core->reg[core->ta]);
}

void cpu_mov(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading destination register
    memory_read(core->pc++, &core->tb);     // Reading source registe

    *core->reg[core->ta] = *core->reg[core->tb];
}
