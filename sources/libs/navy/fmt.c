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

#include "fmt.h"

#include <navy/itoa.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct fmt_value fmtvald(int64_t val)
{
    return (struct fmt_value){
        .type = FMT_DECIMAL,
        {.as_decimal = val},
    };
}

struct fmt_value fmtvals(char const *val)
{
    return (struct fmt_value){
        .type = FMT_STRING,
        {.as_str = val},
    };
}

struct fmt_value fmtvalc(char val)
{
    return (struct fmt_value){
        .type = FMT_CHAR,
        {.as_chr = val},
    };
}

char fmt_scanner_next(struct fmt_scanner *scanner)
{
    return scanner->format[++scanner->offset];
}

char fmt_scanner_current(struct fmt_scanner *scanner)
{
    if (scanner->offset < scanner->size)
    {
        return scanner->format[scanner->offset];
    }

    return '\0';
}

void PRINT_FORMAT(void (*callback)(char const *s), char const *format, struct fmt_args args)
{
    struct fmt_scanner scanner = {.format = format, .size = strlen(format), .offset = 0};

    size_t current_value = 0;
    char char_buffer[2] = {0};

    while (fmt_scanner_current(&scanner) != '\0')
    {
        char mode = 0;
        if (fmt_scanner_current(&scanner) == '{')
        {
            while (fmt_scanner_current(&scanner) != '}' && fmt_scanner_current(&scanner) != '\0')
            {
                fmt_scanner_next(&scanner);
                if (fmt_scanner_current(&scanner) != '}')
                {
                    //kassert(mode == 0);
                    mode = fmt_scanner_current(&scanner);
                }
            }

            if (current_value < args.count)
            {
                switch (args.values[current_value].type)
                {
                    case FMT_DECIMAL:
                    {
                        char unit[3] = {0, 'B', 0};
                        char buffer[256];
                        char padBuffer[32] = {0};
                        char base = 10;
                        size_t pad = 0;
                        int64_t nbr = args.values[current_value].as_decimal;
                        memset(buffer, 0, 256);

                        switch (mode)
                        {
                            case 'a':
                            {
                                base = 16;
                                pad = 16;
                                break;
                            }
                            case 'x':
                            {
                                base = 16;
                                pad = 0;
                                break;
                            }
                            case 'M':
                            {
                                base = 10;
                                pad = 0;

                                if (nbr / 1073741824 >= 1)
                                {
                                    nbr /= 1073741824;
                                    unit[0] = 'G';
                                }
                                else if (nbr / 1048576 >= 1)
                                {
                                    nbr /= 1048576;
                                    unit[0] = 'M';
                                }
                                else if (nbr / 1024 >= 1)
                                {
                                    nbr /= 1024;
                                    unit[0] = 'K';
                                }
                                else
                                {
                                    unit[0] = 'B';
                                    unit[1] = 0;
                                }

                                break;
                            }
                            default:
                            {
                                base = 10;
                                pad = 0;
                                break;
                            }
                        }

                        itoa(nbr, buffer, base);

                        if (strlen(buffer) < pad)
                        {
                            memset(padBuffer, '0', pad - strlen(buffer));
                            callback(padBuffer);
                        }

                        callback(buffer);

                        if (unit[0] > 0)
                        {
                            callback(unit);
                        }

                        break;
                    }

                    case FMT_STRING:
                    {
                        callback(args.values[current_value].as_str);
                        break;
                    }

                    case FMT_CHAR:
                    {
                        char_buffer[0] = args.values[current_value].as_chr;
                        callback(char_buffer);
                        break;
                    }

                    default:
                    {
                        callback("<error>");
                        break;
                    }
                }

                current_value++;
            }
        }

        else
        {
            char_buffer[0] = fmt_scanner_current(&scanner);
            callback(char_buffer);
        }

        fmt_scanner_next(&scanner);
    }
}
