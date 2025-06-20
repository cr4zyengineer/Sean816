/*
 * MIT License
 *
 * Copyright (c) 2025 Cr4zy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
