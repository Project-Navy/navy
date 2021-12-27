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

#include "symbols.h"

#include <stdlib.h>
#include <string.h>

static range_t symbols;

size_t hex2int(char const *s)
{
    size_t val = 0;

    while (*s)
    {
        char byte = *s++;

        if (byte >= '0' && byte <= '9')
        {
            byte = byte - '0';
        }
        else if (byte >= 'a' && byte <= 'f')
        {
            byte = byte - 'a' + 10;
        }

        val = (val << 4) | (byte & 0xf);
    }

    return val;
}

void find_symbols(uintptr_t ptr, char *buffer)
{
    char line[128] = {0};
    size_t offset = 0;

    for (uintptr_t i = 0; i < symbols.length; i++)
    {
        if (offset == 0 || line[offset - 1] != '\n')
        {
            line[offset++] = *((char *) i + symbols.base);
        }
        else
        {
            char *function;

            line[offset - 1] = 0;
            char *addr = strtok(line, " ");
            char *length = strtok(NULL, " ");

            if (*length == 'T' || *length == 't')
            {
                length = "0";
                function = strtok(NULL, " ");
            }
            else
            {
                strtok(NULL, " ");
                function = strtok(NULL, " ");
            }

            int address = hex2int(addr);
            int size = hex2int(length);

            if ((uintptr_t) address <= ptr && (uintptr_t) (address + size) >= ptr)
            {
                memcpy(buffer, function, strlen(function));
            }

            memset(line, 0, offset);
            offset = 0;
        }
    }
}

void register_symbols(range_t range)
{
    symbols = range;
}