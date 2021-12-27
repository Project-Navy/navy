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

#include "unistd.h"

uint64_t syscall1(uint64_t syscall_id)
{
    return syscall5(syscall_id, 0, 0, 0, 0);
}

uint64_t syscall2(uint64_t syscall_id, uint64_t arg1)
{
    return syscall5(syscall_id, arg1, 0, 0, 0);
}

uint64_t syscall3(uint64_t syscall_id, uint64_t arg1, uint64_t arg2)
{
    return syscall5(syscall_id, arg1, arg2, 0, 0);
}

uint64_t syscall4(uint64_t syscall_id, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
    return syscall5(syscall_id, arg1, arg2, arg3, 0);
}

uint64_t syscall5(uint64_t syscall_id, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4)
{
    uint64_t syscall_return;

    asm volatile(
        "syscall \n"
        : "=a"(syscall_return)
        : "a"(syscall_id), "b"(arg1), "d"(arg2), "S"(arg3), "D"(arg4)
        : "memory");

    return syscall_return;
}

pid_t getpid(void)
{
    return (pid_t) syscall(SYS_GETPID);
}

void sys_log(const char *s)
{
    syscall(SYS_LOG, (uint64_t) s);
}

void *sys_alloc(size_t amount)
{
    return (void *) syscall(SYS_ALLOC, amount);
}

int pledge(const char *promises, const char *execpromises)
{
    return (int) syscall(SYS_PLEDGE, (uint64_t) promises, (uint64_t) execpromises);
}

int pledge_ipc(char const *promises, char const *execpromises)
{
    return (int) syscall(SYS_PLEDGE_IPC, (uint64_t) promises, (uint64_t) execpromises);
}

uint64_t ipc_send_msg(ipc_msg_t *msg)
{
    return syscall(SYS_SENDIPC, (uintptr_t) msg);
}

void ipc_receive_msg(ipc_msg_t *msg)
{
    syscall(SYS_RECEIVEIPC, (uintptr_t) msg);
}

void ipc_memcpy(uintptr_t src, uintptr_t dest, size_t length)
{
    syscall(SYS_MEMCPY_IPC, src, dest, length);
}

int kill(pid_t pid)
{
    return (int) syscall(SYS_KILL, pid);
}