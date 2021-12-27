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

#include <stdint.h>
#include "kernel/debug.h"

uint8_t asm_in8(uint16_t port)
{
    uint8_t data;
    __asm__ volatile("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

void asm_out8(uint16_t port, uint8_t data)
{
    __asm__ volatile("outb %0, %1" ::"a"(data), "Nd"(port));
}

void io_wait(void)
{
    __asm__ volatile ("jmp 1f\n\t" "1:jmp 2f\n\t" "2:");
}

void asm_write_msr(enum msr_registers msr, uint64_t value)
{
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    asm volatile("wrmsr" :: "c"((uint64_t)msr), "a"(low), "d"(high));
}

uint64_t asm_read_msr(enum msr_registers msr)
{
    uint32_t low, high;
    asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"((uint64_t)msr));
    return ((uint64_t) high << 32) | low;
}

void asm_invlpg(uintptr_t addr)
{
    asm volatile("invlpg (%0)" ::"r"(addr): "memory");
}

uintptr_t read_cr3(void)
{
    uintptr_t value;
    __asm__ volatile("mov %%cr3, %0" : "=r"(value));

    return value;
}

uintptr_t read_cr2(void)
{
    uintptr_t value;
    __asm__ volatile("mov %%cr2, %0" : "=r"(value));

    return value;
}

void load_cr3(uintptr_t val)
{
    asm volatile("mov %0, %%cr3" ::"a"(val));
}
