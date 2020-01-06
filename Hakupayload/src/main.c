/*
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gfx/di.h"
#include "gfx/gfx.h"
#include "soc/hw_init.h"
#include "mem/heap.h"
#include "soc/fuse.h"

#include "core/launcher.h"

#include "utils/util.h"
#include "utils/fs_utils.h"
#include "utils/touch.h"
#include "utils/btn.h"
#include "utils/dirlist.h"
#include "menu/gui/gui_argon_menu.h"

#include "minerva/minerva.h"

#define PATH_ENABLE_TOUCH "StarDust/touch"

extern void pivot_stack(u32 stack_top);

static inline void setup_gfx()
{
    u32 *fb = display_init_framebuffer();
    gfx_init_ctxt(&g_gfx_ctxt, fb, 1280, 720, 720);
    gfx_clear_buffer(&g_gfx_ctxt);
    gfx_con_init(&g_gfx_con, &g_gfx_ctxt);
    gfx_con_setcol(&g_gfx_con, 0xFFCCCCCC, 1, BLACK);
}

//copy code
void copyarall(char* directory, char* destdir, char* filet, char* coment)
{
char* files = listfil(directory, filet, true);
char* folder = listfol(directory, "*", true);
f_mkdir(destdir);
    u32 i = 0;
    while(files[i * 256])
    {
char* sourcefile = (char*)malloc(256);
			if(strlen(&files[i * 256]) <= 100){			
			strcpy(sourcefile, directory);
			strcat(sourcefile, "/");
			strcat(sourcefile, &files[i * 256]);
			
char* destfile = (char*)malloc(256);
			strcpy(destfile, destdir);
			strcat(destfile, "/");
			strcat(destfile, &files[i * 256]);
			if(strlen(coment) > 0){
				gfx_con_setpos(&g_gfx_con, 1, 10);	
				gfx_printf(&g_gfx_con, "\n %s \n", coment);
				gfx_con_setcol(&g_gfx_con, 0xFF008F39, 0xFF726F68, 0xFF191414);
				gfx_con_setpos(&g_gfx_con, 1, 100);		
				gfx_printf(&g_gfx_con, "\n %s\n", destfile);
				gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
				gfx_swap_buffer(&g_gfx_ctxt);}
			copyfile(sourcefile,destfile);
//			f_unlink(sourcefile);
			}
	i++;
    }

    u32 r = 0;
    while(folder[r * 256])
    {
char* folderpath = (char*)malloc(256);
			if((strlen(&folder[r * 256]) <= 100) & (strlen(&folder[r * 256]) > 0)){			
			strcpy(folderpath, directory);
			strcat(folderpath, "/");
			strcat(folderpath, &folder[r * 256]);
//			deleteall(folderpath, "*","");

char* folderdest = (char*)malloc(256);
			strcpy(folderdest, destdir);
			strcat(folderdest, "/");
			strcat(folderdest, &folder[r * 256]);
//			deleteall(folderpath, "*","");
			copyarall(folderpath, folderdest, filet, coment);
			}
	r++;
    }
}

//folder delete use with care
void deleteall(char* directory, char* filet, char* coment)
{
char* payloads = listfil(directory, filet, true);
char* folder = listfol(directory, "*", true);

    u32 i = 0;
    while(payloads[i * 256])
    {
char* payloadpath = (char*)malloc(256);
			if(strlen(&payloads[i * 256]) <= 100){			
			strcpy(payloadpath, directory);
			strcat(payloadpath, "/");
			strcat(payloadpath, &payloads[i * 256]);
			if(strlen(coment) > 0){
				gfx_con_setpos(&g_gfx_con, 1, 10);	
				gfx_printf(&g_gfx_con, "\n %s \n", coment);
				gfx_con_setcol(&g_gfx_con, 0xFF008F39, 0xFF726F68, 0xFF191414);
				gfx_con_setpos(&g_gfx_con, 1, 100);		
				gfx_printf(&g_gfx_con, "\n %s\n", payloadpath);
				gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
				gfx_swap_buffer(&g_gfx_ctxt);}
			f_unlink(payloadpath);
			}
	i++;
    }

    u32 r = 0;
    while(folder[r * 256])
    {
char* folderpath = (char*)malloc(256);
			if((strlen(&folder[r * 256]) <= 100) & (strlen(&folder[r * 256]) > 0)){			
			strcpy(folderpath, directory);
			strcat(folderpath, "/");
			strcat(folderpath, &folder[r * 256]);
			deleteall(folderpath, "*",coment);
			}
	r++;
    }
f_unlink(directory);
}

//first Delete After Update
void clean_up()
{

//deleteall("/////", "*","");	
	//close
//	f_unlink("/fixer.del");
}

void ipl_main()
{
    /* Configure Switch Hardware (thanks to hekate project) */
    config_hw();

    /* Init the stack and the heap */
    pivot_stack(0x90010000);
    heap_init(0x90020000);

    /* Init display and gfx module */
    display_init();
    setup_gfx();
    display_backlight_pwm_init();
    display_backlight_brightness(20, 1000);


    /* Train DRAM */
    g_gfx_con.mute = 1; /* Silence minerva, comment for debug */
    minerva();
    g_gfx_con.mute = 0;

    /* Cofigure touch input */
    touch_power_on();
    
    /* Mount Sd card and launch payload */
    if (sd_mount())
    {

//Blank ,0xFF00FF22,0xFFCCCCCC verde 
//gfx_render_splash(&g_gfx_ctxt, (u8*)sd_file_read("StarDust/background.bmp"));
		gfx_printf(&g_gfx_con, "%k*%k Haku33 v2.3 Kronos2308, Hard Reset\n\n",0xFF00FF22,0xFFCCCCCC);
		gfx_printf(&g_gfx_con, "%k TU CONSOLA SERA COMPLETAMENTE LIMPADA: SAVES, JUEGOS, ETC\n\n",0xFF828282);
		gfx_printf(&g_gfx_con, "%k SE REALIZARA UN HARD RESET LUEGO SE APAGARA LA CONSOLA\n\n",0xFF828282);
		gfx_printf(&g_gfx_con, "%k SI NO SABES LO QUE HACES, PRESIONA B PARA ABORTAR \n\n",0xFF828282);
		gfx_printf(&g_gfx_con, "%k\n-------- LO DEVORARE TODO --------\n\n",0xFF828282);
		gfx_printf(&g_gfx_con, "%k PULSA %k- + ZR ZL %k PARA LIMPIAR\n\n",0xFF828282,0xFF00FF22,0xFF828282);

		gfx_printf(&g_gfx_con, "%k*%k Kill System Services\n",0xFF00FF22,0xFFCCCCCC);
		gfx_printf(&g_gfx_con, "%k*%k Kill Extra Services\n",0xFF00FF22,0xFFCCCCCC);
		gfx_printf(&g_gfx_con, "%k*%k Terminate Critical Services\n",0xFF00FF22,0xFFCCCCCC);
		gfx_printf(&g_gfx_con, "%k*%k Kill Homebrew Services\n",0xFF00FF22,0xFFCCCCCC);
		gfx_printf(&g_gfx_con, "%k*%k Mount system\n",0xFF00FF22,0xFFCCCCCC);
		gfx_printf(&g_gfx_con, "%k*%k Delete system\n",0xFF00FF22,0xFFCCCCCC);
		gfx_printf(&g_gfx_con, "%k*%k Umount system\n",0xFF00FF22,0xFFCCCCCC);
		gfx_printf(&g_gfx_con, "%k*%k Mount User\n",0xFF00FF22,0xFFCCCCCC);
		gfx_printf(&g_gfx_con, "%k*%k Delete User\n",0xFF00FF22,0xFFCCCCCC);
		gfx_printf(&g_gfx_con, "%k*%k Umount User\n",0xFF00FF22,0xFFCCCCCC);
		gfx_printf(&g_gfx_con, "%k*%k Exit proc\n",0xFF00FF22,0xFFCCCCCC);

//		
//		BootStrapNX();
		f_rename("/Nintendo", "/Hamburgesa_tendo");
		f_rename("/Nintendo", "/Hamburgesa_tendo_1");
		f_rename("/Nintendo", "/Hamburgesa_tendo_2");
		f_rename("/Nintendo", "/Hamburgesa_tendo_3");
		f_rename("/Nintendo", "/Hamburgesa_tendo_4");
		f_rename("/Nintendo", "/Hamburgesa_tendo_5");
		f_rename("/Nintendo", "/Hamburgesa_tendo_6");
		f_rename("/Nintendo", "/Hamburgesa_tendo_7");
		gfx_printf(&g_gfx_con, "%k*%k Nintendo Folder Renamed...\n",0xFF00FF22,0xFFCCCCCC);
		f_unlink("/Haku33.nro");
		f_unlink("/Switch/Haku33.nro");
    } else {
        gfx_printf(&g_gfx_con, "No SD Card Found...\n");
    }

		gfx_printf(&g_gfx_con, "\n\n%k*%k Cleaning finished ...\n",0xFF00FF22,0xFFCCCCCC);
		gfx_printf(&g_gfx_con, "\n\n%k*%k All Green ...\n",0xFF00FF22,0xFFCCCCCC);

gfx_swap_buffer(&g_gfx_ctxt);
btn_wait_timeout(15000, BTN_POWER);
BootStrapNX();

	
	
	
/*If payload launch fails wait for user input to reboot the switch */
    gfx_printf(&g_gfx_con, "Press power button to reboot into RCM...\n\n");
    gfx_swap_buffer(&g_gfx_ctxt);
    wait_for_button_and_reboot();
}

