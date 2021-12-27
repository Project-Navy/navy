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

#include <navy/handover.h>
#include <stdint.h>

struct [[gnu::packed]] rsdp_descriptor
{
    char sign[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t rev;
    uint32_t rsdt;
};

struct [[gnu::packed]] rsdt_descriptor
{
    char sign[4];
    uint32_t length;
    uint8_t rev;
    uint8_t checksum;
    char oemid[6];
    char oemtableid[8];
    uint32_t revoem;
    uint32_t creatorid;
    uint32_t revmaker;
};

struct rsdt
{
    struct rsdt_descriptor descriptor;
    uint32_t otherPtr[];
};

struct [[gnu::packed]] madt_record
{
    uint8_t type;
    uint8_t lenght;
};

struct [[gnu::packed]] madt
{
    char sign[4];
    uint32_t length;
    uint8_t rev;
    uint8_t checksum;
    char oemid[6];
    char oemidtable[8];
    uint32_t oemrev;
    uint32_t creatid;
    uint32_t crerev;
    uint32_t lapic;
    uint32_t flags;
    struct madt_record record[];
};

enum LAPIC_REG
{
    LAPIC_CPU_ID = 0x20,
    LAPIC_EOI = 0x0b0,
};

enum APIC_TIMER
{
    APIC_TIMER_REG = 0x320,
    APIC_TIMER_INITIAL_COUNT = 0x380,
    APIC_TIMER_CURRENT_COUNT = 0x390,
    APIC_TIMER_DEVIDER = 0x3e0,
    APIC_LVT_INT_MASKED = 0x10000,
    APIC_LVT_TIMER_MODE_PERIODIC = 0x20000,
};

void acpi_init(struct handover const *handover);
void apic_timer_init(void);
uint32_t lapic_current_cpu(void);
void lapic_eoi(void);