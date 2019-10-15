/*
 * Copyright (c) 2018 Atmosphère-NX
 * Copyright (c) 2018 Guillem96
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "panic/panic.h"
#include "gfx/di.h"
#include "soc/pmc.h"
#include "soc/fuse.h"
#include "utils/util.h"
#include "soc/t210.h"

static uint32_t g_panic_code = 0;

#define BIT(n) (1u << (n))
#define BITL(n)     (1ull << (n))
#define MASK(n) (BIT(n) - 1)

void check_and_display_panic(void) {
    /* We also handle our own panics. */
    /* In the case of our own panics, we assume that the display has already been initialized. */
    bool has_panic = PMC(APBDEV_PMC_RST_STATUS) != 0 || g_panic_code != 0;
    uint32_t code = g_panic_code == 0 ? PMC(APBDEV_PMC_SCRATCH200) : g_panic_code;

    has_panic = has_panic && !(PMC(APBDEV_PMC_RST_STATUS) != 1 && code == PANIC_CODE_SAFEMODE);

    if (has_panic) {
        uint32_t color;

        /* Check for predefined codes: */
        switch (code & MASK(20)) {
            case 0x01: /* Package2 signature verification failed. */
            case 0x02: /* Package2 meta verification failed. */
            case 0x03: /* Package2 version check failed. */
            case 0x04: /* Package2 payload verification failed. */
                color = PANIC_COLOR_KERNEL;
                break;
            case 0x05: /* Unknown SMC. */
            case 0x06: /* Unknown Abort. */
                color = PANIC_COLOR_SECMON_GENERIC;
                break;
            case 0x07: /* Invalid CPU context. */
            case 0x08: /* Invalid SE state. */
            case 0x09: /* CPU is already awake (2.0.0+). */
                color = PANIC_COLOR_SECMON_DEEPSLEEP;
                break;
            case 0x10: /* Unknown exception. */
                color = PANIC_COLOR_SECMON_EXCEPTION;
                break;
            case 0x30: /* General bootloader error. */
            case 0x31: /* Invalid DRAM ID. */
            case 0x32: /* Invalid size. */
            case 0x33: /* Invalid arguement. */
            case 0x34: /* Bad GPT. */
            case 0x35: /* Failed to boot SafeMode. */
            case 0x36: /* Activity monitor fired (4.0.0+). */
                color = PANIC_COLOR_BOOTLOADER_GENERIC;
                break;
            case 0x40: /* Kernel panic. */
                color = PANIC_COLOR_KERNEL;
                break;
            default:
                color = code >> 20;
                color |= color << 4;
                break;
        }

        if (g_panic_code == 0) {
            display_init();
        }

        display_color_screen(color);
        wait_for_button_and_reboot();
    } else {
        g_panic_code = 0;
        PMC(APBDEV_PMC_SCRATCH200) = 0;
    }
}

void panic(u32 code) {
    /* Set panic code. */
    if (g_panic_code == 0) {
        g_panic_code = code;
        PMC(APBDEV_PMC_SCRATCH200) = code;
    }

    fuse_disable_program();
    PMC(APBDEV_PMC_CRYPTO_OP) = 1; /* Disable all SE operations. */

    check_and_display_panic();
    while(true);
}