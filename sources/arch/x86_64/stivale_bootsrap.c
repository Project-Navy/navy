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
#include "kernel/consts.h"
#include "kernel/debug.h"
#include "kernel/symbols.h"
#include "stivale2_handover.h"

#include <stdint.h>
#include <stivale/stivale2.h>
#include <navy/handover.h>

static uint8_t stack[KERNEL_STACK_SIZE];
void stivale2_entry(struct stivale2_struct *stivale2);
static struct handover handover;

static struct stivale2_header_tag_framebuffer fb_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0
    },

    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0
};

[[gnu::section(".stivale2hdr"), gnu::used]] static struct stivale2_header stivale_hdr = {
    .entry_point = (uintptr_t) stivale2_entry,
    .stack = (uintptr_t) stack + sizeof(stack),
    .flags = (1 << 1),
    .tags = (uintptr_t) &fb_tag,
};

void stivale2_entry(struct stivale2_struct *stivale2)
{
    log("Booting from limine...");
    handover = stivale2_parse_header(stivale2);
    register_symbols(handover_find_module(&handover, "symbols"));
    arch_init(&handover);
}