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

#include <sys/stat.h>

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


bool install()
{
	consoleUpdate(NULL);
		//Initialize proc
		printf("\x1b[31;1m*\x1b[0m Initialize proc\n");
		consoleUpdate(NULL);
		fsInitialize();
		pmdmntInitialize();
		pmshellInitialize();
		//terminate
		printf("\x1b[31;1m*\x1b[0m terminate\n");
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
		
		//mount system
		printf("\x1b[31;1m*\x1b[0m mount system\n");
		consoleUpdate(NULL);
		FsFileSystem mySystem;
		fsOpenBisFileSystem(&mySystem, FsBisStorageId_System, "");
		fsdevMountDevice("myssytem", mySystem);
		//delete system
		printf("\x1b[31;1m*\x1b[0m delete system\n");
		consoleUpdate(NULL);
		DeleteDir("myssytem:/save");
		DeleteDir("myssytem:/saveMeta");
		//umount system
		printf("\x1b[31;1m*\x1b[0m umount system\n");
		consoleUpdate(NULL);
		fsdevCommitDevice("myssytem");
		fsdevUnmountDevice("myssytem");
		fsFsClose(&mySystem);
		
		//mount user
		printf("\x1b[31;1m*\x1b[0m mount User\n");
		consoleUpdate(NULL);
		FsFileSystem myUser;
		fsOpenBisFileSystem(&myUser, FsBisStorageId_User, "");
		fsdevMountDevice("myUser", myUser);
		
		//delete user
		printf("\x1b[31;1m*\x1b[0m delete User\n");
		consoleUpdate(NULL);
		DeleteDir("myUser:/Contents/registered");
		DeleteDir("myUser:/Contents");
		DeleteDir("myUser:/saveMeta");
		DeleteDir("myUser:/save");
		
		//umount user
		printf("\x1b[31;1m*\x1b[0m umount User\n");
		consoleUpdate(NULL);
		fsdevCommitDevice("myUser");
		fsdevUnmountDevice("myUser");
		fsFsClose(&myUser);
		
		//exit proc
		printf("\x1b[31;1m*\x1b[0m exit proc\n");
		consoleUpdate(NULL);
		pmdmntExit();
		pmshellExit();
		fsExit();
		
		printf("\x1b[31;1m*\x1b[0m power off\n");
		consoleUpdate(NULL);
			bpcInitialize();
			bpcShutdownSystem();
			bpcExit();
		
/*
	if(isSpanish())
	printf("Estas seguro de que deseas borrar tu informacion personal del prodinfo?\n");
	else
	printf("Are you sure you want erase your personal information from prodinfo?\n");
	if (!confirm())
	{
//		return end();
	}

	return true;*/
return 0;
}

int main(int argc, char **argv)
{
appletBeginBlockingHomeButton(0);

	u64 count = 1000;//kill time
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
					printf("\n\n\x1b[30;1m-------- LO DEVORARE TODO --------\x1b[0m\n\n");
					printf("PULSA + PARA CANSELAR\n\n");
					printf("\x1b[31;1m*\x1b[0m CUENTA ATRAS-%u\n",count);
				}else{
					printf("\n\x1b[30;1m YOUR CONSOLE WILL BE COMPLETELY CLEANED: SAVES, GAMES, ETC  \x1b[0m\n");
					printf("\n\x1b[30;1m IF YOU DON'T KNOW WHAT YOU DO, PRESS + NOW \x1b[0m\n");
					printf("\n\x1b[30;1m A HARD RESET WILL BE PERFORMED IN BRIEF AFTER THE CONSOLE WILL BE OFF \x1b[0m\n");
					printf("\n\n\x1b[30;1m-------- I WILL CONSUME EVERYTHING --------\x1b[0m\n\n");
					printf("PRESS + TO CANCEL\n\n");
					printf("\x1b[31;1m*\x1b[0m COUNTDOWN-%u\n",count);
				}
		consoleUpdate(NULL);
	}

	


    socketExit();
    fsdevUnmountAll();
	appletEndBlockingHomeButton();
	while (appletMainLoop())
	{
		consoleInit(NULL);
		printf("\x1b[32;1m*\x1b[0m %s v%s Kronos2308, Hard Reset\n",TITLE, VERSION);
				if(isSpanish())
				{
					printf("\n\x1b[30;1m PRESIONE HOME PARA SALIR \x1b[0m\n");
				}else{
					printf("\n\x1b[30;1m PRESS HOME TO EXIT \x1b[0m\n");
				}
		consoleUpdate(NULL);
	}

	
	consoleExit(NULL);
	return 0;
}
