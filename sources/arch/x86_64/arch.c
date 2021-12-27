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

#include "arch/arch.h"

#include "arch/cross-x86/asm.h"
#include "arch/cross-x86/pit.h"
#include "arch/cross-x86/serial.h"
#include "arch/cross-x86/pic.h"

#include "kernel/entry.h"
#include "kernel/debug.h"
#include "kernel/sched.h"

#include "consts.h"
#include "gdt.h"
#include "idt.h"
#include "pmm.h"
#include "vmm.h"

extern void syscall_handle(void);
static uint32_t lock = 0;

static void syscall_init(void)
{
    asm_write_msr(MSR_EFER, asm_read_msr(MSR_EFER) | 1);
    asm_write_msr(MSR_STAR, ((uint64_t)(GDT_KERNEL_CODE * 8) << STAR_KCODE_OFFSET) | ((uint64_t) (((GDT_USER_DATA - 1) * 8) | 3 ) << STAR_UCODE_OFFSET));
    asm_write_msr(MSR_LSTAR, (uint64_t) syscall_handle);
    asm_write_msr(MSR_SYSCALL_FLAG_MASK, 0);
}

void arch_init(struct handover *handover)
{
    log("Initializing modules for x86-64");

    gdt_init();
    idt_init();
    pmm_init(handover);
    vmm_init(handover);
    intstack_init();
    pit_init(1000);
    pic_init();
    syscall_init();

    log("Arch Initialized !");

    kmain(handover);
}

void arch_hlt(void)
{
    __asm__ volatile("cli");
    __asm__ volatile("hlt");
}

void raise_debug(void)
{
    __asm__ volatile("int $1");
}

size_t get_page_size(void)
{
    return PAGE_SIZE;
}

uintptr_t arch_create_space(void)
{
    return vmm_space_create();
}

uintptr_t arch_kernel_space(void)
{
    return vmm_kernel_space();
}

range_t arch_pmm_alloc(size_t amount)
{
    return pmm_alloc(amount);
}

void arch_vmm_map(void *pml, range_t virt, range_t phys, bool is_user)
{
    vmm_map_range(pml, virt, phys, is_user);
}

void arch_vmm_unmap(void *pml, range_t virt)
{
    vmm_unmap(pml, virt);
}

void arch_serial_puts(char const *s)
{
    arch_acquire(&lock);
    serial_puts(s);
    arch_release(&lock);
}

void arch_space_switch(uintptr_t ctx)
{
    load_cr3(mmap_io_to_phys(ctx));
}

uintptr_t arch_read_space(void)
{
    return read_cr3();
}

void arch_context_switch(void *ctx, registers_t *regs)
{
    asm_write_msr(MSR_GS_BASE, (uintptr_t) ctx);
    asm_write_msr(MSR_KERN_GS_BASE, (uintptr_t) ctx);

    *regs = ((process_t *) ctx)->regs;
}

void arch_space_destroy(void *pml)
{
    vmm_space_destroy(pml);
}

void arch_context_save(void *ctx, registers_t *regs)
{
    ((process_t *) ctx)->regs = *regs;
}