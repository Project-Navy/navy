/**
 * Copyright (C) 2021 keyboard-slayer
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

#include "tar.h"

size_t tar_get_size(char const *in)
{
    size_t size = 0;
    size_t count = 1;
 
    for (size_t i = 11; i > 0; i--, count *= 8)
    {
        size += ((in[i - 1] - '0') * count);
    }
 
    return size;
}