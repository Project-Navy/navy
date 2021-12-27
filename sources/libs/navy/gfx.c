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

#include "gfx.h"
#include "ansi/unistd.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <navy/debug.h>

gfx_size_t get_screen_size(void)
{
    ipc_msg_t *msg = calloc(1, sizeof(ipc_msg_t));
    gfx_size_t size;
    char *name_to = strdup("framebuffer");

    msg->name_to = name_to;
    msg->data[0] = GFX_GET_HEIGHT;
    ipc_send_msg(msg);
    ipc_receive_msg(msg);
    size.height = msg->data[0];

    msg->name_to = name_to;
    msg->data[0] = GFX_GET_WIDTH;
    ipc_send_msg(msg);
    ipc_receive_msg(msg);
    size.width = msg->data[0];

    free(name_to);
    free(msg);

    return size;
}

gfx_canvas_t create_canvas(gfx_size_t size)
{

    ipc_msg_t *msg = calloc(1, sizeof(ipc_msg_t));
    char *name_to = strdup("framebuffer");

    msg->name_to = name_to;
    msg->data[0] = GFX_GET_PITCH;
    ipc_send_msg(msg);
    ipc_receive_msg(msg);

    gfx_canvas_t canvas =  {
        .raw = calloc(1, size.height * msg->data[0]),
        .size = size 
    };

    free(name_to);
    free(msg);

    return canvas;
}

void draw_pixel_canvas(gfx_canvas_t *canvas, gfx_color_t color, size_t x, size_t y)
{
    size_t pos = x + y * canvas->size.width;

    canvas->raw[pos] = (gfx_pixel_t) {
        .blue = color.blue,
        .red = color.red,
        .green = color.green,
        .__unused = 0
    };
}

void free_canvas(gfx_canvas_t *canvas)
{
    free(canvas->raw);
}

void send_canvas_framebuffer(gfx_canvas_t *canvas, size_t x, size_t y)
{
    ipc_msg_t *msg = calloc(1, sizeof(ipc_msg_t));
    char *name_to = strdup("framebuffer");

    msg->name_to = name_to;
    msg->data[0] = GFX_ALLOC_CANVAS;
    msg->data[1] = canvas->size.width;
    msg->data[2] = canvas->size.height;

    ipc_send_msg(msg);
    ipc_receive_msg(msg);
    ipc_memcpy(msg->data[0], (uintptr_t) canvas->raw, msg->data[1]);

    msg->name_to = name_to;
    msg->data[0] = GFX_APPLY_CANVAS;
    msg->data[1] = x;
    msg->data[2] = y;
    
    ipc_send_msg(msg);
}