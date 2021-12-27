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

#include "stivale2_handover.h"

#include "arch/arch.h"
#include "arch/cross-x86/acpi.h"
#include "consts.h"
#include "kernel/debug.h"
#include "pmm.h"

#include <navy/handover.h>
#include <string.h>

static struct stivale2_struct_tag_smp *smp = NULL;

void parse_mmap(struct handover *this, struct stivale2_struct_tag_memmap *mmap)
{
    struct stivale2_mmap_entry mmap_entry;
    struct mmap_entry *entry;

    for (size_t i = 0; i < mmap->entries; i++)
    {
        mmap_entry = mmap->memmap[i];

        entry = &this->mmap[this->memory_map_size];
        entry->is_free = false;

        log("Found mmap at {a}", mmap_entry.base);

        if (mmap_entry.type == STIVALE2_MMAP_USABLE)
        {
            log("Found a usable memory map of {M} ", mmap_entry.length);
            this->memory_usable += mmap_entry.length;
            entry->is_free = true;
        }

        entry->range = (struct range){.base = mmap_entry.base, .length = mmap_entry.length};

        entry->type = mmap_entry.type;

        this->memory_map_size++;
    }

    this->usable_pages = this->memory_usable / PAGE_SIZE;
}

void parse_module(struct handover *handover, struct stivale2_struct_tag_modules *mod)
{
    for (size_t i = 0; i < mod->module_count; i++)
    {
        struct stivale2_module m = mod->modules[i];
        struct handover_module module;

        log("Found module named: {}", m.string);

        module.addr = (range_t) {.base = m.begin, .length = m.end - m.begin};
        memset(&module.name, 0, 64);
        memcpy(&module.name, m.string, strlen(m.string));
        memcpy(&handover->modules[handover->module_count++], &module, sizeof(struct handover_module));
    }
}

static void parse_rsdp(struct handover *handover, struct stivale2_struct_tag_rsdp *tag)
{
    struct rsdp_descriptor *rsdp = (struct rsdp_descriptor *) tag->rsdp;

    struct rsdt *rsdt = (struct rsdt *) ((uintptr_t) rsdp->rsdt);
    handover->rsdt = rsdt;
}

static void parse_framebuffer(struct handover *handover,
                              struct stivale2_struct_tag_framebuffer *tag)
{
    struct framebuffer fb = {.addr = tag->framebuffer_addr,
                             .height = tag->framebuffer_height,
                             .width = tag->framebuffer_width,
                             .pitch = tag->framebuffer_pitch};

    handover->framebuffer = fb;
}

struct handover stivale2_parse_header(struct stivale2_struct *stivale)
{
    struct handover handover = {0};

    struct stivale2_tag *tag = (struct stivale2_tag *) ((uintptr_t) stivale->tags);
    while (tag != NULL)
    {
        switch (tag->identifier)
        {
            case STIVALE2_STRUCT_TAG_MEMMAP_ID:
            {
                parse_mmap(&handover, (struct stivale2_struct_tag_memmap *) tag);
                break;
            }
            case STIVALE2_STRUCT_TAG_MODULES_ID:
            {
                parse_module(&handover, (struct stivale2_struct_tag_modules *) tag);
                break;
            }
            case STIVALE2_STRUCT_TAG_SMP_ID:
            {
                smp = (struct stivale2_struct_tag_smp *) tag;
                break;
            }
            case STIVALE2_STRUCT_TAG_RSDP_ID:
            {
                parse_rsdp(&handover, (struct stivale2_struct_tag_rsdp *) tag);
                break;
            }
            case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID:
            {
                parse_framebuffer(&handover, (struct stivale2_struct_tag_framebuffer *) tag);
                break;
            }

            default:
            {
                log("Unknown tag: {x}", tag->identifier);
            }
        }

        tag = (struct stivale2_tag *) ((uintptr_t) tag->next);
    }

    return handover;
}