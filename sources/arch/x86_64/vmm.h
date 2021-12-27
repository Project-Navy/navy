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

#include "pmm.h"

#include <navy/handover.h>
#include <stdbool.h>
#include <stdint.h>

#define PMLX_GET_INDEX(addr, level)                                                                \
    (((uint64_t) addr & ((uint64_t) 0x1ff << (12 + level * 9))) >> (12 + level * 9))
#define PML4_GET_INDEX(addr) PMLX_GET_INDEX(addr, 3)
#define PML3_GET_INDEX(addr) PMLX_GET_INDEX(addr, 2)
#define PML2_GET_INDEX(addr) PMLX_GET_INDEX(addr, 1)
#define PML1_GET_INDEX(addr) PMLX_GET_INDEX(addr, 0)

struct [[gnu::packed]] pml_entry
{
    union 
    {
        uint64_t raw;

        struct [[gnu::packed]] 
        {
            bool present : 1;
            bool read_write : 1;
            bool user : 1;
            bool caching : 1;
            bool caching_disable : 1;
            bool accessed : 1;
            bool dirty : 1;
            bool huge_page : 1;
            bool global_page : 1;
            uint8_t _available : 3;
            uint64_t physical : 52;
        };
    };
};

struct [[gnu::packed]] pml
{
    struct pml_entry entries[512];
};

static inline struct pml_entry pml_make_entry(uintptr_t physical, bool is_user)
{
    return (struct pml_entry){
        .physical = physical >> 12,
        .user = is_user,
        .read_write = true,
        .present = true,
        .caching = false,
        .caching_disable = false,
        .accessed = false,
        .dirty = false,
        .huge_page = false,
        .global_page = false,
        ._available = 0,
    };
}

static inline uintptr_t mmap_phys_to_io(uintptr_t phys_addr)
{
    return phys_addr + MMAP_IO_BASE;
}

static inline uintptr_t mmap_io_to_phys(uintptr_t vaddr)
{
    return vaddr - MMAP_IO_BASE;
}

static inline uintptr_t mmap_phys_to_kernel(uintptr_t phys_addr)
{
    return phys_addr + MMAP_KERNEL_BASE;
}

static inline uintptr_t mmap_kernel_to_phys(uintptr_t vaddr)
{
    return vaddr - MMAP_KERNEL_BASE;
}

void vmm_init(struct handover const *handover);
void vmm_reload();
struct pml *get_kernel_space(void);
void vmm_unmap(struct pml *context, range_t virtual_range);
void vmm_map_range(struct pml *pml, range_t phys, range_t virt, bool is_user);
uintptr_t vmm_kernel_space(void);
uintptr_t vmm_space_create(void);
void vmm_space_destroy(struct pml *space);