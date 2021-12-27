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

#include "elf.h"
#include "debug.h"
#include "arch/arch.h"
#include "kmalloc.h"
#include "consts.h"
#include "vmm.h"

#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <navy/elf.h>
#include <navy/macro.h>

process_t *load_elf_module(range_t elf, char const *path, bool is_user)
{
    log("Loading {a}-{a} ({})", elf.base, elf.base + elf.length, path);

    Elf64_Ehdr *header = (void *) elf.base;
    Elf64_Phdr *phdr = (void *)(elf.base + header->e_phoff);
    uintptr_t pml = arch_create_space();

    for (size_t i = 0; i < header->e_phnum; i++)
    {
        log("Mapping {a} -> {a} ({})", elf.base + phdr->p_offset, phdr->p_vaddr, phdr->p_type);
        if (phdr->p_type == PT_LOAD)
        {
            range_t addr = arch_pmm_alloc(ALIGN_UP(phdr->p_memsz, get_page_size()));
            assert(addr.length);

            arch_vmm_map((void *) pml, (range_t) {.base = phdr->p_vaddr, .length = addr.length}, addr, true);
            memcpy((void *) mmap_phys_to_io(addr.base), (uint8_t *) (elf.base + phdr->p_offset), phdr->p_filesz);
            memset((void *) (mmap_phys_to_io(addr.base) + phdr->p_filesz), 0, phdr->p_memsz - phdr->p_filesz);
        }

        phdr = (Elf64_Phdr *)((uint8_t *)phdr + header->e_phentsize);
    }

    log("Entry: {a}", header->e_entry);
    return task_init(path, pml, header->e_entry, is_user);
}