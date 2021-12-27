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
#include <stdint.h>
#include <stddef.h>

#include <navy/macro.h>

#define IPC_DATA_LENGTH 256

typedef uint64_t pid_t;

enum UNISTD_SYSCALLS 
{
    SYS_EXIT,
    SYS_LOG,
    SYS_GETPID,
    SYS_SENDIPC,
    SYS_RECEIVEIPC,
    SYS_ALLOC,
    SYS_FREE,
    SYS_REALLOC,
    SYS_PLEDGE,
    SYS_PLEDGE_IPC,
    SYS_MEMCPY_IPC,
    SYS_KILL,
};

enum IPC_ERROR
{
    IPC_OK,
    IPC_PROCESS_NOT_PRESENT
};

typedef struct 
{
    pid_t pid_from;
    pid_t pid_to;

    char *name_to;
    uint64_t command;
    uint64_t data[IPC_DATA_LENGTH];
} ipc_msg_t;

#define __syscall_impl2(n, ...) syscall ## n(__VA_ARGS__)
#define __syscall_impl(n, ...) __syscall_impl2(n, __VA_ARGS__)
#define syscall(...) __syscall_impl(GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

uint64_t syscall1(uint64_t syscall_id);
uint64_t syscall2(uint64_t syscall_id, uint64_t arg1);
uint64_t syscall3(uint64_t syscall_id, uint64_t arg1, uint64_t arg2);
uint64_t syscall4(uint64_t syscall_id, uint64_t arg1, uint64_t arg2, uint64_t arg3);
uint64_t syscall5(uint64_t syscall_id, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4);;

void sys_log(const char *msg);
void *sys_alloc(size_t amount);
int pledge(char const *promises, char const *execpromises);
int pledge_ipc(char const *promises, char const *execpromises);
uint64_t ipc_send_msg(ipc_msg_t *msg);
void ipc_receive_msg(ipc_msg_t *msg);
void ipc_memcpy(uintptr_t dest, uintptr_t src, size_t length);

pid_t getpid();
int kill(pid_t pid);