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

#include <stdlib.h>
#include <string.h>

#define reader_t(T)                                                            \
    struct                                                                     \
    {                                                                          \
        T content;                                                             \
        size_t offset;                                                         \
        size_t length;                                                         \
    }

#define reader_init(r, t, l)                                                   \
    (memset((r), 0, sizeof(*(r))), (r)->content = (t), (r)->length = (l))

#define reader_next(r) (r)->content[(r)->offset++]

#define reader_next_now(r) (r)->content[++(r)->offset]

#define reader_peek(r) (r)->content[(r)->offset]

#define reader_eof(r) ((r)->offset >= (r)->length)

typedef reader_t(char **) reader_str_t;