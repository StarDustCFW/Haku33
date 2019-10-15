/*
 * Copyright (c) 2018 langerhans
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

#ifndef _TOUCH_H_
#define _TOUCH_H_
 
#include "soc/i2c.h"
#include "utils/util.h"
#include "utils/types.h"

/* I2C commands */
#define STMFTS_READ_INFO			    0x80
#define STMFTS_READ_STATUS			    0x84
#define STMFTS_READ_ONE_EVENT			0x85
#define STMFTS_READ_ALL_EVENT			0x86
#define STMFTS_LATEST_EVENT			    0x87
#define STMFTS_SLEEP_IN				    0x90
#define STMFTS_SLEEP_OUT			    0x91
#define STMFTS_MS_MT_SENSE_OFF			0x92
#define STMFTS_MS_MT_SENSE_ON			0x93
#define STMFTS_SS_HOVER_SENSE_OFF		0x94
#define STMFTS_SS_HOVER_SENSE_ON		0x95
#define STMFTS_MS_KEY_SENSE_OFF			0x9a
#define STMFTS_MS_KEY_SENSE_ON			0x9b
#define STMFTS_SYSTEM_RESET			    0xa0
#define STMFTS_CLEAR_EVENT_STACK		0xa1
#define STMFTS_FULL_FORCE_CALIBRATION	0xa2
#define STMFTS_MS_CX_TUNING			    0xa3
#define STMFTS_SS_CX_TUNING			    0xa4
#define STMFTS_WRITE_REGISTER			0xb6

/* events */
#define STMFTS_EV_NO_EVENT			    0x00
#define STMFTS_EV_MULTI_TOUCH_DETECTED	0x02
#define STMFTS_EV_MULTI_TOUCH_ENTER		0x03
#define STMFTS_EV_MULTI_TOUCH_LEAVE		0x04
#define STMFTS_EV_MULTI_TOUCH_MOTION	0x05
#define STMFTS_EV_HOVER_ENTER			0x07
#define STMFTS_EV_HOVER_LEAVE			0x08
#define STMFTS_EV_HOVER_MOTION			0x09
#define STMFTS_EV_KEY_STATUS			0x0e
#define STMFTS_EV_ERROR				    0x0f
#define STMFTS_EV_CONTROLLER_READY		0x10
#define STMFTS_EV_SLEEP_OUT_CONTROLLER_READY    0x11
#define STMFTS_EV_STATUS			    0x16
#define STMFTS_EV_DEBUG				    0xdb

/* multi touch related event masks */
#define STMFTS_MASK_EVENT_ID			0x0f
#define STMFTS_MASK_TOUCH_ID			0xf0
#define STMFTS_MASK_LEFT_EVENT			0x0f
#define STMFTS_MASK_X_MSB			    0x0f
#define STMFTS_MASK_Y_LSB			    0xf0

/* key related event masks */
#define STMFTS_MASK_KEY_NO_TOUCH		0x00
#define STMFTS_MASK_KEY_MENU			0x01
#define STMFTS_MASK_KEY_BACK			0x02

#define STMFTS_EVENT_SIZE	    8
#define STMFTS_STACK_DEPTH	    32
#define STMFTS_DATA_MAX_SIZE    (STMFTS_EVENT_SIZE * STMFTS_STACK_DEPTH)
#define STMFTS_MAX_FINGERS	    10
#define STMFTS_DEV_NAME		    "stmfts"

/* TOUCH SUPPORT ONLY WORKS WHEN A GC IS INSIDE NINTENDO SWITCH */


typedef struct {
    u8 raw[4];
    u8 type;
    u16 x;
    u16 y;
} touch_event_t;


/* Init touch support */
int touch_power_on();

/* Wait for touch input */
touch_event_t touch_wait();

/**
 * Checks if touch event is produced inside a rectangle
 * x, y are the top left coordinates of the rect
 */
bool is_rect_touched(touch_event_t* event, u32 x, u32 y, u32 width, u32 height);

#endif /* _TOUCH_H_ */ 
