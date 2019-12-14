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

extern "C" {
#include "reboot.h"
}

using namespace std;

//traduction
bool isSpanish = false;
void set_LANG()
{
			setInitialize();
			u64 lcode = 0;
			SetLanguage lang;
			setGetSystemLanguage(&lcode);
			setMakeLanguage(lcode, &lang);
				switch(lang)
				{
					case 5:
					case 14:
					isSpanish =  true;
					   break;
					default:
					isSpanish =  false;
						break;
				}
			setsysExit();
}

//ask to the switch for the serial detect incognito
char *incognito(void) {
	setInitialize();
    setsysInitialize();
	Result ret = 0;
	static char serial[0x19];
	if (R_FAILED(ret = setsysGetSerialNumber(serial)))
	printf("setsysGetSerialNumber() failed: 0x%x.\n\n", ret);
	setsysExit();
	return serial;
}


	bool fileExists(const char* path)
	{
		FILE* f = fopen(path, "rb");
		if (f)
		{
			fclose(f);
			return true;
		}
		return false;
	}

	bool IsExist(std::string Path) 
	{
		std::ifstream ifs(Path);
		bool ex = ifs.good();
		ifs.close();
		return ex;
	}

	bool DirExists(const char* const path)
	{
		struct stat info;

		int statRC = stat(path, &info);
		if (statRC != 0)
		{
			if (errno == ENOENT) { return 0; } // something along the path does not exist
			if (errno == ENOTDIR) { return 0; } // something in path prefix is not a dir
			return false;
		}

		return (info.st_mode & S_IFDIR) ? true : false;
	}

	bool IsFile(std::string Path)
	{
		bool is = false;
		struct stat st;
		if (stat(Path.c_str(), &st) == 0) if (st.st_mode & S_IFREG) is = true;
		return is;
	}
	void CreateFile(std::string Path)
	{
		std::ofstream ofs(Path);
		ofs.close();
	}
	void CreateDir(std::string Path) 
	{
		mkdir(Path.c_str(), 777);
	}
	void DeleteFile(std::string Path)
	{
		if(IsExist(Path))
			remove(Path.c_str());
	}

	void DeleteDir(std::string Path)
	{
		DIR *d = opendir(Path.c_str());
		if (d)
		{
			struct dirent *dent;
			while (true)
			{
				dent = readdir(d);
				if (dent == NULL) break;
				std::string nd = dent->d_name;
				std::string pd = Path + "/" + nd;
				if (IsFile(pd)) DeleteFile(pd);
				else DeleteDir(pd);
			}
		}
		closedir(d);
	}

	
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

	bool HasConnection()
	{
		u32 strg = 0;
		nifmInitialize(NifmServiceType_Admin);
		nifmGetInternetConnectionStatus(NULL, &strg, NULL);
		nifmExit();
		return (strg > 0);
	}

