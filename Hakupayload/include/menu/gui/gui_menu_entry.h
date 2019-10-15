/*  
 * Copyright (c) 2018 Guillem96
 *
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
#ifndef _GFX_MENU_ENTRY_H
#define _GFX_MENU_ENTRY_H

#include "utils/types.h"

typedef struct
{
	char text[0x100];
    u32 x;
    u32 y;
    u32 width;
    u32 height;
    u8* bitmap;
	void *param;
	int (*handler)(void *);
} gui_menu_entry_t;

/* Creates a menu entry */
gui_menu_entry_t *gui_create_menu_entry(const char *text, 
                                        u8* bitmap, 
                                        u32 x, u32 y, 
                                        u32 width, u32 height, 
                                        int (*handler)(void *), void *param);

gui_menu_entry_t *gui_create_menu_entry_no_bitmap(const char *text, 
                                                    u32 x, u32 y, 
                                                    u32 width, u32 height, 
                                                    int (*handler)(void *), void *param);
/* Renders a gfx menu entry */
void gui_menu_render_entry(gui_menu_entry_t*);

/* Destroy menu entry */
void gui_menu_entry_destroy(gui_menu_entry_t*);

#endif
