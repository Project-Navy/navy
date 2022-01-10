// Copyright (C) 2022 Keyboard Slayer
// 
// This file is part of navy.
// 
// navy is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// navy is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with navy.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void vec_expand_(char **data, size_t *length, size_t *capacity, int memsz);

#define Vec(T)                                                                                     \
    struct                                                                                         \
    {                                                                                              \
        T *data;                                                                                   \
        size_t length;                                                                             \
        size_t capacity;                                                                           \
        size_t iter;                                                                               \
    }

#define vec_init(v) memset((v), 0, sizeof(*(v)))

#define vec_push(v, val)                                                                           \
    vec_expand_((char **) &(v)->data, &(v)->length, &(v)->capacity, sizeof(*(v)->data));           \
    (v)->data[(v)->length++] = (val)

#define vec_foreach(v, t)                                                                          \
    if ((v)->length > 0)                                                                           \
        for ((v)->iter = 0; (v)->iter < (v)->length && (((t) = (v)->data[(v)->iter]), 1);          \
             ++(v)->iter)

#define vec_free(v) free((v)->data)

typedef Vec(uint8_t) vec_uint8_t;