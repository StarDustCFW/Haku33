/*
 * Copyright (c) 2018 naehrwert
 * Copyright (C) 2018 CTCaer
 * Copyright (C) 2018 M4xw
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

#ifndef _GFX_H_
#define _GFX_H_

#include "utils/types.h"

#define RED 0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE 0xFFFF0000
#define YELLOW 0xFF00FFFF
#define ORANGE 0xFF3891FF
#define WHITE 0xFFFFFFFF
#define BLACK 0xFF000000

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

typedef struct _gfx_ctxt_t
{
	u32 *fb;
    u32* next;
	u32 width;
	u32 height;
	u32 stride;
} gfx_ctxt_t;

typedef struct _gfx_con_t
{
	gfx_ctxt_t *gfx_ctxt;
    u8 scale;
	u32 x;
	u32 y;
	u32 savedx;
	u32 savedy;
	u32 fgcol;
	int fillbg;
	u32 bgcol;
	bool mute;
} gfx_con_t;

typedef struct 
{
    u32 size;
    u32 size_x;
    u32 size_y;
    u32 offset;
    u32 pos_x;
    u32 pos_y;
} bmp_data_t;

gfx_ctxt_t g_gfx_ctxt;
gfx_con_t g_gfx_con;

#define EPRINTF(text) gfx_printf(&g_gfx_con, "%k"text"%k\n", 0xFFFF0000, 0xFFCCCCCC)
#define EPRINTFARGS(text, args...) gfx_printf(&g_gfx_con, "%k"text"%k\n", 0xFFFF0000, args, 0xFFCCCCCC)
#define WPRINTF(text) gfx_printf(&g_gfx_con, "%k"text"%k\n", 0xFFFFDD00, 0xFFCCCCCC)
#define WPRINTFARGS(text, args...) gfx_printf(&g_gfx_con, "%k"text"%k\n", 0xFFFFDD00, args, 0xFFCCCCCC)

void gfx_init_ctxt(gfx_ctxt_t *ctxt, u32 *fb, u32 width, u32 height, u32 stride);
void gfx_end_ctxt(gfx_ctxt_t *ctxt);
void gfx_clear_buffer(gfx_ctxt_t *ctxt);
void gfx_swap_buffer(gfx_ctxt_t *ctxt);
void gfx_clear_grey(gfx_ctxt_t *ctxt, u8 color);
void gfx_clear_partial_grey(gfx_ctxt_t *ctxt, u8 color, u32 pos_x, u32 height);
void gfx_clear_color(gfx_ctxt_t *ctxt, u32 color);
void gfx_con_init(gfx_con_t *con, gfx_ctxt_t *ctxt);
void gfx_con_setcol(gfx_con_t *con, u32 fgcol, int fillbg, u32 bgcol);
void gfx_con_getpos(gfx_con_t *con, u32 *x, u32 *y);
void gfx_con_setpos(gfx_con_t *con, u32 x, u32 y);
void gfx_putc(gfx_con_t *con, char c);
void gfx_puts(gfx_con_t *con, const char *s);
void gfx_printf(gfx_con_t *con, const char *fmt, ...);
void gfx_hexdump(gfx_con_t *con, u32 base, const u8 *buf, u32 len);

void gfx_set_pixel(gfx_ctxt_t *ctxt, u32 x, u32 y, u32 color);
void gfx_line(gfx_ctxt_t *ctxt, int x0, int y0, int x1, int y1, u32 color);
void gfx_put_small_sep(gfx_con_t *con);
void gfx_put_big_sep(gfx_con_t *con);
void gfx_set_rect_grey(gfx_ctxt_t *ctxt, const u8 *buf, u32 size_x, u32 size_y, u32 pos_x, u32 pos_y);
void gfx_set_rect_rgb(gfx_ctxt_t *ctxt, const u8 *buf, u32 size_x, u32 size_y, u32 pos_x, u32 pos_y);
void gfx_set_rect_argb(gfx_ctxt_t *ctxt, const u32 *buf, u32 size_x, u32 size_y, u32 pos_x, u32 pos_y);
void gfx_render_bmp_argb(gfx_ctxt_t *ctxt, const u32 *buf, u32 size_x, u32 size_y, u32 pos_x, u32 pos_y);
void gfx_render_bmp_argb_transparent(gfx_ctxt_t *ctxt, const u32 *buf, u32 size_x, u32 size_y, u32 pos_x, u32 pos_y, u32 transparent_color);
void gfx_render_bmp_arg_bitmap(gfx_ctxt_t *ctxt, u8* bitmap, u32 x, u32 y, u32 width, u32 height);
void gfx_render_bmp_arg_bitmap_transparent(gfx_ctxt_t *ctxt, u8* bitmap, u32 x, u32 y, u32 width, u32 height, u32 transparent_color);
void gfx_render_bmp_arg_file(gfx_ctxt_t *ctxt, char *path, u32 x, u32 y, u32 width, u32 height);
void gfx_render_splash(gfx_ctxt_t *ctxt, u8 *bitmap);

#endif
