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
#include "spl.hpp"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "spl.hpp"

#define IRAM_PAYLOAD_MAX_SIZE 0x24000
static u8 g_reboot_payload[IRAM_PAYLOAD_MAX_SIZE];
char Logs[2024];
bool isXSOS;

extern "C" {
#include "reboot.h"
}

using namespace std;
Result Init_Services(void)
{
	Result ret = 0;
	if (R_FAILED(ret = splInitialize())) {return 1;}
	if (R_FAILED(ret = hiddbgInitialize())) {return 1;}
	if (R_FAILED(ret = psmInitialize())) {return 1;}
	return ret;
}

void close_Services()
{
	setsysExit();
	splExit();
	smExit();
	hiddbgExit();
	psmExit();
}

void copy_me(string origen, string destino) {
	ifstream source(origen, ios::binary);
	ofstream dest(destino, ios::binary);
	dest << source.rdbuf();
	source.close();
	dest.close();
}

bool is_patched = false;
void CheckHardware()
{
	/*Check if is Ipatched/Mariko */
	if (spl::GetHardwareType() == "Iowa" || spl::GetHardwareType() == "Hoag" || spl::GetHardwareType() == "Calcio" || spl::HasRCMbugPatched())
	{
		is_patched = true;
	}
}

void SetupClean (){
	if (is_patched){
		led_on(1);
		copy_me("romfs:/EmuKiller/startup.te", "/startup.te");
		copy_me("romfs:/poweroff.bin", "/poweroff.bin");
		copy_me("romfs:/TegraExplorer.bin", "/Haku33.bin");
		mkdir("sdmc:/bootloader",0777);
		mkdir("sdmc:/bootloader/res",0777);
		mkdir("sdmc:/bootloader/ini",0777);
		copy_me("romfs:/ini/Haku33.bmp", "/bootloader/res/Haku33.bmp");
		copy_me("romfs:/ini/Haku33.ini", "/bootloader/ini/Haku33.ini");
		spsmShutdown(true);
	}else{
		led_on(1);
		copy_me("romfs:/startup.te", "/startup.te");
		copy_me("romfs:/poweroff.bin", "/poweroff.bin");
		copy_me("romfs:/TegraExplorer.bin", "/Haku33.bin");
		bpcInitialize();
		if(init_slp()){reboot_to_payload();}
		bpcExit();
	}
}

int main(int argc, char **argv)
{
	set_LANG();
	romfsInit();
	consoleInit(NULL);
	Init_Services();
	CheckHardware();
	PadState pad;
	padConfigureInput(8, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);

    Result rc = 0;

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
			if (is_patched){
				printf("\n\n %s",LG.text7);
			} 
			printf("\n\x1b[31;1m%s \x1b[0m ",Logs);
			printf(LG.text5);


		consoleUpdate(NULL);
		
		//call clean after combo
		if (kHeld & KEY_A)
		{
			SetupClean();
			//break;
		}
		
		//exit
		if (kDown & KEY_B || kDown & KEY_Y || kDown & KEY_X)
		{
			break;
		}
	}

	//cansel
	close_Services();
	consoleExit(NULL);
	return 0;
}