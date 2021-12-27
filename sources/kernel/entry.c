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

#include "debug.h"
#include "navy/vec.h"
#include "kernel/consts.h"
#include "kernel/elf.h"
#include "kernel/kmalloc.h"
#include "sched.h"
#include "interrupt.h"
#include "vmm.h"

#include <assert.h>
#include <navy/handover.h>

void kernel_splash(void)
{
    size_t pages = get_usable_pages();
    arch_serial_puts("\n__/\\\\\\\\\\_____/\\\\\\____________________________________________        \n");
    arch_serial_puts(" _\\/\\\\\\\\\\\\___\\/\\\\\\____________________________________________       \n");
    arch_serial_puts("  _\\/\\\\\\/\\\\\\__\\/\\\\\\_________________________________/\\\\\\__/\\\\\\_      \n");
    arch_serial_puts("   _\\/\\\\\\//\\\\\\_\\/\\\\\\__/\\\\\\\\\\\\\\\\\\_____/\\\\\\____/\\\\\\___\\//\\\\\\/\\\\\\__     \n");
    arch_serial_puts("    _\\/\\\\\\\\//\\\\\\\\/\\\\\\_\\////////\\\\\\___\\//\\\\\\__/\\\\\\_____\\//\\\\\\\\\\___    \n");
    arch_serial_puts("     _\\/\\\\\\_\\//\\\\\\/\\\\\\___/\\\\\\\\\\\\\\\\\\\\___\\//\\\\\\/\\\\\\_______\\//\\\\\\____   \n");
    arch_serial_puts("      _\\/\\\\\\__\\//\\\\\\\\\\\\__/\\\\\\/////\\\\\\____\\//\\\\\\\\\\_____/\\\\_/\\\\\\_____  \n");
    arch_serial_puts("       _\\/\\\\\\___\\//\\\\\\\\\\_\\//\\\\\\\\\\\\\\\\/\\\\____\\//\\\\\\_____\\//\\\\\\\\/______ \n");
    arch_serial_puts("        _\\///_____\\/////___\\////////\\//______\\///_______\\////________\n\n");


    log("> Usable memory: {M} Usable pages: {}", pages * get_page_size(), pages);
}

void kmain([[gnu::unused]] handover_t const *handover)
{
    kernel_splash();
    sched_init();

    log("Microkernel is dead :(");

    task_slayer();
}