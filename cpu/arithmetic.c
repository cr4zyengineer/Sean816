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
    core->a = *core->reg[core->ta] + *core->reg[core->tb];
}

void cpu_sub(cpu_core_t *core)
{
    cpu_math_arg_get(core);
    core->a = *core->reg[core->ta] - *core->reg[core->tb];
}

void cpu_mul(cpu_core_t *core)
{
    cpu_math_arg_get(core);
    core->a = *core->reg[core->ta] * *core->reg[core->tb];
}

void cpu_div(cpu_core_t *core)
{
    cpu_math_arg_get(core);
    core->a = *core->reg[core->ta] / *core->reg[core->tb];
}

void cpu_inc(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading register

    *core->reg[core->ta] = *core->reg[core->ta] + 1;
}

void cpu_dec(cpu_core_t *core)
{
    memory_read(core->pc++, &core->ta);     // Reading register

    *core->reg[core->ta] = *core->reg[core->ta] - 1;
}

/*
 * Signed ALU operations
 */
void cpu_sadd(cpu_core_t *core)
{
    cpu_math_arg_get(core);
    int8_t a = (int8_t)(*core->reg[core->ta]);
    int8_t b = (int8_t)(*core->reg[core->tb]);
    int8_t res = a + b;
    core->a = (uint8_t)res;
}

void cpu_ssub(cpu_core_t *core)
{
    cpu_math_arg_get(core);
    int8_t a = (int8_t)(*core->reg[core->ta]);
    int8_t b = (int8_t)(*core->reg[core->tb]);
    int8_t res = a - b;
    core->a = (uint8_t)res;
}

void cpu_smul(cpu_core_t *core)
{
    cpu_math_arg_get(core);
    int8_t a = (int8_t)(*core->reg[core->ta]);
    int8_t b = (int8_t)(*core->reg[core->tb]);
    int8_t res = a * b;
    core->a = (uint8_t)res;
}

void cpu_sdiv(cpu_core_t *core)
{
    cpu_math_arg_get(core);
    int8_t a = (int8_t)(*core->reg[core->ta]);
    int8_t b = (int8_t)(*core->reg[core->tb]);
    if (b == 0) {
        // Handle division by zero as needed
        core->a = 0;
        return;
    }
    int8_t res = a / b;
    core->a = (uint8_t)res;
}
