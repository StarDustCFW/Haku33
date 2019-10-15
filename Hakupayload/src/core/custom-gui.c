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

#include "core/custom-gui.h"
#include "gfx/gfx.h"
#include "mem/heap.h"
#include "utils/fs_utils.h"
#include "utils/util.h"
#include <string.h>


custom_gui_t* custom_gui_load()
{
    custom_gui_t* custom_gui = (custom_gui_t*)malloc(sizeof(custom_gui_t));
    custom_gui->custom_bg = (u8*)sd_file_read(CUSTOM_BG_PATH);
    custom_gui->title_bmp = (u8*)sd_file_read(CUSTOM_TITLE_PATH);
    return custom_gui;
}

void custom_gui_end(custom_gui_t* cg)
{
    free(cg->custom_bg);
    free(cg->title_bmp);
    free(cg);
}

bool render_custom_background(custom_gui_t* cg)
{
    if (cg->custom_bg == NULL)
        return false;
    
    gfx_render_splash(&g_gfx_ctxt, (u8*)sd_file_read("StarDust/background.bmp"));
    return true;
}

bool render_custom_title(custom_gui_t* cg)
{  
    if (cg->title_bmp == NULL)
        return false;

    u32 bmp_width = (cg->title_bmp[0x12] | (cg->title_bmp[0x13] << 8) | (cg->title_bmp[0x14] << 16) | (cg->title_bmp[0x15] << 24));
    u32 bmp_height = (cg->title_bmp[0x16] | (cg->title_bmp[0x17] << 8) | (cg->title_bmp[0x18] << 16) | (cg->title_bmp[0x19] << 24));
    gfx_render_bmp_arg_bitmap(&g_gfx_ctxt, cg->title_bmp, 420, 10, bmp_width, bmp_height);
    return true;
}

int screenshot(void* params)
{
if (!sd_mount()){BootStrapNX();}//check sd
    //width, height, and bitcount are the key factors:
    s32 width = 720;
    s32 height = 1280;
    u16 bitcount = 32;//<- 24-bit bitmap

    //take padding in to account
    int width_in_bytes = ((width * bitcount + 31) / 32) * 4;

    //total image size in bytes, not including header
    u32 imagesize = width_in_bytes * height;

    //this value is always 40, it's the sizeof(BITMAPINFOHEADER)
    const u32 bi_size = 40;

    //bitmap bits start after headerfile, 
    //this is sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
    const u32 buf_offset_bits = 54; 

    //total file size:
    u32 filesize = 54 + imagesize;

    //number of planes is usually 1
    const u16 bi_planes = 1;

    //create header:
    //copy to buffer instead of BITMAPFILEHEADER and BITMAPINFOHEADER
    //to avoid problems with structure packing
    unsigned char header[54] = { 0 };
	char* namef = "";
    memcpy(header, "BM", 2);
    memcpy(header + 2 , &filesize, 4);
    memcpy(header + 10, &buf_offset_bits, 4);
    memcpy(header + 14, &bi_size, 4);
    memcpy(header + 18, &width, 4);
    memcpy(header + 22, &height, 4);
    memcpy(header + 26, &bi_planes, 2);
    memcpy(header + 28, &bitcount, 2);
    memcpy(header + 34, &imagesize, 4);
		if (!sd_file_exists("StarDust/screenshot-1.bmp")){namef = "-1";
			}else{
			if (!sd_file_exists("StarDust/screenshot-2.bmp")){namef = "-2";
				}else{
				if (!sd_file_exists("StarDust/screenshot-3.bmp")){namef = "-3";
					}else{
					if (!sd_file_exists("StarDust/screenshot-4.bmp")){namef = "-4";
						}else{
						if (!sd_file_exists("StarDust/screenshot-5.bmp")){namef = "-5";
							}else{
							if (!sd_file_exists("StarDust/screenshot-6.bmp")){namef = "-6";
								}else{
								if (!sd_file_exists("StarDust/screenshot-7.bmp")){namef = "-7";
									}else{gfx_printf(&g_gfx_con, " Screenshot limit");}
								}
							}
						}
					}
				}
			}
	char tmp[256];
    strcpy(tmp, "StarDust/screenshot");
    strcat(tmp, namef);
    strcat(tmp, ".bmp");
    u8* buff = (u8*)malloc(imagesize + 54);
    memcpy(buff, header, 54);
    memcpy(buff + 54, g_gfx_ctxt.fb, imagesize);
    sd_save_to_file(buff, imagesize + 54, tmp);
    free(buff);

    g_gfx_con.scale = 2;
    gfx_con_setpos(&g_gfx_con, 0, 605);
    gfx_printf(&g_gfx_con, " Screenshot saved!\n Find it at %s",tmp);
    return 0;
}
