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

#include <liballoc/liballoc.h>
#include <string.h>

void *kmalloc(size_t count)
{
    return liballoc_malloc(count);
}

void *kcalloc(size_t count)
{
    void *ptr = liballoc_malloc(count);
    memset(ptr, 0, count);
    return ptr;
}

void *krealloc(void *ptr, size_t count)
{
    return liballoc_realloc(ptr, count);
}

void kfree(void *ptr)
{
    liballoc_free(ptr);
}