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


#include "debug.h"

#include "arch/arch.h"

#include <navy/fmt.h>

void log_impl(char const *filename, size_t line_nbr, char const *format, struct fmt_args args)
{
    print_format(arch_serial_puts, "\033[33m{}:{}\033[0m ", filename, line_nbr);
    PRINT_FORMAT(arch_serial_puts, format, args);
    arch_serial_puts("\n");
}