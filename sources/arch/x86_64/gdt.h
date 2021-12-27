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

#pragma once

#include <stdint.h>
#include <stddef.h>

#define GDT_LENGTH 5
#define GDT_KERNEL_CODE 1 
#define GDT_KERNEL_DATA 2 
#define GDT_USER_DATA 3
#define GDT_USER_CODE 4

union gdt_entry
{
    struct [[gnu::packed]]
    {
        uint16_t base_low;
        uint16_t limit_low;
        uint8_t base_mid;

        uint8_t access;
        uint8_t limit_high : 4;
        uint8_t flags : 4;
        uint8_t base_high;
    };

    uint64_t raw;
};

struct [[gnu::packed]] gdt_pointer
{
    uint16_t limit;
    uint64_t base;
};

enum gdt_bit_field
{
    GDT_KERNEL = 0b00000000,
    GDT_USER = 0b01100000,
    GDT_CODE = 0b00011000,
    GDT_DATA = 0b00010000,
};

struct [[gnu::packed]] tss
{
    uint32_t reserved;
    uint64_t rsp[3];
    uint64_t reserved2;
    uint64_t ist[7];
    unsigned __int128 reserved3 : 80;
    uint16_t iopb;
};

struct [[gnu::packed]] tss_entry 
{
    uint16_t length;
    uint16_t base_low16;
    uint8_t base_mid8;
    uint8_t flags1;
    uint8_t flags2;
    uint8_t base_high8;
    uint32_t base_upper32;
    uint32_t reserved;
};

struct [[gnu::packed]] gdt 
{
    union gdt_entry entries[GDT_LENGTH];
    struct tss_entry tss;
};

extern void gdt_flush(uintptr_t ptr);
extern void tss_flush(void);
void gdt_init(void);
void gdt_reload(void);
void intstack_init(void);
union gdt_entry gdt_entry_get(size_t index);