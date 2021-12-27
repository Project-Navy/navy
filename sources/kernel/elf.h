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

#include <stdint.h>
#include <navy/range.h>
#include <navy/handover.h>

#include "kernel/sched.h"

process_t *load_elf_module(range_t elf, char const *path, bool is_user);