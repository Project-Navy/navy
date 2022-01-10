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

#define SWITCH_TICK 9

#include <navy/range.h>
#include <navy/vec.h>

#include "arch/arch.h"
#include <stddef.h>
#include <unistd.h>

enum TASK_STATE
{
    TASK_RUNNING,
    TASK_IDLE,
    TASK_WAITINGFORDEATH,
    TASK_DEAD
};

typedef struct process
{
    uintptr_t syscall_kernel_stack;
    uintptr_t syscall_user_stack;
    uintptr_t syscall_kernel_bstack;

    range_t stack;

    registers_t regs;
    pid_t pid;
    char name[64];
    char path[256];
    uintptr_t pml;

    char *pledges;
    char *execpledges;
    
    enum TASK_STATE state;
    int return_value;
} process_t;

typedef Vec(process_t *) vec_process_t;

#define __sched_push_impl2(n, ...) sched_push_process ## n(__VA_ARGS__)
#define __sched_push_impl(n, ...) __sched_push_impl2(n, __VA_ARGS__)
#define sched_push_process(...) __sched_push_impl(GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

process_t *sched_current_process(void);
process_t *sched_get_by_pid(pid_t pid);
process_t *sched_get_by_name(char const *s);
void force_yield(void);
void task_slayer(void);

void sched_init(void);
void yield_task(registers_t *regs);
process_t *task_init(char const *path, uintptr_t pml, uintptr_t rip, bool is_user);

void sched_push_process1(process_t *new_process);
void sched_push_process2(process_t *new_process, uint64_t arg1);
void sched_push_process3(process_t *new_process, uint64_t arg1, uint64_t arg2);
void sched_push_process4(process_t *new_process, uint64_t arg1, uint64_t arg2, uint64_t arg3);
void sched_push_process5(process_t *new_process, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4);

bool is_sched_init(void);