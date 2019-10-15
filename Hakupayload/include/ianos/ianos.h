/*
 * Copyright (c) 2018 M4xw
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

#ifndef IANOS_H
#define IANOS_H

#include <stddef.h>

#include "utils/types.h"
#include "mem/heap.h"
#include "gfx/gfx.h"

typedef enum
{
	DRAM_LIB = 0, // DRAM library.
	EXEC_ELF = 1, // Executable elf that does not return.
	DR64_LIB = 2, // AARCH64 DRAM library.
	AR64_ELF = 3, // Executable elf that does not return.
	KEEP_IN_RAM = (1 << 31)  // Shared library mask.
} elfType_t;

// Module Callback
typedef void (*cbMainModule_t)(const char *s);
typedef void (*memcpy_t)(void *, void *, size_t);
typedef void (*memset_t)(void *, int, size_t);

typedef struct _bdkParams_t
{
	gfx_con_t *gfxCon;
	gfx_ctxt_t *gfxCtx;
	heap_t *sharedHeap;
	memcpy_t memcpy;
	memset_t memset;
} *bdkParams_t;

// Module Entrypoint
typedef void (*moduleEntrypoint_t)(void *, bdkParams_t);

int ianos_loader(bool sdmount, char *path, elfType_t type, void* config);

#endif