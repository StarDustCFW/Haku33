#include <iostream>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <switch.h>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <memory>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iterator>
#include <dirent.h>
#include <cstdio>
#include <filesystem>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <vector>
#include <stdlib.h>
#include "FileSystem.hpp"
#include "lang.hpp"
extern "C" {
#include "reboot.h"

}

using namespace std;
//Vars
	//ask to the switch for the serial detect incognito

	//Power on led
	bool led_on(void)
	{
	Result rc=0;
		s32 i;
		s32 total_entries;
		u64 UniquePadIds[2];
		HidsysNotificationLedPattern pattern;
		hidsysExit();
		rc = hidsysInitialize();
		if (R_FAILED(rc)) {
			printf("hidsysInitialize(): 0x%x\n", rc);
		}


		memset(&pattern, 0, sizeof(pattern));
		// Setup Breathing effect pattern data.
		pattern.baseMiniCycleDuration = 0x8;             // 100ms.
		pattern.totalMiniCycles = 0x2;                   // 3 mini cycles. Last one 12.5ms.
		pattern.totalFullCycles = 0x0;                   // Repeat forever.
		pattern.startIntensity = 0x2;                    // 13%.

		pattern.miniCycles[0].ledIntensity = 0xF;        // 100%.
		pattern.miniCycles[0].transitionSteps = 0xF;     // 15 steps. Transition time 1.5s.
		pattern.miniCycles[0].finalStepDuration = 0x0;   // Forced 12.5ms.
		pattern.miniCycles[1].ledIntensity = 0x2;        // 13%.
		pattern.miniCycles[1].transitionSteps = 0xF;     // 15 steps. Transition time 1.5s.
		pattern.miniCycles[1].finalStepDuration = 0x0;   // Forced 12.5ms.
			
		rc = hidsysGetUniquePadsFromNpad(hidGetHandheldMode() ? CONTROLLER_HANDHELD : CONTROLLER_PLAYER_1, UniquePadIds, 2, &total_entries);

		if (R_SUCCEEDED(rc)) {
			for(i=0; i<total_entries; i++) { // System will skip sending the subcommand to controllers where this isn't available.
				rc = hidsysSetNotificationLedPattern(&pattern, UniquePadIds[i]);
			}
		}
		hidsysExit();
	return 0;
	}


int main(int argc, char **argv)
{
	set_LANG();
	romfsInit();
	consoleInit(NULL);
	
	//keys
	while (appletMainLoop())
	{
	    hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);

		u32 minus = 0;
		u32 more = 0;
		u32 LT = 0;
		u32 RT = 0;
	
		if (kHeld & KEY_ZL)
			LT = 4;
		if (kHeld & KEY_ZR)
			RT = 4;
		if (kHeld & KEY_MINUS)
			minus = 4;
		if (kHeld & KEY_PLUS)
			more = 4;

		if (kHeld & KEY_ZL && kHeld & KEY_ZR && kHeld & KEY_MINUS && kHeld & KEY_PLUS)
		{
			minus = 2;
			more = 2;
			LT = 2;
			RT = 2;
		}

		//main menu
		consoleClear();
			printf("\x1b[32;1m*\x1b[0m %s v%s Kronos2308, Hard Reset \n\n",TITLE, VERSION);
			
//			if (isEMU) printf(LG.text11);
					printf(LG.text1);
					printf(LG.text2);
					printf(LG.text3);
					printf(LG.text4);
					printf(LG.text5,LT,minus,more,RT);


		consoleUpdate(NULL);
		
		//call clean after combo
		if (kHeld & KEY_ZL && kHeld & KEY_ZR && kHeld & KEY_MINUS && kHeld & KEY_PLUS)
		{
			fs::copyFile("romfs:/Haku33.te", "/Haku33.te");
			led_on();
			bpcInitialize();
			if(init_slp())
			{reboot_to_payload();}else{bpcShutdownSystem();}
			bpcExit();
			break;
		}
		
		//exit
		if (kDown & KEY_A || kDown & KEY_B || kDown & KEY_Y || kDown & KEY_X)
		{
			break;
		}
	}

	//cansel
	fsExit();
    socketExit();
    fsdevUnmountAll();
	pmshellExit();
	consoleExit(NULL);
	return 0;
}