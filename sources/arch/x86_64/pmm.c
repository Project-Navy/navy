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

#include "pmm.h"
#include "vmm.h"

#include "consts.h"
#include "kernel/debug.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <navy/macro.h>

uint8_t *bitmap = NULL;
static size_t bitmap_size = 0;
static size_t usable_pages = 0;
static size_t last_page = 0;

void bitmap_unused(size_t index)
{
    uint64_t bit = index % 8;
    uint64_t byte = index / 8;

    bitmap[byte] &= ~(1 << bit);
    usable_pages++;
}

void bitmap_used(size_t index)
{
    uint64_t bit = index % 8;
    uint64_t byte = index / 8;

    bitmap[byte] |= (1 << bit);
    usable_pages--;
}

bool is_set_pmm(size_t index)
{
    uint64_t bit = index % 8;
    uint64_t byte = index / 8;

    return bitmap[byte] & (1 << bit);
}

void pmm_range_used(struct range range)
{
    size_t target = range.base / PAGE_SIZE;

    for (size_t i = 0; i < range.length / PAGE_SIZE; i++)
    {
        bitmap_used(target + i);
    }
}

void pmm_range_unused(struct range range)
{
    size_t target = range.base / PAGE_SIZE;
    for (size_t i = 0; i < range.length / PAGE_SIZE; i++)
    {
        bitmap_unused(target + i);
    }
}

struct range pmm_find_pages(size_t amount)
{
    struct range range = {0};

    for (size_t i = last_page; i < bitmap_size && range.length < amount; i++)
    {
        if (!is_set_pmm(i))
        {
            if (range.length == 0)
            {
                range.base = i * PAGE_SIZE;
            }

            range.length += PAGE_SIZE;
        }
        else
        {
            range.length = 0;
        }
    }

    if (range.length >= amount)
    {
        last_page = (range.base + range.length) / PAGE_SIZE;

        pmm_range_used(range);
        return range;
    }
    else
    {
        if (last_page == 0)
        {
            return (struct range){
                .base = 0,
                .length = 0,
            };
        }

        return pmm_find_pages(amount);
    }
}

struct range pmm_alloc(size_t amount)
{
    assert(usable_pages > 0);
    return pmm_find_pages(amount);
}

void pmm_free(struct range range)
{
    pmm_range_unused(range);
}

size_t get_usable_pages(void)
{
    return usable_pages;
}

void pmm_init(struct handover *handover)
{
    log("Initializing PMM");

    struct mmap_entry last_mmap = handover->mmap[handover->memory_map_size - 1];
    size_t memory_end = last_mmap.range.base + last_mmap.range.length;
    bitmap_size = memory_end / (PAGE_SIZE * 8);

    log("The bitmap needs {} bytes", bitmap_size);

    for (size_t i = 0; i < handover->memory_map_size && bitmap == NULL; i++)
    {
        struct mmap_entry *mmap = &handover->mmap[i];
        if (mmap->is_free && mmap->range.length > bitmap_size)
        {
            log("Found mmap for bitmap at 0x{a}", mmap->range.base);
            bitmap = (uint8_t *) mmap_phys_to_io(mmap->range.base);
            
            mmap->range.length -= ALIGN_UP(bitmap_size, PAGE_SIZE);
            mmap->range.base += ALIGN_UP(bitmap_size, PAGE_SIZE);
        }
    }

    assert(bitmap != NULL);
    memset(bitmap, 0xff, bitmap_size);

    for (size_t i = 0; i < handover->memory_map_size; i++)
    {
        struct mmap_entry mmap = handover->mmap[i];

        if (mmap.is_free)
        {
            pmm_range_unused((range_t) {
                .base = ALIGN_DOWN(mmap.range.base, PAGE_SIZE),
                .length = ALIGN_UP(mmap.range.length, PAGE_SIZE)
            });
        }
    }

    pmm_range_used((range_t) {
        .base = mmap_io_to_phys((uintptr_t) bitmap),
        .length = bitmap_size
    });

    log("PMM Initialized");
}
