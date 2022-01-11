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

#include "interrupt.h"

#include "arch/arch.h"

#include "arch/cross-x86/asm.h"
#include "arch/cross-x86/rtc.h"
#include "arch/cross-x86/pic.h"

#include "kernel/symbols.h"
#include "kernel/debug.h"
#include "kernel/sched.h"
#include "kernel/kmalloc.h"
#include "kernel/consts.h"

#include <navy/fmt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static bool is_scheduler = false;
static bool has_fault = false;

static const char *exceptions[32] = {"Division by zero",
                                     "Debug",
                                     "Non-maskable Interrupt",
                                     "Breakpoint",
                                     "Overflow",
                                     "Bound Range Exceeded",
                                     "Invalid Opcode",
                                     "Device Not Available",
                                     "Double Fault",
                                     "Coprocessor Segment Overrun",
                                     "Invalid TSS",
                                     "Segment Not Present",
                                     "Stack-Segment Fault",
                                     "General Protection Fault",
                                     "Page Fault",
                                     "Reserved",
                                     "x87 Floating-Point Exception",
                                     "Aligment Check",
                                     "Machine Check",
                                     "SIMD Floating-Point Exception",
                                     "Virtualization Exception",
                                     "Reserved",
                                     "Reserved",
                                     "Reserved",
                                     "Reserved",
                                     "Reserved",
                                     "Reserved",
                                     "Reserved",
                                     "Reserved",
                                     "Reserved",
                                     "Security Exception",
                                     "Reserved"};

static const char *comments[] = {
    "Panic At The Kernel !",
    "Press F to pay respect.",
    "Could not boot up kernel because no",
    "Booting up is not supported on this kernel",
    "Seems like Duolingo halted the kernel",
    "Hey /r/softwaregore",
    "Believe me, everything is fine :)",
    "The kernel defined that 1 + 1 = 3",
    "Congratulations, this kernel can now run Half Life 3 !",
    "Still more stable than Windows",
    "Looks like the CPU took a lil' nap",
    "Stability is bloat",
    "The kernel took some vacations",
    "https://www.youtube.com/watch?v=dQw4w9WgXcQ",
    "Still faster developpement than GNU Hurd",
    "Did you try to rewrite the kernel in rust ?",
    "Better of doing a megalithic kernel",
    "Keyboard Slayer, just contribute to brutal wtf",
    "The dev of this kernel is the worst at copying and pasting",
    "Maybe if you create your own cpu architecture it will work",
    "DevseChan, what if we kissed during the kernel panic",
    "Paging hell is real",
};

static void backtrace(uint64_t rbp)
{
    struct stackframe *stackframe = (struct stackframe *) rbp;
    char function[64] = {0};

    while (stackframe)
    {
        find_symbols(stackframe->rip, function);

        if (function[0] == 0)
        {
            print_format(arch_serial_puts, "{a}\n", stackframe->rip);
        }
        else
        {
            print_format(arch_serial_puts, "{a} ({})\n", stackframe->rip, function);
        }

        memset(function, 0, 64);
        stackframe = stackframe->rbp;
    }
}

void arch_enable_scheduler(void)
{
    is_scheduler = true;
}

uint64_t interrupts_handler(uint64_t rsp)
{
    if (has_fault) 
    {
        log("DOUBLE FAULT HALTING !!");
        asm volatile("cli");
        for(;;);
    }

    struct interrupt_stackframe *stackframe = (struct interrupt_stackframe *) rsp;

    if (stackframe->intno < 32)
    {
        has_fault = true;
        char function[64] = {0};
        srandom(rtc_sec());

        int funny_id = random() % (sizeof(comments) / sizeof(comments[0]));

        find_symbols(stackframe->rip, function);

        print_format(arch_serial_puts, " ____________________\n");
        print_format(arch_serial_puts, "/                    \\\n");
        print_format(arch_serial_puts, "!    Kernel Panic    !\n");
        print_format(arch_serial_puts, "!   (Do not panic)   !\n");
        print_format(arch_serial_puts, "\\____________________/\n");
        print_format(arch_serial_puts, "         !  !\n");
        print_format(arch_serial_puts, "         !  !\n");
        print_format(arch_serial_puts, "         L_ !\n");
        print_format(arch_serial_puts, "        / _)!\n");
        print_format(arch_serial_puts, "       / /__L\n");
        print_format(arch_serial_puts, " _____/ (____)\t\t\t{}\n", comments[funny_id]);
        print_format(arch_serial_puts, "        (____)\t\t\tException: {}\n",
                     exceptions[stackframe->intno]);
        print_format(arch_serial_puts, " _____  (____)\t\t\tError: {}\n", stackframe->err);
        print_format(arch_serial_puts, "      \\_(____)\n");
        print_format(arch_serial_puts, "         !  !\n");
        print_format(arch_serial_puts, "         !  !\n");
        print_format(arch_serial_puts, "         \\__/ \n");
        arch_serial_puts("\n");
        print_format(arch_serial_puts, "RAX={a} RBX={a} RCX={a} RDX={a}\n", stackframe->rax,
                     stackframe->rbx, stackframe->rcx, stackframe->rdx);
        print_format(arch_serial_puts, "RSI={a} RDI={a} RBP={a} RSP={a}\n", stackframe->rsi,
                     stackframe->rdi, stackframe->rbp, stackframe->rsp);
        print_format(arch_serial_puts, "R8= {a} R9= {a} R10={a} R11={a}\n", stackframe->r8,
                     stackframe->r9, stackframe->r10, stackframe->r11);
        print_format(arch_serial_puts, "R12={a} R13={a} R14={a} R15={a}\n", stackframe->r12,
                     stackframe->r13, stackframe->r14, stackframe->r15);
        print_format(arch_serial_puts, "CR2={a} CR3={a}\n", read_cr2(), read_cr3());
        print_format(arch_serial_puts, "RIP={a} ({})\n", stackframe->rip, function);

        if (is_sched_init())
        {
            print_format(arch_serial_puts, "PID: {}", sched_current_process()->pid);
        }

        arch_serial_puts("\n *** Backtrace *** \n");
        backtrace(stackframe->rbp);

        arch_hlt();
    }

    else if (stackframe->intno < 48)
    {
        uint8_t irq = stackframe->intno - 32;

        switch(irq)
        {
            case 0:
            {
                if (is_scheduler)
                {
                    yield_task(stackframe);
                }
                break;
            }
        }
    }
    else if (stackframe->intno == 0x80)
    {
        arch_serial_puts((char const *) stackframe->rdi);
    }

    pic_sendEOI(stackframe->intno);
    return rsp;
}