bool install()
{
		//Initialize proc
		printf("\x1b[32;1m*\x1b[0m Initialize proc\n");
		consoleUpdate(NULL);
		fsInitialize();
		pmdmntInitialize();
		pmshellInitialize();
		
		//terminate System proc
		printf("\x1b[32;1m*\x1b[0m Kill System Services\n");
		consoleUpdate(NULL);
		pmshellTerminateProgram(0x010000000000000C);//btca	
		pmshellTerminateProgram(0x010000000000000E);//friends
		pmshellTerminateProgram(0x010000000000001E);//account
		pmshellTerminateProgram(0x010000000000001F);//ns
		pmshellTerminateProgram(0x0100000000000020);//nfc
		pmshellTerminateProgram(0x0100000000000022);//capsrv
		pmshellTerminateProgram(0x0100000000000024);//ssl
		pmshellTerminateProgram(0x0100000000000025);//nim
		pmshellTerminateProgram(0x010000000000002B);//erpt
		pmshellTerminateProgram(0x010000000000002E);//pctl
		pmshellTerminateProgram(0x010000000000002F);//npns
		pmshellTerminateProgram(0x0100000000000030);//eupld
		pmshellTerminateProgram(0x0100000000000033);//es
		pmshellTerminateProgram(0x0100000000000036);//creport
		pmshellTerminateProgram(0x010000000000003A);//migration
		pmshellTerminateProgram(0x010000000000003E);//olsc
		pmshellTerminateProgram(0x0100000000001000);//qlaunch - make freeze?
		pmshellTerminateProgram(0x0100000000001009);//miiEdit
		
		//serv test may freeze
		printf("\x1b[32;1m*\x1b[0m Kill Extra Services\n");
		consoleUpdate(NULL);
/*		pmshellTerminateProgram(0x0100000000000020); //nfc
		pmshellTerminateProgram(0x0100000000000021); //psc
//		pmshellTerminateProgram(0x0100000000000023); //am    make freeze
		pmshellTerminateProgram(0x0100000000000024); //ssl
		pmshellTerminateProgram(0x010000000000002E); //pctl
		pmshellTerminateProgram(0x010000000000002F); //npns
		pmshellTerminateProgram(0x0100000000000034); //fatal
//		pmshellTerminateProgram(0x0100000000000037); //ro   make freeze
		pmshellTerminateProgram(0x0100000000000039); //sdb
		pmshellTerminateProgram(0x010000000000003E); //olsc
*/		pmshellTerminateProgram(0x0100000000002071); //posi (ns)
		pmshellTerminateProgram(0x0100000000000809); //used by sdb
		
		//critical serv 
		printf("\x1b[32;1m*\x1b[0m terminate Critical Services\n");
		consoleUpdate(NULL);
		pmshellTerminateProgram(0x0100000000000012);//bsdsockets - make switch freeze on sxos ftp
		pmshellTerminateProgram(0x0100000000000009);//settings - make switch freeze on airplane mode
		pmshellTerminateProgram(0x010000000000000F);//nifm
		pmshellTerminateProgram(0x0100000000000016);//Wlan
	
		//terminate Homebrew Serv
		printf("\x1b[32;1m*\x1b[0m Kill Homebrew Services\n");
		consoleUpdate(NULL);
		pmshellTerminateProgram(0x420000000000000E);//FTP
		pmshellTerminateProgram(0x0100000000000352);//Emuiio
		pmshellTerminateProgram(0x200000000000010);//Lan Play
		pmshellTerminateProgram(0x0100000000000FAF);//HDI
		pmshellTerminateProgram(0x420000000000000B);//sysplay
		pmshellTerminateProgram(0x00FF0000636C6BFF);//sys-clk
		pmshellTerminateProgram(0x0100000000534C56);//ReverseNX
		pmshellTerminateProgram(0x0100000000000069);//ReiSpoof
		
		//mount system
		printf("\x1b[32;1m*\x1b[0m mount system\n");
		consoleUpdate(NULL);
		FsFileSystem mySystem;
		fsOpenBisFileSystem(&mySystem, FsBisPartitionId_System, "");
		fsdevMountDevice("myssytem", mySystem);
		//delete system
		printf("\x1b[32;1m*\x1b[0m Delete system\n");
		consoleUpdate(NULL);
		DeleteDir("myssytem:/save");//perform the hard reset
		DeleteDir("myssytem:/saveMeta");
		//umount system
		printf("\x1b[32;1m*\x1b[0m umount system\n");
		consoleUpdate(NULL);
		fsdevCommitDevice("myssytem");
		fsdevUnmountDevice("myssytem");
		fsFsClose(&mySystem);

		
		//mount user
		printf("\x1b[32;1m*\x1b[0m mount User\n");
		consoleUpdate(NULL);
		FsFileSystem myUser;
		fsOpenBisFileSystem(&myUser, FsBisPartitionId_User, "");
		fsdevMountDevice("myUser", myUser);
		//delete user
		printf("\x1b[32;1m*\x1b[0m delete User\n");
		consoleUpdate(NULL);
		DeleteDir("myUser:/Contents/registered");
		DeleteDir("myUser:/Contents");
		DeleteDir("myUser:/saveMeta");
		DeleteDir("myUser:/save");
		//umount user
		printf("\x1b[32;1m*\x1b[0m umount User\n");
		consoleUpdate(NULL);
		fsdevCommitDevice("myUser");
		fsdevUnmountDevice("myUser");
		fsFsClose(&myUser);

		//exit proc
		printf("\x1b[32;1m*\x1b[0m exit proc\n");
		consoleUpdate(NULL);
		pmdmntExit();
		pmshellExit();
		fsExit();
		socketExit();
		fsdevUnmountAll();
led_on();	
			bpcInitialize();
			if(init_slp())
			{
				reboot_to_payload();
			}
			else
			{
				bpcShutdownSystem();
			}
//			bpcRebootSystem();
			bpcExit();
return 0;
}

