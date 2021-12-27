/**
 * Copyright (C) 2021 Keyboard Slayer
 *
 * This file is part of navy.
 *
 * navy is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * navy is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with navy.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "arch/cross-x86/asm.h"
#include "kernel/debug.h"

static uint32_t pit_read_count(void)
{
    asm_out8(0x43, 0);
    uint32_t counter = asm_in8(0x40);
    counter |= asm_in8(0x40) << 8;

    return counter;
}

void pit_init(int hz)
{
    log("Initializing PIT");
    int divisor = 1193180 / hz;

    asm_out8(0x43, 0x36);
    asm_out8(0x40, divisor & 0xff);
    asm_out8(0x40, (divisor >> 8) & 0xFF);
    log("PIT Initialized");
}

void pit_sleep(uint16_t ms)
{
    asm_out8(0x43, 0x30);
    asm_out8(0x40, ms & 0xff);
    asm_out8(0x40, (ms >> 8) & 0xff);

    while (pit_read_count() == 0);
}