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
#include "led.hpp"
#include "lang.hpp"
extern "C" {
#include "reboot.h"

}

using namespace std;
void copy_me(string origen, string destino) {
	ifstream source(origen, ios::binary);
	ofstream dest(destino, ios::binary);
	dest << source.rdbuf();
	source.close();
	dest.close();
}

int main(int argc, char **argv)
{
	set_LANG();
	romfsInit();
	consoleInit(NULL);
	PadState pad;
	padConfigureInput(8, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
	
	//keys
	while (appletMainLoop())
	{
	    //hidScanInput();
		padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        u64 kHeld = padGetButtons(&pad);

		
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
			
					printf(LG.text1);
					printf(LG.text2);
					printf(LG.text3);
					printf(LG.text4);
					printf(LG.text5,LT,minus,more,RT);


		consoleUpdate(NULL);
		
		//call clean after combo
		if (kHeld & KEY_ZL && kHeld & KEY_ZR && kHeld & KEY_MINUS && kHeld & KEY_PLUS)
		{
			copy_me("romfs:/startup.te", "/startup.te");
			copy_me("romfs:/poweroff.bin", "/poweroff.bin");
			led_on(1);
			bpcInitialize();
			if(init_slp())
			{reboot_to_payload();}else{break;}
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