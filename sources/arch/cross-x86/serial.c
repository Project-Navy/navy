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

#include "serial.h"

#include "arch/arch.h"
#include "arch/cross-x86/asm.h"

#include <stdbool.h>

static bool is_serial_init = false;

void serial_init(uint16_t port)
{
    asm_out8(port + 1, 0x00);
    asm_out8(port + 0, 0x03);
    asm_out8(port + 1, 0x00);
    asm_out8(port + 3, 0x03);
    asm_out8(port + 2, 0xc7);
    asm_out8(port + 4, 0x0b);
    asm_out8(port + 4, 0x1e);
    asm_out8(port + 0, 0xae);
    asm_out8(port + 4, 0x0f);

    is_serial_init = true;
}

void serial_putc(char c)
{
    if (!is_serial_init)
    {
        serial_init(SERIAL_COM1);
    }

    while ((asm_in8(SERIAL_COM1 + 5) & 0x20) == 0);

    asm_out8(SERIAL_COM1, c);
}

void serial_puts(char const *s)
{
    while (*s != '\0')
    {
        serial_putc(*s++);
    }
}
