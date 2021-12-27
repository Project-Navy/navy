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

enum msr_star_reg
{
    STAR_KCODE_OFFSET = 32,
    STAR_UCODE_OFFSET = 48,
};

enum msr_registers
{
    MSR_APIC = 0x1B,
    MSR_EFER = 0xC0000080,
    MSR_STAR = 0xC0000081,
    MSR_LSTAR = 0xC0000082,
    MSR_COMPAT_STAR = 0xC0000083,
    MSR_SYSCALL_FLAG_MASK = 0xC0000084,
    MSR_FS_BASE = 0xC0000100,
    MSR_GS_BASE = 0xC0000101,
    MSR_KERN_GS_BASE = 0xc0000102,
};

uint8_t asm_in8(uint16_t port);
void asm_out8(uint16_t port, uint8_t data);
void io_wait(void);
void asm_write_msr(enum msr_registers msr, uint64_t value);
uint64_t asm_read_msr(enum msr_registers msr);
void asm_invlpg(uintptr_t addr);
void load_cr3(uintptr_t val);
uintptr_t read_cr3(void);
uintptr_t read_cr2(void);