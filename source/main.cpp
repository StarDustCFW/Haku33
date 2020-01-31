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
#include "ftpdisable.h"
}

using namespace std;
//Vars
bool issxos = false;
bool isEMU = false;

	bool Check_Service(const char* name)
	{
		Handle txHandle;
		Result rc = smRegisterService(&txHandle, smEncodeName(name), false, 1);
		if (R_FAILED(rc))
		return true;
		else
		smUnregisterService(smEncodeName(name));
		return false;
	}
	
	void espera(u32 timeS)
	{
		u32 cout = 0;
		while (appletMainLoop()){cout++; if(cout >= timeS*1000000) break;}//1000000
	}

	//ask to the switch for the serial detect incognito
	char *incognito(void) 
	{
		setInitialize();
		setsysInitialize();
		Result ret = 0;
		static char serial[0x19];
		if (R_FAILED(ret = setsysGetSerialNumber(serial)))
		printf("setsysGetSerialNumber() failed: 0x%x.\n\n", ret);
		setsysExit();
		return serial;
	}

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

	//check connnection 
	bool HasConnection()
	{
		u32 strg = 0;
		nifmInitialize(NifmServiceType_Admin);
		nifmGetInternetConnectionStatus(NULL, &strg, NULL);
		nifmExit();
		return (strg > 0);
	}


	//Clean Funtion
	bool install()
	{
		//warning
		printf(LG.text9);
		consoleUpdate(NULL);
		
		//Detect sxos and disable ftp
		if (issxos)
		{
			//force disable sxos
			printf(LG.text0);
			txinit();
			txforcedisableftp();
			txexit();
		}
		consoleUpdate(NULL);
//		espera(10);

		//terminate Homebrew Serv
		printf("\x1b[32;1m*\x1b[0m Kill Homebrew Services\n");
		consoleUpdate(NULL);
		if (R_SUCCEEDED(pmshellTerminateProgram(0x420000000000000E))){printf("FTP-");}
		if (R_SUCCEEDED(pmshellTerminateProgram(0x0100000000000352))){printf("Emuiio-");}
		if (R_SUCCEEDED(pmshellTerminateProgram(0x4200000000000010))){printf("Lan Play-");}
		if (R_SUCCEEDED(pmshellTerminateProgram(0x0100000000000BEF))){printf("Disk-USB-");}
		if (R_SUCCEEDED(pmshellTerminateProgram(0x420000000000000B))){printf("SysPlay-");}
		if (R_SUCCEEDED(pmshellTerminateProgram(0x00FF0000636C6BFF))){printf("sys-clk-");}
		if (R_SUCCEEDED(pmshellTerminateProgram(0x690000000000000D))){printf("Sys-Con-");}
		if (R_SUCCEEDED(pmshellTerminateProgram(0x00FF0000A53BB665))){printf("SysDVR-");}
		if (R_SUCCEEDED(pmshellTerminateProgram(0x0100000000534C56))){printf("ReverseNX-");}
		if (R_SUCCEEDED(pmshellTerminateProgram(0x0100000000000FAF))){printf("HDI-");}
		if (R_SUCCEEDED(pmshellTerminateProgram(0x0100000000000069))){printf("ReiSpoof-");}

		//DeInitialize
		printf("\n\x1b[32;1m*\x1b[0m DeInitialize\n");
		consoleUpdate(NULL);
		hidsysExit();
		socketExit();
		fsdevUnmountAll();
		pcvExit();
		psmExit();
		nsExit();
		apmExit();
		socketExit();
		nifmExit();
		splExit();
		setsysExit();
		setExit();
		
		//Initialize proc
		printf("\n\x1b[32;1m*\x1b[0m Initialize Proc\n");
		consoleUpdate(NULL);
		fsInitialize();
		pmdmntInitialize();
		pmshellInitialize();
		
		//mount user
		printf("\x1b[32;1m*\x1b[0m mount User\n");
		consoleUpdate(NULL);
		FsFileSystem myUser;
		fsOpenBisFileSystem(&myUser, FsBisPartitionId_User, "");
		fsdevMountDevice("myUser", myUser);

		//mount system
		printf("\x1b[32;1m*\x1b[0m mount system\n");
		consoleUpdate(NULL);
		FsFileSystem mySystem;
		fsOpenBisFileSystem(&mySystem, FsBisPartitionId_System, "");
		fsdevMountDevice("myssytem", mySystem);
		
		//terminate System proc
		printf("\x1b[32;1m*\x1b[0m Kill System Services\n");
		consoleUpdate(NULL);
		if (R_FAILED(pmshellTerminateProgram(0x010000000000000C))){printf("btca-");}
		if (R_FAILED(pmshellTerminateProgram(0x010000000000000E))){printf("friends-");}
		if (R_FAILED(pmshellTerminateProgram(0x010000000000001E))){printf("account-");}
		if (R_FAILED(pmshellTerminateProgram(0x010000000000001F))){printf("ns-");}
		if (R_FAILED(pmshellTerminateProgram(0x0100000000000020))){printf("nfc-");}
		if (R_FAILED(pmshellTerminateProgram(0x0100000000000022))){printf("capsrv-");}
		if (R_FAILED(pmshellTerminateProgram(0x0100000000000024))){printf("ssl-");}
		if (R_FAILED(pmshellTerminateProgram(0x0100000000000025))){printf("nim-");}
		if (R_FAILED(pmshellTerminateProgram(0x010000000000002B))){printf("erpt-");}
		if (R_FAILED(pmshellTerminateProgram(0x010000000000002E))){printf("pctl-");}
		if (R_FAILED(pmshellTerminateProgram(0x010000000000002F))){printf("npns-");}
		if (R_FAILED(pmshellTerminateProgram(0x0100000000000030))){printf("eupld-");}
		if (R_FAILED(pmshellTerminateProgram(0x0100000000000033))){printf("es-");}
		if (R_FAILED(pmshellTerminateProgram(0x0100000000000036))){printf("creport-");}
		if (R_FAILED(pmshellTerminateProgram(0x010000000000003A))){printf("migration-");}
		if (R_FAILED(pmshellTerminateProgram(0x010000000000003E))){printf("olsc-");}
		if (R_FAILED(pmshellTerminateProgram(0x0100000000001009))){printf("miiEdit-");}
		if (R_FAILED(pmshellTerminateProgram(0x0100000000002071))){printf("ns.use-");}
		if (R_FAILED(pmshellTerminateProgram(0x0100000000000809))){printf("used by sdb-");}

		//critical serv 
		printf("\n\x1b[32;1m*\x1b[0m terminate Critical Services\n");
		consoleUpdate(NULL);
		if (R_FAILED(pmshellTerminateProgram(0x0100000000000012))){printf("bsdsockets-");}
		if (R_FAILED(pmshellTerminateProgram(0x0100000000000009))){printf("settings-");}
		if (R_FAILED(pmshellTerminateProgram(0x010000000000000F))){printf("nifm-");}
		if (R_FAILED(pmshellTerminateProgram(0x0100000000000016))){printf("Wlan-");}
		if (R_FAILED(pmshellTerminateProgram(0x0100000000001000))){printf("qlaunch-");}

		//delete user
		printf("\n\x1b[32;1m*\x1b[0m Delete User\n");
		consoleUpdate(NULL);
		fs::DeleteDir("myUser:/Contents/registered");
		fs::DeleteDir("myUser:/Contents");
		fs::DeleteDir("myUser:/saveMeta");
		fs::DeleteDir("myUser:/save");
		//umount user
		printf("\x1b[32;1m*\x1b[0m umount User\n");
		consoleUpdate(NULL);
		fsdevCommitDevice("myUser");
		fsdevUnmountDevice("myUser");
		fsFsClose(&myUser);

		//delete system
		printf("\x1b[32;1m*\x1b[0m Delete system\n");
		consoleUpdate(NULL);
		fs::DeleteDir("myssytem:/saveMeta");
		fs::DeleteDir("myssytem:/save");//perform the hard reset
		//umount system
		printf("\x1b[32;1m*\x1b[0m umount system\n");
		consoleUpdate(NULL);
		fsdevCommitDevice("myssytem");
		fsdevUnmountDevice("myssytem");
		fsFsClose(&mySystem);

		//exit proc
		printf("\x1b[32;1m*\x1b[0m Exit proc\n");
		consoleUpdate(NULL);
		pmdmntExit();
		pmshellExit();
		fsExit();
		socketExit();
		fsdevUnmountAll();
		led_on();
		
		bpcInitialize();
		if(init_slp())
		{reboot_to_payload();}else{bpcShutdownSystem();}
		bpcExit();
	return 0;
	}

int main(int argc, char **argv)
{
	set_LANG();
	issxos = Check_Service("tx");
	isEMU = Check_Service("FS");
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
			
			if (isEMU) printf("\nEMU\n");
					printf(LG.text1);
					printf(LG.text2);
					printf(LG.text3);
					printf(LG.text4);
					printf(LG.text5,LT,minus,more,RT);
					if(strlen(incognito()) == 0)//detect incognito
					printf(LG.text6);
					if(!HasConnection())//detect airplane mode for evoid freeze
					{
						printf(LG.text7);
						if (!issxos)//extra warning if you are not on sxos
							printf(LG.text8);
					}


		consoleUpdate(NULL);
		
		//call clean after combo
		if (kHeld & KEY_ZL && kHeld & KEY_ZR && kHeld & KEY_MINUS && kHeld & KEY_PLUS)
		{
			install();
			break;
		}

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