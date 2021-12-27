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

#include "asm.h"
#include "pic.h"

void pic_init(void)
{
    asm_out8(MASTER_CMD, 0x10 | 0x01);
    io_wait();
    asm_out8(SLAVE_CMD, 0x10 | 0x01);
    io_wait();

    asm_out8(MASTER_DATA, MASTER_OFFSET);
    io_wait();
    asm_out8(SLAVE_DATA, SLAVE_OFFSET);
    io_wait();

    asm_out8(MASTER_DATA, 4);
    io_wait();
    asm_out8(SLAVE_DATA, 2);
    io_wait();

    asm_out8(MASTER_DATA, 0x01);
    io_wait();
    asm_out8(SLAVE_DATA, 0x01);
    io_wait();

    asm_out8(MASTER_DATA, 0);
    io_wait();
    asm_out8(SLAVE_DATA, 0);
    io_wait();
}

void pic_sendEOI(int intno)
{
    if (intno >= 40)
    {
        asm_out8(SLAVE_CMD, 0x20);
    }

    asm_out8(MASTER_CMD, 0x20);
}