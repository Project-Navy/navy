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

#include <navy/ipc.h>
#include <unistd.h>

void ipc_send(ipc_t *ipc)
{
    syscall(SYS_IPC_SEND, (uintptr_t) ipc);
}

void ipc_rcv_sync(ipc_t *ipc)
{
    syscall(SYS_IPC_RCV_SYNC, (uintptr_t) ipc);
}

void ipc_rcv_oneshot(ipc_t *ipc)
{
    syscall(SYS_IPC_RCV_ONESHOT, (uintptr_t) ipc);
}