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

#include "gdt.h"

#include "kernel/debug.h"
#include "kernel/kmalloc.h"
#include "kernel/consts.h"

#include <stdint.h>
#include <string.h>

static struct gdt gdt;
static struct gdt_pointer ptr = {.limit = sizeof(struct gdt) - 1, .base = (uintptr_t) &gdt};
static struct tss tss = {0};

union gdt_entry gdt_entry_get(size_t index)
{
    return gdt.entries[index];
}

void init_descriptor(union gdt_entry *this, uint32_t base, uint32_t limit, uint8_t access)
{
    if (access != GDT_DATA)
    {
        this->flags = 0b0010;
    }
    else
    {
        this->flags = 0;
    }

    this->access = 0b10000010 | access;

    this->base_low = base & 0xffff;
    this->base_mid = (base & 0xff0000) >> 16;
    this->base_high = (base & 0xff000000) >> 24;

    this->limit_low = limit & 0xffff;
    this->limit_high = (limit & 0xf0000) >> 16;
}

static struct tss_entry init_tss(uintptr_t tss)
{
    return (struct tss_entry)
    {
        .length = sizeof(struct tss_entry),
        .base_low16 = (uint16_t) tss & 0xffff,
        .base_mid8 = (uint8_t) (tss >> 16 ) & 0xff,
        .flags1 = 0b10001001,
        .flags2 = 0,
        .base_high8 = (uint8_t) (tss >> 24) & 0xff,
        .base_upper32 = tss >> 32,
        .reserved = 0,
    };
}

void gdt_init(void)
{
    log("Initializing GDT");

    init_descriptor(&gdt.entries[0], 0, 0, 0);

    init_descriptor(&gdt.entries[1], 0, 0, GDT_KERNEL | GDT_CODE);
    init_descriptor(&gdt.entries[2], 0, 0, GDT_KERNEL | GDT_DATA);

    init_descriptor(&gdt.entries[3], 0, 0, GDT_USER | GDT_DATA);
    init_descriptor(&gdt.entries[4], 0, 0, GDT_USER | GDT_CODE);

    gdt.tss = init_tss((uintptr_t) &tss);
    tss.iopb = sizeof(struct tss);

    gdt_reload();
    tss_flush();

    log("GDT Initialized");
}

void gdt_reload(void)
{
    __asm__ volatile("cli");
    gdt_flush((uintptr_t) &ptr);
    __asm__ volatile("sti");
}

void intstack_init(void)
{
    tss.ist[0] = (uint64_t) kmalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    tss.ist[1] = (uint64_t) kmalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    tss.rsp[0] = (uint64_t) kmalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
}