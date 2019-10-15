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
#ifndef _PAYLOADS_H_
#define _PAYLOADS_H_

#include "utils/types.h"

#define PAYBACK_DIR "StarDust/payback"

#define PAYLOADS_DIR "StarDust/payloads"
#define PAYLOADS_LOGOS_DIR "StarDust/logos"

/* Generate full pyload directory */
void payload_full_path(const char*, char*);
void payload_full_back(const char*, char*);

/* Get payload's logo from payload's name */
void payload_logo_path(const char*, char*);


#endif