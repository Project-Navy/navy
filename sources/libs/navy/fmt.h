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

#include <map-macro/map.h>
#include <stddef.h>
#include <stdint.h>
#include "macro.h"

#define SELECT_VALUE(VALUE)                                                                        \
    _Generic(\
    (VALUE), \
    int64_t: fmtvald, \
    uint8_t: fmtvald, \
    uint16_t: fmtvald, \
    uint32_t: fmtvald, \
    size_t: fmtvald, \
    int: fmtvald, \
    char*: fmtvals, \
    char const*: fmtvals, \
    char: fmtvalc)(VALUE),

#define PRINT_ARGS_(...)                                                                           \
    (struct fmt_args)                                                                              \
    {                                                                                              \
        0, (struct fmt_value[]){},                                                                 \
    }

#define PRINT_ARGS_N(...)                                                                          \
    (struct fmt_args)                                                                              \
    {                                                                                              \
        GET_ARG_COUNT(__VA_ARGS__), (struct fmt_value[]){MAP(SELECT_VALUE, __VA_ARGS__)},          \
    }

#define PRINT_ARGS(...) PRINT_ARGS_##__VA_OPT__(N)(__VA_ARGS__)

#define print_format(OUTFN, FORMAT, ...) PRINT_FORMAT(OUTFN, FORMAT, PRINT_ARGS(__VA_ARGS__))

enum fmt_type
{
    FMT_CHAR,
    FMT_STRING,
    FMT_DECIMAL
};

struct fmt_value
{
    enum fmt_type type;
    union
    {
        int64_t as_decimal;
        char const *as_str;
        char as_chr;
    };
};

struct fmt_scanner
{
    char const *format;
    size_t size;
    size_t offset;
};

struct fmt_args
{
    size_t count;
    struct fmt_value *values;
};

struct fmt_value fmtvald(int64_t val);
struct fmt_value fmtvals(char const *val);
struct fmt_value fmtvalc(char val);
void PRINT_FORMAT(void (*callback)(char const *s), char const *format, struct fmt_args args);

char fmt_scanner_next(struct fmt_scanner *parser);
char fmt_scanner_current(struct fmt_scanner *parser);