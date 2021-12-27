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

#include "rtc.h"

#include "arch/cross-x86/asm.h"

static int is_updating(void)
{
    asm_out8(0x70, 0x0a);
    return asm_in8(0x71) & 0x80;
}

static uint8_t read(uint8_t reg)
{
    while (is_updating())
        ;

    asm_out8(0x70, reg);
    return asm_in8(0x71);
}

uint8_t rtc_sec(void)
{
    uint8_t sec = read(0);
    return (sec & 0xf) + ((sec / 0x10) * 0xa);
}