/*
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

#include <string.h>

#include "libs/fatfs/ff.h"
#include "mem/heap.h"
#include "utils/types.h"
//#include <stdio.h>
#include "utils/fs_utils.h"

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}


char *dirlist(const char *directory, const char *pattern, bool includeHiddenFiles)
{
	u8 max_entries = 61;

	int res = 0;
	u32 i = 0, j = 0, k = 0;
	DIR dir;
	static FILINFO fno;
	
	char *dir_entries = (char *)calloc(max_entries, 256);
	char *copy_entries = (char *)calloc(max_entries, 256);
	char *temp = (char *)calloc(1, 256);

	if (!pattern && !f_opendir(&dir, directory))
	{
		for (;;)
		{
			res = f_readdir(&dir, &fno);
			if (res || !fno.fname[0])
				break;
			if (!(fno.fattrib & AM_DIR) && (fno.fname[0] != '.') && (includeHiddenFiles || !(fno.fattrib & AM_HID)))
			{
				memcpy(dir_entries + (k * 256), fno.fname, strlen(fno.fname) + 1);
				k++;
				if (k > (max_entries - 1))
					break;
			}
		}
		f_closedir(&dir);
	}
	else if (pattern && !f_findfirst(&dir, &fno, directory, pattern) && fno.fname[0])
	{
		do
		{
			if (!(fno.fattrib & AM_DIR) && (fno.fname[0] != '.') && (includeHiddenFiles || !(fno.fattrib & AM_HID)))
			{
				memcpy(dir_entries + (k * 256), fno.fname, strlen(fno.fname) + 1);
				k++;
				if (k > (max_entries - 1))
					break;
			}
			res = f_findnext(&dir, &fno);
		} while (fno.fname[0] && !res);
		f_closedir(&dir);
	}

	if (!k)
	{
		free(temp);
		free(dir_entries);
		free(copy_entries);

		return NULL;
	}

	// make copy_entries lowercase version of dir_entries
	for(i = 0; i < k; i++) 
	{
		j = i * 256;
		while(dir_entries[j]) 
		{
			copy_entries[j] = dir_entries[j];

			if(dir_entries[j] >= 'A' && dir_entries[j] <= 'Z')
				copy_entries[j] += 32;
			
			j++;
		}
		
		copy_entries[j] = '\0';
	}

	// compare copy_entries but sort dir_entries
char *sxos = "sxos";
	char *reinx = "reinx";
	char *atmos = "atmos";

	char *payload1 = "";
	char *payload2 = "";
	char *payload3 = "";


	char buff[20];

/*
	for (i = 0; i < k - 1 ; i++)
	{
		for (j = i + 1; j < k; j++)
		{
			if (strcmp(&copy_entries[i * 256], &copy_entries[j * 256]) > 0) 
			{
				memcpy(temp, &dir_entries[i * 256], strlen(&dir_entries[i * 256]) + 1);
				memcpy(&dir_entries[i * 256], &dir_entries[j * 256], strlen(&dir_entries[j * 256]) + 1);
				memcpy(&dir_entries[j * 256], temp, strlen(temp) + 1);
			}
		}
	}
*/
	char allpayloads[1300];

	strcpy(allpayloads,"Payload Entries: ");
	strcat(allpayloads,itoa(k,buff));

	for (i = 0; i < k ; i++)
	{

	    if(strstr(&copy_entries[i * 256],atmos) != NULL)
	    {
		memcpy(&dir_entries[i * 256], &copy_entries[i * 256], strlen(&copy_entries[i * 256]) + 1);
		payload1=&dir_entries[i * 256];
		strcat(allpayloads,"\nPAYLOAD1: ");
		strcat(allpayloads,payload1);
	    }
	    if(strstr(&copy_entries[i * 256],reinx) != NULL)
	    {
		memcpy(&dir_entries[i * 256], &copy_entries[i * 256], strlen(&copy_entries[i * 256]) + 1);
		payload2=&dir_entries[i * 256];
		strcat(allpayloads,"\nPAYLOAD2: ");
		strcat(allpayloads,payload2);
	    }
	    if(strstr(&copy_entries[i * 256],sxos) != NULL)
	    {
		memcpy(&dir_entries[i * 256], &copy_entries[i * 256], strlen(&copy_entries[i * 256]) + 1);
		payload3=&dir_entries[i * 256];
		strcat(allpayloads,"\nPAYLOAD3: ");
		strcat(allpayloads,payload3);
	    }
	}
	free(temp);
	free(copy_entries);

	return dir_entries;
}
//------------------------------------------------------------------------//
	
	
char *listfol(const char *directory, const char *pattern, bool includeHiddenFiles)
{
	u8 max_entries = 61;

	int res = 0;
	u32 i = 0, j = 0, k = 0;
	DIR dir;
	static FILINFO fno;
	
	char *dir_entries = (char *)calloc(max_entries, 256);
	char *copy_entries = (char *)calloc(max_entries, 256);
	char *temp = (char *)calloc(1, 256);

	if (!pattern && !f_opendir(&dir, directory))
	{
		for (;;)
		{
			res = f_readdir(&dir, &fno);
			if (res || !fno.fname[0])
				break;
			if (!(fno.fattrib & AM_DIR) && (fno.fname[0] != ':') && (includeHiddenFiles || !(fno.fattrib & AM_HID)))
			{
				memcpy(dir_entries + (k * 256), fno.fname, strlen(fno.fname) + 1);
				k++;
				if (k > (max_entries - 1))
					break;
			}
		}
		f_closedir(&dir);
	}
	else if (pattern && !f_findfirst(&dir, &fno, directory, pattern) && fno.fname[0])
	{
		do
		{
			if ((fno.fattrib & AM_DIR) && (fno.fname[0] != ':') && (includeHiddenFiles || !(fno.fattrib & AM_HID)))
			{
				memcpy(dir_entries + (k * 256), fno.fname, strlen(fno.fname) + 1);
				k++;
				if (k > (max_entries - 1))
					break;
			}
			res = f_findnext(&dir, &fno);
		} while (fno.fname[0] && !res);
		f_closedir(&dir);
	}

	if (!k)
	{
		free(temp);
		free(dir_entries);
		free(copy_entries);

		return NULL;
	}

	// make copy_entries lowercase version of dir_entries
	for(i = 0; i < k; i++) 
	{
		j = i * 256;
		while(dir_entries[j]) 
		{
			copy_entries[j] = dir_entries[j];

			if(dir_entries[j] >= 'A' && dir_entries[j] <= 'Z')
				copy_entries[j] += 32;
			
			j++;
		}
		
		copy_entries[j] = '\0';
	}

	// compare copy_entries but sort dir_entries
	for (i = 0; i < k - 1 ; i++)
	{
		for (j = i + 1; j < k; j++)
		{
			if (strcmp(&copy_entries[i * 256], &copy_entries[j * 256]) > 0) 
			{
				memcpy(temp, &dir_entries[i * 256], strlen(&dir_entries[i * 256]) + 1);
				memcpy(&dir_entries[i * 256], &dir_entries[j * 256], strlen(&dir_entries[j * 256]) + 1);
				memcpy(&dir_entries[j * 256], temp, strlen(temp) + 1);
			}
		}
	}

	free(temp);
	free(copy_entries);

	return dir_entries;
	}

