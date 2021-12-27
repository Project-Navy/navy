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

#include "sched.h"
#include "arch/arch.h"
#include "navy/vec.h"
#include "kmalloc.h"
#include "debug.h"
#include "consts.h"
#include "vmm.h"

#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

vec_process_t processes;

static size_t tick = 0;
static uint32_t lock = 0;
static uint32_t usertask_lock = 0;
static pid_t current_pid = 0;
static pid_t last_pid = 1;
static bool sched_is_init = false;

static pid_t next_pid(void)
{
    if (current_pid + 1 == (size_t) processes.length)
    {
        current_pid = -1;
    }

    return ++current_pid;
}

void yield_task(registers_t *regs)
{
    arch_acquire(&lock);

    if (++tick >= SWITCH_TICK + 1)
    {
        tick = 0;
        process_t *current_process = processes.data[current_pid];
        arch_context_save(current_process, regs);

        while ((current_process = processes.data[next_pid()])->state != TASK_RUNNING);

        arch_context_switch(current_process, regs);

        if (current_pid){}
        arch_space_switch(current_process->pml);
    }

    arch_release(&lock);
}

void force_yield(void)
{
    tick = SWITCH_TICK;
}

void sched_init(void)
{
    vec_init(&processes);

    process_t *kernel_process = kcalloc(sizeof(process_t));
    kernel_process->pid = 0;
    kernel_process->state = TASK_RUNNING;

    memcpy(kernel_process->name, "Kernel", 7);
    memcpy(kernel_process->path, "Kernel", 7);
    kernel_process->pml = mmap_phys_to_io(arch_read_space());

    assert(vec_push(&processes, kernel_process) == 0);

    arch_enable_scheduler();
    sched_is_init = true;
}

void task_slayer(void)
{
    for(;;)
    {
        for (int i = 0; i < processes.length; i++)
        {
            process_t *proc = processes.data[i];
            if (proc->state == TASK_WAITINGFORDEATH)
            {
                kfree((void *) proc->syscall_kernel_bstack);
                pmm_free(proc->stack);
                arch_space_destroy((void *) proc->pml);

                if (proc->pledges != NULL)
                {
                    kfree((void *) proc->pledges);
                }

                if (proc->execpledges != NULL)
                {
                    kfree((void *) proc->execpledges);
                }
            }
        }
    }
}

process_t *task_init(char const *path, uintptr_t pml, uintptr_t rip, bool is_user)
{
    arch_acquire(&usertask_lock);
    process_t *new_task = kcalloc(sizeof(process_t));

    memset(new_task->name, 0, 64);
    memcpy(new_task->path, path, strlen(path));
    new_task->pml = pml; 
    new_task->state = TASK_RUNNING;

    new_task->pledges = NULL;
    new_task->execpledges = NULL;

    if (is_user)
    {
        new_task->regs.cs = 0x23;
        new_task->regs.ss = 0x1b;
    }

    new_task->regs.rip = rip;

    new_task->syscall_kernel_bstack = (uintptr_t) kmalloc(KERNEL_STACK_SIZE);
    new_task->syscall_kernel_stack = new_task->syscall_kernel_bstack + KERNEL_STACK_SIZE;

    new_task->stack = pmm_alloc(KERNEL_STACK_SIZE);
    assert(new_task->stack.length);

    arch_vmm_map((void *) new_task->pml,(range_t) {
        .base = (uintptr_t) USER_STACK_BASE,
        .length = KERNEL_STACK_SIZE,
    }, new_task->stack, true);

    new_task->regs.rsp = USER_STACK_BASE + KERNEL_STACK_SIZE;
    new_task->regs.rbp = USER_STACK_BASE;
    new_task->regs.rflags = 0x202;

    arch_release(&usertask_lock);

    return new_task;
}

int sched_push_process1(process_t *new_process)
{
    return sched_push_process5(new_process, 0, 0, 0, 0);
}

int sched_push_process2(process_t *new_process, uint64_t arg1)
{
    return sched_push_process5(new_process, arg1, 0, 0, 0);
}

int sched_push_process3(process_t *new_process, uint64_t arg1, uint64_t arg2)
{
    return sched_push_process5(new_process, arg1, arg2, 0, 0);
}

int sched_push_process4(process_t *new_process, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
    return sched_push_process5(new_process, arg1, arg2, arg3, 0);
}

int sched_push_process5(process_t *new_process, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4)
{
    new_process->pid = last_pid++;

    new_process->regs.rdi = arg1;
    new_process->regs.rsi = arg2;
    new_process->regs.rdx = arg3;
    new_process->regs.rcx = arg4;

    return vec_push(&processes, new_process) == 0;
}

process_t *sched_current_process(void)
{
    return processes.data[current_pid];
}

process_t *sched_get_by_pid(pid_t pid)
{
    if (processes.length > (int) pid && processes.data[pid]->state == TASK_RUNNING)
    {
        return processes.data[pid];
    }
    else 
    {
        return NULL;
    }
}

process_t *sched_get_by_name(char const *s)
{
    for (int i = 0; i < processes.length; i++)
    {
        if (strcmp(processes.data[i]->name, s) == 0)
        {
            return processes.data[i];
        }
    }

    return NULL;
}

bool is_sched_init(void)
{
    return sched_is_init;
}
