/*
* Copyright (c) 2018 naehrwert
* Copyright (C) 2018 CTCaer
* Copyright (C) 2018 Guillem96
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

#include "utils/util.h"
#include "utils/btn.h"
#include "utils/fs_utils.h"
#include "soc/t210.h"
#include "soc/fuse.h"
#include "power/max77620.h"
#include "soc/pmc.h"
#include "soc/i2c.h"
#include "panic/panic.h"
#include "gfx/di.h"
#include "gfx/gfx.h"
#include "mem/heap.h"
#include <string.h>
#include "core/launcher.h"
#include "core/payloads.h"
#include "core/custom-gui.h"
#include "power/max17050.h"
u32 get_tmr_s()
{
	return RTC(APBDEV_RTC_SECONDS);
}

u32 get_tmr_ms()
{
	// The registers must be read with the following order:
	// -> RTC_MILLI_SECONDS (0x10) -> RTC_SHADOW_SECONDS (0xC)
	return (RTC(APBDEV_RTC_MILLI_SECONDS) | (RTC(APBDEV_RTC_SHADOW_SECONDS) << 10));
}

u32 get_tmr_us()
{
	return TMR(TIMERUS_CNTR_1US); //TIMERUS_CNTR_1US
}

void msleep(u32 milliseconds)
{
	u32 start = RTC(APBDEV_RTC_MILLI_SECONDS) | (RTC(APBDEV_RTC_SHADOW_SECONDS) << 10);
	while (((RTC(APBDEV_RTC_MILLI_SECONDS) | (RTC(APBDEV_RTC_SHADOW_SECONDS) << 10)) - start) <= milliseconds)
		;
}

void usleep(u32 microseconds)
{
	u32 start = TMR(TIMERUS_CNTR_1US);
	// Casting to u32 is important!
	while ((u32)(TMR(TIMERUS_CNTR_1US) - start) <= microseconds)
		;
}

void exec_cfg(u32 *base, const cfg_op_t *ops, u32 num_ops)
{
	for(u32 i = 0; i < num_ops; i++)
		base[ops[i].off] = ops[i].val;
}

#define CRC32C_POLY 0x82F63B78
u32 crc32c(const void *buf, u32 len)
{
	const u8 *cbuf = (const u8 *)buf;
	u32 crc = 0xFFFFFFFF;
	while (len--)
	{
		crc ^= *cbuf++;
		for (int i = 0; i < 8; i++)
			crc = crc & 1 ? (crc >> 1) ^ CRC32C_POLY : crc >> 1;
	}
	return ~crc;
}

u32 memcmp32sparse(const u32 *buf1, const u32 *buf2, u32 len)
{
	u32 len32 = len / 4;

	if (!(len32 % 32))
	{
		while (len32)
		{
			len32 -= 32;
			if(buf1[len32] != buf2[len32])
				return 1;
		}
	}
	else
	{
		while (len32)
		{
			len32 -= 32;
			if(buf1[len32] != buf2[len32])
				return 1;
			if (len32 < 32)
				return 0;
		}
	}

	return 0;
}

__attribute__((noreturn)) void wait_for_button_and_reboot(void) {
    u32 button;
    while (true) {
        button = btn_read();
        if (button & BTN_POWER) {
            reboot_rcm();
        }
    }
}

void reboot_normal()
{
	sd_unmount();

    gfx_end_ctxt(&g_gfx_ctxt);
	display_end();
	panic(0x21); // Bypass fuse programming in package1.
}

void reboot_rcm()
{
	sd_unmount();
    gfx_end_ctxt(&g_gfx_ctxt);
	display_end();

	PMC(APBDEV_PMC_SCRATCH0) = 2; // Reboot into rcm.
	PMC(APBDEV_PMC_CNTRL) |= PMC_CNTRL_MAIN_RST;
	while (true)
		usleep(1);
}

void power_off()
{
	sd_unmount();
	//TODO: we should probably make sure all regulators are powered off properly.
	i2c_send_byte(I2C_5, MAX77620_I2C_ADDR, MAX77620_REG_ONOFFCNFG1, MAX77620_ONOFFCNFG1_PWR_OFF);
}


char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}


void BootStrapNX()
{
gfx_clear_buffer(&g_gfx_ctxt);
sd_unmount();

	while (true)
	{
		display_backlight_brightness(0, 1000);
		btn_wait();
		
		if (btn_read() & BTN_POWER)
		{
		msleep(500);
		if (btn_read() & BTN_VOL_UP) reboot_rcm();
			power_off();
		}
		
		if (btn_read() & BTN_VOL_UP)
		{
			if (sd_mount())
			{
				g_gfx_con.mute = 0;
				launch_payload("payload.bin");
				sd_unmount();
				display_backlight_brightness(100, 1000);
				gfx_con_setpos(&g_gfx_con, 250, 230);
				gfx_printf(&g_gfx_con, "%kpayload.bin%k missing%k\n",0xFF008F39,0xFFea2f1e,0xFFF9F9F9);
				gfx_swap_buffer(&g_gfx_ctxt);
				btn_wait_timeout(7000, BTN_POWER);
				}else{
				g_gfx_con.mute = 0;
				display_backlight_brightness(100, 1000);
				gfx_con_setpos(&g_gfx_con, 250, 230);
				gfx_printf(&g_gfx_con, "%kSD card Mount failed...%k\n",0xFFea2f1e,0xFFF9F9F9);
				sd_mount();
				gfx_swap_buffer(&g_gfx_ctxt);
				btn_wait_timeout(7000, BTN_POWER);
			}
		}
	}
}