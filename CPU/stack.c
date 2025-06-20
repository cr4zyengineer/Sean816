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
#include "../cpu.h"
#include "../memory.h"
#include <stdio.h>

static void cpu_prvt_push(cpu_core_t *core, uint8_t value)
{
    memory_write(--core->sp, value);
}

static void cpu_prvt_pop(cpu_core_t *core, uint8_t *value)
{
    memory_read(core->sp++, value);
}

static void cpu_prvt_push16(cpu_core_t *core, uint16_t value)
{
    cpu_prvt_push(core, value & 0xFF);
    cpu_prvt_push(core, (value >> 8) & 0xFF);
}

static void cpu_prvt_pop16(cpu_core_t *core, uint16_t *value)
{
    uint8_t lo, hi;
    cpu_prvt_pop(core, &hi); // MSB
    cpu_prvt_pop(core, &lo); // LSB
    *value = (hi << 8) | lo;
}

void cpu_push(cpu_core_t *core)
{
    cpu_core_get_args(core, 1);

    cpu_prvt_push(core, *core->targ[0]);
}

void cpu_pop(cpu_core_t *core)
{
    cpu_core_get_args(core, 1);

    cpu_prvt_pop(core, core->targ[0]);
}

void cpu_call(cpu_core_t *core)
{
    cpu_core_get_args(core, 2);

    cpu_prvt_push16(core, core->bp);        // save old base pointer
    cpu_prvt_push16(core, core->pc);        // save return address
    cpu_prvt_push(core, core->mh);
    cpu_prvt_push(core, core->ml);
    cpu_prvt_push(core, core->a);           // save registers
    cpu_prvt_push(core, core->b);
    cpu_prvt_push(core, core->c);
    cpu_prvt_push(core, core->d);
    cpu_prvt_push(core, core->e);
    cpu_prvt_push(core, core->f);
    cpu_prvt_push(core, core->g);
    cpu_prvt_push(core, core->h);
    core->bp = core->sp;                    // new stack frame base

    // TODO: If needed we need to add arguments to the calls. the issue is how do we indicate intermediate and register selection, shall it only be register selecttions!?
    // NOTE: Not really a Todo, more of an "Add the logic if current logic is sufficient"

    core->pc =  (*core->targ[1] << 8) | *core->targ[0];
}

void cpu_ret(cpu_core_t *core)
{
    core->ra = core->a;                    // Copy over register values to the return register copy
    core->rb = core->b;
    core->rc = core->c;
    core->rd = core->d;
    core->re = core->e;
    core->rf = core->f;
    core->rg = core->g;
    core->rh = core->h;

    core->sp = core->bp;                  // Moving stack pointer back to back pointer
    cpu_prvt_pop(core, &core->h);         // Popping stack back for each register
    cpu_prvt_pop(core, &core->g);
    cpu_prvt_pop(core, &core->f);
    cpu_prvt_pop(core, &core->e);
    cpu_prvt_pop(core, &core->d);
    cpu_prvt_pop(core, &core->c);
    cpu_prvt_pop(core, &core->b);
    cpu_prvt_pop(core, &core->a);
    cpu_prvt_pop(core, &core->ml);
    cpu_prvt_pop(core, &core->mh);
    cpu_prvt_pop16(core, &core->pc);
    cpu_prvt_pop16(core, &core->bp);
}
