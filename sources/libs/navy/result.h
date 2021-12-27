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

#include <stdbool.h>

#define result_t(TError, TOk)                                                  \
    struct                                                                     \
    {                                                                          \
        bool success;                                                          \
        union                                                                  \
        {                                                                      \
            TError _error;                                                     \
            TOk _ok;                                                           \
        };                                                                     \
    }

#define result_err(T, Err)                                                     \
    (T)                                                                        \
    {                                                                          \
        .success = false, ._error = Err                                        \
    }

#define result_ok(T, Res)                                                      \
    (T)                                                                        \
    {                                                                          \
        .success = true, ._ok = Res                                            \
    }
