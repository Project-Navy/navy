/**
 * Copyright (C) 2022 Keyboard Slayer
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

#include <navy/debug.h>
#include <navy/ipc.h>

static void receiver(void)
{
    ipc_t msg;
    ipc_rcv_sync(&msg);
    log("Got IPC from {} with value {x}", msg.sender, msg.data);
}

static void receiver_oneshot(void)
{
    ipc_t msg = {0};
    for (; msg.sender == 0; ipc_rcv_oneshot(&msg));
    log("Got IPC ONESHOT from {} with value {x}", msg.sender, msg.data)
}

static void sender(void)
{
    ipc_t msg = {
        .data = 0xdeadbeef,
        .receiver = 1
    };

    ipc_t msg2 = {
        .data = 0xcafebabe,
        .receiver = 2
    };

    ipc_send(&msg);
    ipc_send(&msg2);
}

int main(void)
{
    switch(getpid())
    {
        case 1:
        {
            receiver();
            break;
        }

        case 2:
        {
            receiver_oneshot();
            break;
        }

        case 3:
        {
            sender();
            break;
        }
    }

    return 0;
}