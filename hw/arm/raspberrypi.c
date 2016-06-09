/*
 * Raspberry Pi rev B emulation
 *
 * Copyright (C) 2014 Nolaan
 * Written by Nolaan
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#include "hw/sysbus.h"
#include "hw/devices.h"
#include "hw/boards.h"
#include "hw/arm/allwinner-a10.h"
#include "hw/arm/raspberrypi.h"

#define RPI_FIMD0_BASE_ADDR 0xC0000000

static struct arm_boot_info raspberrypi_binfo = {
    .loader_start = AW_A10_SDRAM_BASE,
    .board_id = 0x1009,
};

typedef struct RapsberryPiState {
    AwA10State *a10;
    MemoryRegion sdram;
} RapsberryPiState;

static void raspberrypi_init(MachineState *args)
{
    RapsberryPiState *s = g_new(RapsberryPiState, 1);
    /* DeviceState *dev; */
    Error *err = NULL;
    /* SysBusDevice *sysbusdev; */
    /* qemu_irq fiq, irq; */
    /* uint8_t i; */

    s->a10 = AW_A10(object_new(TYPE_AW_A10));

    object_property_set_int(OBJECT(&s->a10->emac), 1, "phy-addr", &err);
    if (err != NULL) {
        error_report("Couldn't set phy address: %s", error_get_pretty(err));
        exit(1);
    }

    object_property_set_int(OBJECT(&s->a10->timer), 32768, "clk0-freq", &err);
    if (err != NULL) {
        error_report("Couldn't set clk0 frequency: %s", error_get_pretty(err));
        exit(1);
    }

    object_property_set_int(OBJECT(&s->a10->timer), 24000000, "clk1-freq",
                            &err);
    if (err != NULL) {
        error_report("Couldn't set clk1 frequency: %s", error_get_pretty(err));
        exit(1);
    }

    object_property_set_bool(OBJECT(s->a10), true, "realized", &err);
    if (err != NULL) {
        error_report("Couldn't realize Allwinner A10: %s",
                     error_get_pretty(err));
        exit(1);
    }

    memory_region_init_ram(&s->sdram, NULL, "raspberrypi.ram", args->ram_size);
    vmstate_register_ram_global(&s->sdram);
    memory_region_add_subregion(get_system_memory(), AW_A10_SDRAM_BASE,
                                &s->sdram);

    raspberrypi_binfo.ram_size = args->ram_size;
    raspberrypi_binfo.kernel_filename = args->kernel_filename;
    raspberrypi_binfo.kernel_cmdline = args->kernel_cmdline;
    arm_load_kernel(&s->a10->cpu, &raspberrypi_binfo);
}

static QEMUMachine raspberrypi_machine = {
    .name = "raspberrypi",
    .desc = "Raspberry Pi rev B",
    .init = raspberrypi_init,
};


static void raspberrypi_machine_init(void)
{
    qemu_register_machine(&raspberrypi_machine);
}

machine_init(raspberrypi_machine_init)
