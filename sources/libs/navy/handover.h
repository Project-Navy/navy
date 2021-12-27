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

#define LIMIT_MEMORY_MAP_SIZE 64
#define LIMIT_SMP_SIZE 64

#include <navy/range.h>
#include <stdbool.h>
#include <stddef.h>

struct mmap_entry
{
    range_t range;
    uint16_t type;
    bool is_free;
};

struct framebuffer
{
    uintptr_t addr;
    uint16_t height;
    uint16_t width;
    uint16_t pitch;
};

struct handover_module 
{
    char name[128];
    range_t addr;
};

typedef struct handover
{
    size_t memory_usable;
    size_t usable_pages;
    size_t memory_map_size;
    struct rsdt *rsdt;

    struct framebuffer framebuffer;

    struct mmap_entry mmap[LIMIT_MEMORY_MAP_SIZE];

    size_t module_count;
    struct handover_module modules[64];
} handover_t;

range_t handover_find_module(struct handover const *handover, char const *name);