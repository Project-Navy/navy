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

#ifdef __x86_64__
#include "arch/x86_64/interrupt.h"
typedef struct interrupt_stackframe registers_t;
#endif

#include <navy/handover.h>
#include <navy/range.h>
#include <stdbool.h>

void arch_serial_puts(char const *s);
void arch_hlt(void);
void raise_debug(void);

void arch_init(struct handover *handover);
size_t get_usable_pages(void);
size_t get_page_size(void);

extern void arch_acquire(uint32_t *lock);
extern void arch_release(uint32_t *lock);

uintptr_t arch_create_space(void);
uintptr_t arch_kernel_space(void);
void arch_vmm_map(void *pml, range_t virt, range_t phys, bool is_user);
void arch_vmm_unmap(void *pml, range_t virt);
range_t arch_pmm_alloc(size_t amount);
void arch_space_switch(uintptr_t space);
void arch_space_destroy(void *pml);
void arch_context_switch(void *ctx, registers_t *regs);
void arch_context_save(void *ctx, registers_t *regs);
uintptr_t arch_read_space(void);
void arch_enable_scheduler(void);