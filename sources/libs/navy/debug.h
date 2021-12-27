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
#ifndef KERNEL 

#include <stddef.h>
#include <navy/fmt.h>

#define __FILENAME__                                                                               \
    (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define log(FORMAT, ...) \
	log_impl_userspace(__FILENAME__, __LINE__, FORMAT, PRINT_ARGS(__VA_ARGS__));

void log_impl_userspace(char const *filename, size_t line_nbr, char const *format, struct fmt_args args);
#else 
#include "kernel/debug.h"
#endif