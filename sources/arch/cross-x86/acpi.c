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

#include "acpi.h"

#include "kernel/debug.h"
#include "arch/cross-x86/pit.h"

#include <assert.h>
#include <string.h>

static uintptr_t lapic_addr = 0;

static uint32_t lapic_read(uint32_t reg)
{
    assert(lapic_addr != 0);
    return *((volatile uint32_t *) (lapic_addr + reg));
}

static void lapic_write(uint32_t reg, uint32_t value)
{
    *((volatile uint32_t *) (lapic_addr + reg)) = value;
}

void acpi_init(struct handover const *handover)
{
    log("Initializing ACPI");
    size_t entries = (handover->rsdt->descriptor.length - sizeof(handover->rsdt->descriptor)) / 4;

    for (size_t i = 0; i < entries; i++)
    {
        struct rsdt_descriptor *h =
            (struct rsdt_descriptor *) ((uintptr_t) handover->rsdt->otherPtr[i]);
        if (strncmp(h->sign, "APIC", 4) == 0)
        {
            struct madt *madt = (struct madt *) h;
            log("Found lapic : {a}", madt->lapic);
            lapic_addr = madt->lapic;
        }
    }

    log("ACPI Initialized")
}

uint32_t lapic_current_cpu(void)
{
    return lapic_read(LAPIC_CPU_ID) >> 24;
}

void apic_timer_init(void)
{
    log("Initializing APIC TIMER");
    lapic_write(APIC_TIMER_DEVIDER, 0x3);
    lapic_write(APIC_TIMER_INITIAL_COUNT, 0xFFFFFFFF);

    pit_sleep(10);
    lapic_write(APIC_TIMER_REG, APIC_LVT_INT_MASKED);

    uint32_t tickIn10ms = 0xFFFFFFFF - lapic_read(APIC_TIMER_CURRENT_COUNT);

    lapic_write(APIC_TIMER_REG, 32 | APIC_LVT_TIMER_MODE_PERIODIC);
    lapic_write(APIC_TIMER_DEVIDER, 0x3);
    lapic_write(APIC_TIMER_INITIAL_COUNT, tickIn10ms);
}

void lapic_eoi(void)
{
    lapic_write(LAPIC_EOI, 0);
}