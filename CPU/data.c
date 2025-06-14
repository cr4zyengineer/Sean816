/*
 * Headers
 *
 */
#include "../memory.h"
#include "../cpu.h"
#include <stdio.h>
#include <unistd.h>

void cpu_load(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading the mode
    memory_read(core->pc++, &core->tb);     // Reading the target register
    memory_read(core->pc++, &core->tc);     // Reading the first value that used in both modes

    uint8_t value = 0;

    if(core->ta != 0x00)
    {
        memory_read(core->pc++, &core->td); // Second value needed for 16bit
        memory_read(((core->tc << 8) | core->td) + core->mo, &value);
    } else {
        value = core->tc;
    }

    *(core->reg[core->tb]) = value;
}

void cpu_store(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading the target register
    memory_read(core->pc++, &core->tb);     // Reading first 8bits of address
    memory_read(core->pc++, &core->tc);     // Reading second 8bits of address

    memory_write(((core->tb << 8) | core->tc) + core->mo, *core->reg[core->ta]);
}

void cpu_mlmh(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading first 8bits of address
    memory_read(core->pc++, &core->tb);     // Reading second 8bits of address

    core->ml = core->ta;
    core->mh = core->tb;
}

void cpu_loadlh(cpu_core_t *core)
{
    memory_read(core->pc++, &core->tb);     // Reading the target register

    uint8_t value = 0;

    memory_read(((core->ml << 8) | core->mh), &value);

    *(core->reg[core->tb]) = value;
}

void cpu_storelh(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading the target register

    memory_write(((core->ml << 8) | core->mh), *core->reg[core->ta]);
}

void cpu_mov(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading destination register
    memory_read(core->pc++, &core->tb);     // Reading source register

    *core->reg[core->ta] = *core->reg[core->tb];
}
