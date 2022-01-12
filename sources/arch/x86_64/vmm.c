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

#include "vmm.h"

#include "arch/arch.h"
#include "arch/cross-x86/asm.h"
#include "consts.h"
#include "kernel/debug.h"
#include "pmm.h"

#include <assert.h>
#include <navy/range.h>
#include <navy/macro.h>
#include <stivale/stivale2.h>
#include <string.h>

static struct pml *kernel_pml4;
static uint32_t lock = 0;

struct pml *get_kernel_space(void)
{
    return kernel_pml4;
}

static void vmm_flush(range_t virt_range)
{
    for (size_t i = 0; i < (virt_range.length / PAGE_SIZE); i++)
    {
        asm_invlpg(((virt_range.base / PAGE_SIZE) + 1) * PAGE_SIZE);
    }
}

uintptr_t vmm_kernel_space(void)
{
    return (uintptr_t) kernel_pml4;
}

range_t vmm_get_pml_alloc(struct pml *pml, size_t index, bool is_user)
{
    struct pml_entry entry = pml->entries[index];

    if (entry.present)
    {
        range_t range = {mmap_phys_to_io(entry.physical << 12), PAGE_SIZE};
        return range;
    }
    else
    {
        range_t target_range = pmm_alloc(PAGE_SIZE);
        assert(target_range.length > 0);

        memset((void *) mmap_phys_to_io(target_range.base), 0, PAGE_SIZE);
        pml->entries[index] = pml_make_entry(target_range.base, is_user);

        return (range_t) {.base = mmap_phys_to_io(target_range.base), PAGE_SIZE};
    }
}

range_t vmm_get_pml(struct pml *pml, size_t index)
{
    struct pml_entry entry = pml->entries[index];

    if (entry.present)
    {
        return (range_t){.base = mmap_phys_to_io(entry.physical << 12), .length = PAGE_SIZE};
    }
    else
    {
        return (range_t){.base = 0, .length = 0};
    }
}

void vmm_map_page(struct pml *pml, size_t virt, size_t phys, bool is_user)
{
    range_t pml3_range = vmm_get_pml_alloc(pml, PML4_GET_INDEX(virt), true);
    struct pml *pml3 = (struct pml *) pml3_range.base;

    range_t pml2_range = vmm_get_pml_alloc(pml3, PML3_GET_INDEX(virt), true);
    struct pml *pml2 = (struct pml *) pml2_range.base;

    range_t pml1_range = vmm_get_pml_alloc(pml2, PML2_GET_INDEX(virt), true);
    struct pml *pml1 = (struct pml *) pml1_range.base;

    pml1->entries[PML1_GET_INDEX(virt)] = pml_make_entry(phys, is_user);
}

void vmm_map_range(struct pml *pml, range_t virt, range_t phys, bool is_user)
{
    assert(phys.length == virt.length);

    arch_acquire(&lock);

    for (size_t i = 0; i < (virt.length / PAGE_SIZE); i++)
    {
        size_t physaddr = i * PAGE_SIZE + ALIGN_DOWN(phys.base, PAGE_SIZE);
        size_t virtaddr = i * PAGE_SIZE + ALIGN_DOWN(virt.base, PAGE_SIZE);

        vmm_map_page(pml, virtaddr, physaddr, is_user);
    }

    arch_release(&lock);

    vmm_flush(virt);
}

void vmm_switch_space(struct pml *pml)
{
    load_cr3(mmap_io_to_phys((uintptr_t) pml));
}


void vmm_init(struct handover const *handover)
{
    log("Initializing VMM")

    range_t kernel_range = pmm_alloc(PAGE_SIZE);
    assert(kernel_range.length > 0);

    kernel_pml4 = (struct pml *) mmap_phys_to_io(kernel_range.base);
    memset(kernel_pml4, 0, PAGE_SIZE);

    log("Kernel PML4 is allocated at {a}", kernel_range.base);

    vmm_map_range(kernel_pml4, 
                (range_t) {
                    .base = mmap_phys_to_io(0),
                    .length = 0xffffffff,
                },
                (range_t) {
                    .base = 0,
                    .length = 0xffffffff,
                }, false);

    for (size_t i = 0; i < handover->memory_map_size; i++)
    {
        struct mmap_entry mmap = handover->mmap[i];
        log("Mapping Mmap {} / {}  {a} ({M})", i + 1, handover->memory_map_size, mmap.range.base,
            mmap.range.length);

        if (mmap.type == STIVALE2_MMAP_KERNEL_AND_MODULES)
        {
            vmm_map_range(kernel_pml4,
                              (range_t){
                                  .base = mmap_phys_to_kernel(ALIGN_DOWN(mmap.range.base, PAGE_SIZE)),
                                  .length = ALIGN_UP(mmap.range.length, PAGE_SIZE) + PAGE_SIZE,
                              },
                              (range_t) {
                                  .base = ALIGN_DOWN(mmap.range.base, PAGE_SIZE),
                                  .length = ALIGN_UP(mmap.range.length, PAGE_SIZE) + PAGE_SIZE,
                              },  false);
        }

        vmm_map_range(kernel_pml4, 
            (range_t){
                .base = mmap_phys_to_io(ALIGN_DOWN(mmap.range.base, PAGE_SIZE)),
                .length = ALIGN_UP(mmap.range.length, PAGE_SIZE) + PAGE_SIZE,
            },
                            
            (range_t) {
                .base = ALIGN_DOWN(mmap.range.base, PAGE_SIZE),
                .length = ALIGN_UP(mmap.range.length, PAGE_SIZE) + PAGE_SIZE,
            }, false);
    }

    vmm_switch_space(kernel_pml4);
    log("Paging enabled !");
}

static void vmm_unmap_page(struct pml *context, uintptr_t vaddr)
{
    range_t pml3_range = vmm_get_pml(context, PML4_GET_INDEX(vaddr));
    assert(pml3_range.length != 0);

    struct pml *pml3 = (struct pml *) pml3_range.base;
    range_t pml2_range = vmm_get_pml(pml3, PML3_GET_INDEX(vaddr));
    assert(pml2_range.length != 0);

    struct pml *pml2 = (struct pml *) pml2_range.base;
    range_t pml1_range = vmm_get_pml(pml2, PML2_GET_INDEX(vaddr));
    assert(pml1_range.length != 0);

    struct pml *pml1 = (struct pml *) pml1_range.base;
    pml1->entries[PML1_GET_INDEX(vaddr)] = (struct pml_entry){};
}

void vmm_unmap(struct pml *context, range_t virtual_range)
{
    arch_acquire(&lock);

    for (size_t i = 0; i < (virtual_range.length / PAGE_SIZE); i++)
    {
        vmm_unmap_page(context, i * PAGE_SIZE + (virtual_range.base & ~(PAGE_SIZE - 1)));
    }

    arch_release(&lock);
}

uintptr_t vmm_space_create(void)
{
    arch_acquire(&lock);

    range_t phys = pmm_alloc(PAGE_SIZE);
    assert(phys.length > 0);

    struct pml *pml_table = (struct pml *) mmap_phys_to_io(phys.base);
    memset(pml_table, 0, PAGE_SIZE);

    for (size_t i = 255; i < 512; i++)
    {
        pml_table->entries[i] = kernel_pml4->entries[i];
    }

    arch_release(&lock);

    return (uintptr_t) pml_table;
}

void vmm_space_destroy(struct pml *space)
{
    memset(space, 0, PAGE_SIZE);
    pmm_free((range_t) {.base = mmap_io_to_phys((uintptr_t) space), .length = PAGE_SIZE});
}