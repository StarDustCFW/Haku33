/*
 * Copyright (c) 2018 Guillem96
 * Copyright (c) 2018 CTCaer
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
#include "core/launcher.h"
#include "gfx/di.h"
#include "gfx/gfx.h"
#include <string.h>
#include "soc/hw_init.h"
#include "libs/fatfs/ff.h"
#include "utils/types.h"
#include "utils/util.h"
#include "utils/fs_utils.h"
#include "gfx/gfx.h"
#include "soc/hw_init.h"
#include "mem/heap.h"
#include "menu/gui/gui_menu_pool.h"

// This is a safe and unused DRAM region for our payloads.
#define IPL_LOAD_ADDR      0x40008000
#define EXT_PAYLOAD_ADDR   0xC03C0000
#define PATCHED_RELOC_SZ   0x94
#define RCM_PAYLOAD_ADDR   (EXT_PAYLOAD_ADDR + ALIGN(PATCHED_RELOC_SZ, 0x10))
#define PAYLOAD_ENTRY      0x40010000
#define CBFS_SDRAM_EN_ADDR 0x4003e000
#define COREBOOT_ADDR      (0xD0000000 - 0x100000)

void (*ext_payload_ptr)() = (void *)EXT_PAYLOAD_ADDR;

void reloc_patcher(u32 payload_size)
{
	static const u32 START_OFF = 0x7C;
	static const u32 PAYLOAD_END_OFF = 0x84;
	static const u32 IPL_START_OFF = 0x88;

	memcpy((u8 *)EXT_PAYLOAD_ADDR, (u8 *)IPL_LOAD_ADDR, PATCHED_RELOC_SZ);

	*(vu32 *)(EXT_PAYLOAD_ADDR + START_OFF) = PAYLOAD_ENTRY - ALIGN(PATCHED_RELOC_SZ, 0x10);
	*(vu32 *)(EXT_PAYLOAD_ADDR + PAYLOAD_END_OFF) = PAYLOAD_ENTRY + payload_size;
	*(vu32 *)(EXT_PAYLOAD_ADDR + IPL_START_OFF) = PAYLOAD_ENTRY;

	if (payload_size == 0x7000)
	{
		memcpy((u8 *)(EXT_PAYLOAD_ADDR + ALIGN(PATCHED_RELOC_SZ, 0x10)), (u8 *)COREBOOT_ADDR, 0x7000); //Bootblock
		*(vu32 *)CBFS_SDRAM_EN_ADDR = 0x4452414D;
	}
}

int launch_payload(char *path)
{
	if (!sd_mount()){BootStrapNX();}//check sd

    u32 boot = 0;

    if(strstr(path,"atmos") != NULL)
    	boot=1;
    if(strstr(path,"reinx") != NULL)
    	boot=2;
    if(strstr(path,"sxos") != NULL)
    	boot=3;
		
    if(strstr(path,"Atmos") != NULL)
    	boot=1;
    if(strstr(path,"ReiNX") != NULL)
    	boot=2;
    if(strstr(path,"SXOS") != NULL)
    	boot=3;
    if(strstr(path,"coreboot") != NULL)
    	boot=4;

/*
	u8* buffer = (u8*)malloc(1);
    memcpy(buffer, &boot, 1);
    sd_save_to_file(buffer, 1, "StarDust/boot.txt");
*/
    if(boot==1)
    {
		display_backlight_brightness(0, 1000);
		copyfile("atmosphere/fusee-secondary.bin","sept/payload.bin");
		if (sd_file_exists ("StarDust/autobootecho.txt"))
		sd_save_to_file("Atmosphere", 10, "StarDust/autobootecho.txt");
    }

    if(boot==2)
    {
		display_backlight_brightness(0, 1000);
		copyfile("sept/reinx.bin","sept/payload.bin");
		if (sd_file_exists ("StarDust/autobootecho.txt"))
		sd_save_to_file("ReiNX", 5, "StarDust/autobootecho.txt");
    }

	if(boot==3)
    {
		display_backlight_brightness(0, 1000);
		u32 bootR = sd_file_size("boot.dat");
		u32 bootS = sd_file_size("StarDust/boot.dat");
		if (bootR != bootS)
		copyfile("StarDust/boot.dat","boot.dat");//
		if (sd_file_exists ("StarDust/autobootecho.txt"))
		sd_save_to_file("SXOS", 4, "StarDust/autobootecho.txt");
	}

	if(boot==4)
    {
	display_backlight_brightness(0, 1000);
		if (!sd_file_exists ("/switchroot_android/coreboot.bin"))
		{
		copyfile("switchroot_android/coreboot.rom","switchroot_android/coreboot.bin");
		copyfile("atmosphere/reboot_payload.bin","switchroot_android/coreboot.rom");
		}
    }
	

    FIL fp;
    if (f_open(&fp, path, FA_READ))
    {
        //gfx_printf(&g_gfx_con, "Cannot find %s\n", path);
        return 1;
    }

    // Read and copy the payload to our chosen address
    void *buf;
    u32 size = f_size(&fp);

    if (size < 0x30000)
        buf = (void *)RCM_PAYLOAD_ADDR;
    else
        buf = (void *)COREBOOT_ADDR;

    if (f_read(&fp, buf, size, NULL))
    {
        f_close(&fp);
		display_backlight_brightness(100, 1000);
        gfx_printf(&g_gfx_con, "Error loading %s\n", path);
        return 1;
    }

    f_close(&fp);	
    free(path);
    path = NULL;

    sd_unmount();

    if (size < 0x30000)
    {
        reloc_patcher(ALIGN(size, 0x10));
        reconfig_hw_workaround(false, byte_swap_32(*(u32 *)(buf + size - sizeof(u32))));
    }
    else
    {
        reloc_patcher(0x7000);
        if (*(vu32 *)CBFS_SDRAM_EN_ADDR != 0x4452414D)
            return 1;
        reconfig_hw_workaround(true, 0);
    }

    gui_menu_pool_cleanup();
    gfx_end_ctxt(&g_gfx_ctxt);
    
    // Launch our payload.
    (*ext_payload_ptr)();
display_backlight_brightness(100, 1000);
	return 1;
}
