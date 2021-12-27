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

#include "itoa.h"

#include <stddef.h>
#include <string.h>

void strrev(char *s)
{
    char tmp;
    size_t end = strlen(s) - 1;
    size_t start = 0;

    while (start < end)
    {
        tmp = s[start];
        s[start++] = s[end];
        s[end--] = tmp;
    }
}

char *itoa(int64_t value, char *str, uint16_t base)
{
    size_t index = 0;
    int8_t digit;

    if (value < 0 && base != 10)
    {
        int64_t left;
        int64_t right;
        size_t i;

        char left_str[32];
        char right_str[32];

        right = value & 0xffffffff;
        left = (value & 0xffffffff00000000) >> 32;

        itoa(left, left_str, base);
        itoa(right, right_str, base);

        for (i = 0; i < strlen(left_str); i++)
        {
            str[i] = left_str[i];
        }

        for (i = 0; i < strlen(right_str); i++)
        {
            str[i + strlen(left_str)] = right_str[i];
        }

        return str;
    }

    else if (value < 0)
    {
        str[index++] = '-';
        value *= 1;
    }

    do
    {
        digit = value % base;

        if (digit < 0xa)
        {
            str[index++] = digit + '0';
        }
        else
        {
            str[index++] = (digit - 0xa) + 'a';
        }

        value /= base;
    } while (value);

    str[index] = 0;

    strrev(str);

    return str;
}