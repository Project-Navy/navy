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
#include "arch/x86_64/pmm.h"
#include "arch/x86_64/vmm.h"

#include "consts.h"
#include "kernel/debug.h"
#include "kernel/sched.h"

#include <assert.h>
#include <navy/range.h>
#include <navy/macro.h>

static uint32_t lock = 0;

int liballoc_lock(void)
{
    arch_acquire(&lock);
    return 0;
}

int liballoc_unlock(void)
{
    arch_release(&lock);
    return 0;
}

void *liballoc_alloc(int size)
{
    bool is_user = true;
    struct pml *space;

    if (is_sched_init())
    {
        space = (struct pml *) sched_current_process()->pml;
    }
    else 
    {
        space = get_kernel_space();
    }

    range_t phys_range = pmm_alloc(size * PAGE_SIZE);
    assert(phys_range.length > 0);

    vmm_map_range(space, (range_t){
        .base = mmap_phys_to_io(phys_range.base),
        .length = phys_range.length
    }, phys_range, is_user);

    return (void *) mmap_phys_to_io(phys_range.base);
}

int liballoc_free_(void *ptr, int pages)
{
    pmm_free((range_t) {.base = (uintptr_t) ptr, .length = pages});
    return 0;
}