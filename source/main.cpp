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

using namespace std;

//traduction
bool isSpanish()
{
			setInitialize();
			u64 lcode = 0;
			s32 lang = 1;
			setGetSystemLanguage(&lcode);
			setMakeLanguage(lcode, &lang);
				switch(lang)
				{
					case 5:
					case 14:
					return true;
					   break;
					default:
					return false;
						break;
				}
			setsysExit();
		return false;
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
				size_t i;
				size_t total_entries;
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

bool install()
{
		//Initialize proc
		printf("\x1b[32;1m*\x1b[0m Initialize proc\n");
		consoleUpdate(NULL);
		fsInitialize();
		pmdmntInitialize();
		pmshellInitialize();
	
		//mount user
		printf("\x1b[32;1m*\x1b[0m mount User\n");
		consoleUpdate(NULL);
		FsFileSystem myUser;
		fsOpenBisFileSystem(&myUser, FsBisStorageId_User, "");
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

		//mount system
		printf("\x1b[32;1m*\x1b[0m mount system\n");
		consoleUpdate(NULL);
		FsFileSystem mySystem;
		fsOpenBisFileSystem(&mySystem, FsBisStorageId_System, "");
		fsdevMountDevice("myssytem", mySystem);
		
		//terminate System proc
		printf("\x1b[32;1m*\x1b[0m terminate System proc\n");
		consoleUpdate(NULL);
		pmshellTerminateProcessByTitleId(0x010000000000000C);//btca	
		pmshellTerminateProcessByTitleId(0x0100000000000009);//settings
		pmshellTerminateProcessByTitleId(0x0100000000000012);//bsdsockets
		pmshellTerminateProcessByTitleId(0x010000000000000E);//friends
		pmshellTerminateProcessByTitleId(0x010000000000000F);//nifm
		pmshellTerminateProcessByTitleId(0x010000000000001E);//account
		pmshellTerminateProcessByTitleId(0x010000000000001F);//ns
		pmshellTerminateProcessByTitleId(0x0100000000000020);//nfc
		pmshellTerminateProcessByTitleId(0x0100000000000022);//capsrv
		pmshellTerminateProcessByTitleId(0x0100000000000024);//ssl
		pmshellTerminateProcessByTitleId(0x0100000000000025);//nim
		pmshellTerminateProcessByTitleId(0x010000000000002B);//erpt
		pmshellTerminateProcessByTitleId(0x010000000000002E);//pctl
		pmshellTerminateProcessByTitleId(0x010000000000002F);//npns
		pmshellTerminateProcessByTitleId(0x0100000000000030);//eupld
		pmshellTerminateProcessByTitleId(0x0100000000000033);//es
		pmshellTerminateProcessByTitleId(0x0100000000000036);//creport
		pmshellTerminateProcessByTitleId(0x010000000000003A);//migration
		pmshellTerminateProcessByTitleId(0x010000000000003E);//olsc
//		pmshellTerminateProcessByTitleId(0x0100000000001000);//qlaunch
		pmshellTerminateProcessByTitleId(0x0100000000001009);//miiEdit

		//delete system
		printf("\x1b[32;1m*\x1b[0m delete system\n");
		consoleUpdate(NULL);
		DeleteDir("myssytem:/save");
		DeleteDir("myssytem:/saveMeta");
		//umount system
		printf("\x1b[32;1m*\x1b[0m umount system\n");
		consoleUpdate(NULL);
		fsdevCommitDevice("myssytem");
		fsdevUnmountDevice("myssytem");
		fsFsClose(&mySystem);
		

		
		//exit proc
		printf("\x1b[32;1m*\x1b[0m exit proc\n");
		consoleUpdate(NULL);
		pmdmntExit();
		pmshellExit();
		fsExit();
led_on();	
		printf("\x1b[32;1m*\x1b[0m power off\n");
		consoleUpdate(NULL);
			bpcInitialize();
			bpcShutdownSystem();
			bpcExit();
		
return 0;
}

int main(int argc, char **argv)
{
appletBeginBlockingHomeButton(0);
	u64 count = 800;//kill time
	while (appletMainLoop())
	{
		hidScanInput();
		u64 keys = hidKeysDown(CONTROLLER_P1_AUTO);
		
		if (keys & KEY_PLUS)
		{
		break;
		}

		if (count <= 0)
		{
		install();
			break;
		}
		
		count--;
		consoleInit(NULL);
			printf("\x1b[32;1m*\x1b[0m %s v%s Kronos2308, Hard Reset\n",TITLE, VERSION);
				if(isSpanish())
				{
					printf("\n\x1b[30;1m TU CONSOLA SERA COMPLETAMENTE LIMPADA: SAVES, JUEGOS, ETC  \x1b[0m\n");
					printf("\n\x1b[30;1m SI NO SABES LO QUE HACES, PRESIONA + RAPIDO \x1b[0m\n");
					printf("\n\x1b[30;1m SE REALIZARA UN HARD RESET EN BREVE LUEGO SE APAGARA LA CONSOLA \x1b[0m\n");
					printf("\n\n\x1b[3%u;1m-------- LO DEVORARE TODO --------\x1b[0m\n\n",count/100);
					printf("PULSA + PARA CANSELAR\n\n");
					printf("\x1b[36m*\x1b[0m CUENTA ATRAS-%u\n",count/100);
				}else{
					printf("\n\x1b[30;1m YOUR CONSOLE WILL BE COMPLETELY CLEANED: SAVES, GAMES, ETC  \x1b[0m\n");
					printf("\n\x1b[30;1m IF YOU DON'T KNOW WHAT YOU DO, PRESS + NOW \x1b[0m\n");
					printf("\n\x1b[30;1m A HARD RESET WILL BE PERFORMED IN BRIEF AFTER THE CONSOLE WILL BE OFF \x1b[0m\n");
					printf("\n\n\x1b[3%u;1m-------- I WILL CONSUME EVERYTHING --------\x1b[0m\n\n",count/100);
					printf("PRESS + TO CANCEL\n\n");
					printf("\x1b[36;1m*\x1b[0m COUNTDOWN-%u\n",count/100);
				}
		consoleUpdate(NULL);
	}

    socketExit();
    fsdevUnmountAll();
	appletEndBlockingHomeButton();
	pmshellInitialize();
	pmshellTerminateProcessByTitleId(0x05229B5E9D160000);//haku33
	pmshellTerminateProcessByTitleId(0x0104444444441001);//haku33
	while (appletMainLoop())
	{
		hidScanInput();
		u64 keys = hidKeysDown(CONTROLLER_P1_AUTO);
		consoleInit(NULL);
		printf("\x1b[32;1m*\x1b[0m %s v%s Kronos2308, Hard Reset\n",TITLE, VERSION);
				if(isSpanish())
				{
					printf("\n\x1b[30;1m PRESIONE HOME PARA SALIR \x1b[0m\n");
				}else{
					printf("\n\x1b[30;1m PRESS HOME TO EXIT \x1b[0m\n");
				}
		if (keys & KEY_PLUS)
		break;
		consoleUpdate(NULL);

	}
	consoleExit(NULL);
	return 0;
}