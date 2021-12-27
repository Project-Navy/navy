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

#include "idt.h"

#include "kernel/debug.h"

#include <string.h>

struct idt_descr kidt[256] = {};
struct idt_ptr kidtr;
extern uintptr_t __interrupt_vector[];

void idt_descr_init(uintptr_t offset, enum idt_attr type, struct idt_descr *this, uint8_t ist)
{
    this->offset_1 = offset;
    this->offset_2 = (offset >> 16);
    this->offset_3 = (offset >> 32);

    this->ist = ist;
    this->selector = 0x08;
    this->type_attr = type;
    this->zero = 0;
}

void idt_init(void)
{
    log("Initializing IDT");

    memset(kidt, 0, 256 * sizeof(struct idt_descr));

    for (size_t i = 0; i < 3; i++)
    {
        idt_descr_init(__interrupt_vector[i], INTGATE, &kidt[i], 0);
    }

    idt_descr_init(__interrupt_vector[3], TRAPGATE, &kidt[3], 0);
    idt_descr_init(__interrupt_vector[4], TRAPGATE, &kidt[4], 0);

    for (size_t i = 5; i < 48; i++)
    {
        idt_descr_init(__interrupt_vector[i], INTGATE, &kidt[i], 0);
    }

    kidtr.base = (uintptr_t) &kidt[0];
    kidtr.limit = sizeof(kidt) - 1;

    idt_reload();

    log("IDT initialized");
}

void idt_reload(void)
{
    idt_flush((uintptr_t) &kidtr);
}
