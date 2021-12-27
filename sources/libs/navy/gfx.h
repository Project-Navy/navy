// Copyright (C) 2021 Keyboard Slayer
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

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>


typedef struct [[gnu::packed]]
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t __unused;
} gfx_pixel_t;

typedef enum 
{
    GFX_GET_HEIGHT,
    GFX_GET_WIDTH,
    GFX_GET_PITCH,
    GFX_APPLY_CANVAS,
    GFX_ALLOC_CANVAS,
} gfx_cmd_t;

typedef struct 
{
    size_t height;
    size_t width;
} gfx_size_t;

typedef struct 
{
    gfx_pixel_t *raw;
    gfx_size_t size;
} gfx_canvas_t;

typedef struct 
{
    union 
    {
        struct [[gnu::packed]]
        {
            uint8_t blue;
            uint8_t red;
            uint8_t green;
        };

        uint32_t _raw;
    };
} gfx_color_t;

gfx_size_t get_screen_size(void);
gfx_canvas_t create_canvas(gfx_size_t size);
void draw_pixel_canvas(gfx_canvas_t *canvas, gfx_color_t color, size_t x, size_t y);
void free_canvas(gfx_canvas_t *canvas);
void send_canvas_framebuffer(gfx_canvas_t *canvas, size_t x, size_t y);