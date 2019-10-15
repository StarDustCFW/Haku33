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
#ifndef _MENU_H_
#define _MENU_H_
#include "soc/hw_init.h"
#include "utils/types.h"
#include "menu/gui/gui_menu_entry.h"
#include "core/custom-gui.h"

#define MAX_ENTRIES 0x80

typedef struct
{
	char title[0x100];
	int next_entry;
	int selected_index;
    custom_gui_t* custom_gui;
	gui_menu_entry_t *entries[MAX_ENTRIES];
} gui_menu_t;

/* Allocate menu to heap */
gui_menu_t *gui_menu_create(const char *title);

/* Add an entry to menu */
void gui_menu_append_entry(gui_menu_t *menu, gui_menu_entry_t *menu_entry);

/* Handle all menu related stuff */
int gui_menu_open(gui_menu_t *menu);
int gui_menu_open2(gui_menu_t *menu);
int gui_menu_open3(gui_menu_t *menu);

/* Deallocate a menu from heap */
void gui_menu_destroy(gui_menu_t *menu);

#endif