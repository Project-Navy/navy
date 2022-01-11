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

#include <stdint.h>
#include <unistd.h>
#include <navy/ipc.h>
#include <navy/debug.h>
#include <assert.h>

#include "arch/arch.h"
#include "sched.h"

typedef int64_t (*syscall_t)(registers_t *);

int64_t syscall_exit(registers_t *regs)
{
    process_t *current = sched_current_process();
    log("PID {} exited {}", current->pid, regs->rbx);
    current->return_value = regs->rbx;
    current->state = TASK_WAITINGFORDEATH;
    for(;;);
}

int64_t syscall_log(registers_t *regs)
{
    char const *msg = (char const *) regs->rbx;
    arch_serial_puts(msg);

    return 0;
}

int64_t syscall_getpid([[gnu::unused]] registers_t *regs)
{
    return sched_current_process()->pid;
}

int64_t syscall_ipcsend(registers_t *regs)
{
    ipc_t *msg = (void *) regs->rbx;
    msg->sender = sched_current_process()->pid;

    process_t *proc = sched_get_by_pid(msg->receiver);
    assert(proc != NULL);

    vec_push(&proc->mailbox, *msg);
    return 0;
}

int64_t syscall_ipcrcv_sync(registers_t *regs)
{
    process_t *proc = sched_current_process();
    ipc_t *msg = (void *) regs->rbx;

    while (proc->mailbox.length == 0)
    {
        force_yield();
    }


    *msg = vec_pop(&proc->mailbox);
    return 0;
}

int64_t syscall_ipcrcv_oneshot(registers_t *regs)
{
    process_t *proc = sched_current_process();
    ipc_t *msg = (void *) regs->rbx;

    if (proc->mailbox.length != 0)
    {
        *msg = vec_pop(&proc->mailbox);
    }

    return 0;
}

syscall_t syscall_matrix[] = {
        [SYS_IPC_SEND] = syscall_ipcsend,
        [SYS_IPC_RCV_SYNC] = syscall_ipcrcv_sync,
        [SYS_IPC_RCV_ONESHOT] = syscall_ipcrcv_oneshot,
        [SYS_LOG] = syscall_log,
        [SYS_EXIT] = syscall_exit,
        [SYS_GETPID] = syscall_getpid
};

int64_t syscall_handler(registers_t *regs)
{
    return syscall_matrix[regs->rax](regs);
}
