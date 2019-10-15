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
#include "menu/gui/gui_menu_pool.h"
#include "mem/heap.h"

void gui_menu_pool_init()
{
    g_menu_pool = (gui_menu_pool_t *)malloc(sizeof(gui_menu_pool_t));
    g_menu_pool->max_items = 0x16;
    g_menu_pool->current_items = 0;
    g_menu_pool->menus = (gui_menu_t **)malloc(sizeof(gui_menu_t *) * g_menu_pool->max_items);
}

void gui_menu_push_to_pool(gui_menu_t *menu)
{
    if (menu != NULL)
    {
        if (g_menu_pool->current_items == g_menu_pool->max_items - 1)
        {
            // Resize the pool
            u32 new_size = g_menu_pool->max_items << 1;
            g_menu_pool->menus = (gui_menu_t **)m_realloc(g_menu_pool->menus, sizeof(gui_menu_t *) * g_menu_pool->max_items, new_size);
            g_menu_pool->max_items = new_size;
        }
        g_menu_pool->menus[g_menu_pool->current_items] = menu;
        g_menu_pool->current_items++;
    }
}

void gui_menu_pool_cleanup()
{
    for (int i = 0; i < g_menu_pool->current_items; ++i)
        gui_menu_destroy(g_menu_pool->menus[i]);
    free(g_menu_pool->menus);
    free(g_menu_pool);
}