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

#define MASTER_CMD      0x20
#define MASTER_DATA     0x21
#define MASTER_OFFSET   0x20

#define SLAVE_CMD       0xa0
#define SLAVE_DATA      0xa1
#define SLAVE_OFFSET    0x28

void pic_init(void);
void pic_sendEOI(int);