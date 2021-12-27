// Copyright (C) 2021 keyboard-slayer
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
#include <navy/vec.h>

typedef struct 
{
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
} tar_header_t;

size_t tar_get_size(char const *in);