int main(int argc, char **argv)
{
		//romfs
		romfsInit();

		//keys
	u32 minus = 0;
	u32 more = 0;
	u32 LT = 0;
	u32 RT = 0;
	while (appletMainLoop())
	{

	    hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
		
	minus = 0;
	more = 0;
	LT = 0;
	RT = 0;
if (kHeld & KEY_ZL)
	LT = 2;
if (kHeld & KEY_ZR)
	RT = 2;
if (kHeld & KEY_MINUS)
	minus = 2;
if (kHeld & KEY_PLUS)
	more = 2;

		consoleInit(NULL);
			printf("\x1b[32;1m*\x1b[0m %s v%s Kronos2308, Hard Reset\n",TITLE, VERSION);
				if(isSpanish)
				{
					printf("\n\x1b[30;1m TU CONSOLA SERA COMPLETAMENTE LIMPADA: SAVES, JUEGOS, ETC  \x1b[0m\n");
					printf("\n\x1b[30;1m SE REALIZARA UN HARD RESET LUEGO SE APAGARA LA CONSOLA \x1b[0m\n");
					printf("\n\x1b[30;1m SI NO SABES LO QUE HACES, PRESIONA B PARA ABORTAR \x1b[0m\n");
					printf("\n\n\x1b[30;1m-------- LO DEVORARE TODO --------\x1b[0m\n\n");
					printf("\x1b[30;1m PULSA \x1b[3%u;1m -\x1b[3%u;1m +\x1b[3%u;1m ZR\x1b[3%u;1m ZL \x1b[0m \x1b[30;1m PARA LIMPIAR\n\n",minus,more,RT,LT);
					if(strlen(incognito()) == 0)//detect incognito
					printf("\x1b[31;1m*\x1b[0m Desinstala Incognito %s(Requerido)\n\n",incognito());
					if(!HasConnection())//detect airplane mode for evoid freeze
					printf("\x1b[31;1m*\x1b[0m Desactiva el Modo Avion usar las 90DNS (Requerido)\n\n\x1b[33;1m*\x1b[0m DNS Primario: 163.172.141.219\n\n\x1b[33;1m*\x1b[0m DNS Secundario: 207.246.121.77\n\n");
					if(fileExists("license.dat"))
					printf("\x1b[33;1m*\x1b[0m Si lo tienes activo, Apaga el FTP de sxos\n\n");

				}else{
					printf("\n\x1b[30;1m YOUR CONSOLE WILL BE COMPLETELY CLEANED: SAVES, GAMES, ETC  \x1b[0m\n");
					printf("\n\x1b[30;1m A HARD RESET WILL BE PERFORMED AFTER THE CONSOLE WILL BE OFF \x1b[0m\n");
					printf("\n\x1b[30;1m IF YOU DON'T KNOW WHAT YOU DO, PRESS B FOR ABORT \x1b[0m\n");
					printf("\n\n\x1b[30;1m-------- I WILL CONSUME EVERYTHING --------\x1b[0m\n\n");
					printf("PRESS - + ZR ZL TO CLEAN\n\n");
					if(strlen(incognito()) == 0)//detect incognito
					printf("\x1b[31;1m*\x1b[0m Uninstall Incognito (Required)\n\n");
					if(!HasConnection())//detect airplane mode for evoid freeze
					printf("\x1b[31;1m*\x1b[0m Disable Airplane mode and use 90DNS(Required)\n\n\x1b[32;1m*\x1b[0m Primary DNS: 163.172.141.219\n\n\x1b[32;1m*\x1b[0m Secondary DNS: 207.246.121.77\n\n");
				if(fileExists("license.dat"))
					printf("\x1b[33;1m*\x1b[0m If you have it active, Turn off FTP sxos\n\n");

				}
		consoleUpdate(NULL);
		
		
		if ((kDown & KEY_ZL || kDown & KEY_ZR || kDown & KEY_MINUS || kDown & KEY_PLUS) && (kHeld & KEY_ZL && kHeld & KEY_ZR && kHeld & KEY_MINUS && kHeld & KEY_PLUS))
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