char *listfil(const char *directory, const char *pattern, bool includeHiddenFiles)
{
	u8 max_entries = 61;

	int res = 0;
	u32 i = 0, j = 0, k = 0;
	DIR dir;
	static FILINFO fno;
	
	char *dir_entries = (char *)calloc(max_entries, 256);
	char *copy_entries = (char *)calloc(max_entries, 256);
	char *temp = (char *)calloc(1, 256);

	if (!pattern && !f_opendir(&dir, directory))
	{
		for (;;)
		{
			res = f_readdir(&dir, &fno);
			if (res || !fno.fname[0])
				break;
			if (!(fno.fattrib & AM_DIR) && (fno.fname[0] != ':') && (includeHiddenFiles || !(fno.fattrib & AM_HID)))
			{
				memcpy(dir_entries + (k * 256), fno.fname, strlen(fno.fname) + 1);
				k++;
				if (k > (max_entries - 1))
					break;
			}
		}
		f_closedir(&dir);
	}
	else if (pattern && !f_findfirst(&dir, &fno, directory, pattern) && fno.fname[0])
	{
		do
		{
			if (!(fno.fattrib & AM_DIR) && (fno.fname[0] != ':') && (includeHiddenFiles || !(fno.fattrib & AM_HID)))
			{
				memcpy(dir_entries + (k * 256), fno.fname, strlen(fno.fname) + 1);
				k++;
				if (k > (max_entries - 1))
					break;
			}
			res = f_findnext(&dir, &fno);
		} while (fno.fname[0] && !res);
		f_closedir(&dir);
	}

	if (!k)
	{
		free(temp);
		free(dir_entries);
		free(copy_entries);

		return NULL;
	}

	// make copy_entries lowercase version of dir_entries
	for(i = 0; i < k; i++) 
	{
		j = i * 256;
		while(dir_entries[j]) 
		{
			copy_entries[j] = dir_entries[j];

			if(dir_entries[j] >= 'A' && dir_entries[j] <= 'Z')
				copy_entries[j] += 32;
			
			j++;
		}
		
		copy_entries[j] = '\0';
	}

	// compare copy_entries but sort dir_entries
	for (i = 0; i < k - 1 ; i++)
	{
		for (j = i + 1; j < k; j++)
		{
			if (strcmp(&copy_entries[i * 256], &copy_entries[j * 256]) > 0) 
			{
				memcpy(temp, &dir_entries[i * 256], strlen(&dir_entries[i * 256]) + 1);
				memcpy(&dir_entries[i * 256], &dir_entries[j * 256], strlen(&dir_entries[j * 256]) + 1);
				memcpy(&dir_entries[j * 256], temp, strlen(temp) + 1);
			}
		}
	}

	free(temp);
	free(copy_entries);

	return dir_entries;
	}