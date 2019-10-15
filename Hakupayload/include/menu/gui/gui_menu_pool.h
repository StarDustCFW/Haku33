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
#ifndef _GUI_MENU_POOL_H_
#define _GUI_MENU_POOL_H_

#include "menu/gui/gui_menu.h"

typedef struct {
    int max_items;
    int current_items;
    gui_menu_t** menus;
} gui_menu_pool_t;

gui_menu_pool_t* g_menu_pool;

/* Initializes the pool */
void gui_menu_pool_init();

/* Add a menu and its entries to the pool */
void gui_menu_push_to_pool(gui_menu_t * menu);

/* Deallocate all entries and menus */
void gui_menu_pool_cleanup();

#endif