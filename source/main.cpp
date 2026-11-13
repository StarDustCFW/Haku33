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
#include "imkvdb.hpp"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

int verM = 0;


extern "C" {
#include "reboot.h"
}
/// HidControllerKeys
typedef enum {
    KEY_A            = BIT(0),       ///< A
    KEY_B            = BIT(1),       ///< B
    KEY_X            = BIT(2),       ///< X
    KEY_Y            = BIT(3),       ///< Y
    KEY_LSTICK       = BIT(4),       ///< Left Stick Button
    KEY_RSTICK       = BIT(5),       ///< Right Stick Button
    KEY_L            = BIT(6),       ///< L
    KEY_R            = BIT(7),       ///< R
    KEY_ZL           = BIT(8),       ///< ZL
    KEY_ZR           = BIT(9),       ///< ZR
    KEY_PLUS         = BIT(10),      ///< Plus
    KEY_MINUS        = BIT(11),      ///< Minus
    KEY_DLEFT        = BIT(12),      ///< D-Pad Left
    KEY_DUP          = BIT(13),      ///< D-Pad Up
    KEY_DRIGHT       = BIT(14),      ///< D-Pad Right
    KEY_DDOWN        = BIT(15),      ///< D-Pad Down
    KEY_LSTICK_LEFT  = BIT(16),      ///< Left Stick Left
    KEY_LSTICK_UP    = BIT(17),      ///< Left Stick Up
    KEY_LSTICK_RIGHT = BIT(18),      ///< Left Stick Right
    KEY_LSTICK_DOWN  = BIT(19),      ///< Left Stick Down
    KEY_RSTICK_LEFT  = BIT(20),      ///< Right Stick Left
    KEY_RSTICK_UP    = BIT(21),      ///< Right Stick Up
    KEY_RSTICK_RIGHT = BIT(22),      ///< Right Stick Right
    KEY_RSTICK_DOWN  = BIT(23),      ///< Right Stick Down
    KEY_SL_LEFT      = BIT(24),      ///< SL on Left Joy-Con
    KEY_SR_LEFT      = BIT(25),      ///< SR on Left Joy-Con
    KEY_SL_RIGHT     = BIT(26),      ///< SL on Right Joy-Con
    KEY_SR_RIGHT     = BIT(27),      ///< SR on Right Joy-Con

    KEY_HOME         = BIT(18),      ///< HOME button, only available for use with HiddbgHdlsState::buttons.
    KEY_CAPTURE      = BIT(19),      ///< Capture button, only available for use with HiddbgHdlsState::buttons.

    // Pseudo-key for at least one finger on the touch screen
    KEY_TOUCH       = BIT(28),

    // Buttons by orientation (for single Joy-Con), also works with Joy-Con pairs, Pro Controller
    KEY_JOYCON_RIGHT = BIT(0),
    KEY_JOYCON_DOWN  = BIT(1),
    KEY_JOYCON_UP    = BIT(2),
    KEY_JOYCON_LEFT  = BIT(3),

    // Generic catch-all directions, also works for single Joy-Con
    KEY_UP    = KEY_DUP     | KEY_LSTICK_UP    | KEY_RSTICK_UP,    ///< D-Pad Up or Sticks Up
    KEY_DOWN  = KEY_DDOWN   | KEY_LSTICK_DOWN  | KEY_RSTICK_DOWN,  ///< D-Pad Down or Sticks Down
    KEY_LEFT  = KEY_DLEFT   | KEY_LSTICK_LEFT  | KEY_RSTICK_LEFT,  ///< D-Pad Left or Sticks Left
    KEY_RIGHT = KEY_DRIGHT  | KEY_LSTICK_RIGHT | KEY_RSTICK_RIGHT, ///< D-Pad Right or Sticks Right
    KEY_SL    = KEY_SL_LEFT | KEY_SL_RIGHT,                        ///< SL on Left or Right Joy-Con
    KEY_SR    = KEY_SR_LEFT | KEY_SR_RIGHT,                        ///< SR on Left or Right Joy-Con
} HidControllerKeys;

using namespace std;
Result Init_Services(void)
{
	Result ret = 0;
	if (R_FAILED(ret = setsysInitialize())) {return ret;}
	if (R_FAILED(ret = splInitialize())) {return ret;}
	if (R_FAILED(ret = hiddbgInitialize())) {return ret;}
	if (R_FAILED(ret = psmInitialize())) {return ret;}
	return ret;
}

void close_Services()
{
	setsysExit();
	splExit();
	hiddbgExit();
	psmExit();
}

bool IsExist(std::string Path) 
{
    std::ifstream ifs(Path);
    bool ex = ifs.good();
    ifs.close();
    return ex;
}

// Copies origen to destino. Returns false, without creating or truncating
// destino, when origen cannot be opened.
bool copy_me(const string &origen, const string &destino) {
	ifstream source(origen, ios::binary);
	if (!source.good()) return false;
	ofstream dest(destino, ios::binary | ios::trunc);
	if (!dest.good()) return false;
	// operator<< sets failbit when it inserts nothing, so an empty file is
	// handled apart.
	if (source.peek() != ifstream::traits_type::eof()) dest << source.rdbuf();
	dest.close();
	return !dest.fail();
}

bool read_file(const char *path, std::vector<u8> &out) {
	out.clear();
	FILE *f = fopen(path, "rb");
	if (f == NULL) return false;
	u8 buf[0x1000];
	size_t n;
	while ((n = fread(buf, 1, sizeof(buf), f)) > 0) out.insert(out.end(), buf, buf + n);
	bool ok = !ferror(f);
	fclose(f);
	return ok;
}

bool write_file(const char *path, const std::vector<u8> &data) {
	FILE *f = fopen(path, "wb");
	if (f == NULL) return false;
	bool ok = fwrite(data.data(), 1, data.size(), f) == data.size();
	if (fclose(f) != 0) ok = false;
	return ok;
}

bool is_patched = false;
bool is_emummc = false;
void CheckHardware()
{
	/*Check if is Ipatched/Mariko */
	const char *hw = spl::GetHardwareType();
	if (!strcmp(hw, "Iowa") || !strcmp(hw, "Hoag") || !strcmp(hw, "Calcio") || !strcmp(hw, "Aula") || spl::HasRCMbugPatched())
	{
		is_patched = true;
	}
	is_emummc = spl::HasEmummc();
}

// Boot files replaced on the SD of patched consoles so the modchip starts
// TegraExplorer. What was there before is recorded in BACKUP_DIR:
//   BACKUP_DIR/<name>       copy of the original file
//   BACKUP_DIR/<name>.none  marker, there was no such file
// romfs/startup.te puts everything back using the same layout.
static const char *const BACKUP_DIR = "sdmc:/Haku33_backup";
static const char *const BOOT_FILES[] = {"payload.bin", "boot.dat", "boot.ini"};

static bool backup_boot_file(const string &name) {
	const string src = string("sdmc:/") + name;
	const string bak = string(BACKUP_DIR) + "/" + name;
	const string none = bak + ".none";
	// An earlier run that never got to restore already recorded the original
	// state. The file on the SD may be ours by now, so keep that record.
	if (IsExist(bak) || IsExist(none)) return true;
	if (IsExist(src)) return copy_me(src, bak);
	ofstream marker(none, ios::binary);
	marker << "none";
	marker.close();
	return IsExist(none);
}

static void restore_boot_files() {
	for (const char *name : BOOT_FILES) {
		const string src = string("sdmc:/") + name;
		const string bak = string(BACKUP_DIR) + "/" + name;
		const string none = bak + ".none";
		if (IsExist(bak)) {
			if (copy_me(bak, src)) remove(bak.c_str());
		} else if (IsExist(none)) {
			remove(src.c_str());
			remove(none.c_str());
		}
	}
	rmdir(BACKUP_DIR);
}

// Starts the TegraExplorer wipe (romfs/startup.te). On success the console
// reboots or powers off and this does not return. Returns false, with the SD
// left as it was, when the wipe could not be started.
bool SetupClean (){
    led_on(2);
    if (!copy_me("romfs:/startup.te", "sdmc:/startup.te")) {
        printf(LG["ERROR3"]);
        return false;
    }
	if (is_patched){
		mkdir(BACKUP_DIR, 0777);
		bool ok = true;
		for (const char *name : BOOT_FILES) ok = backup_boot_file(name) && ok;

		if (ok) {
			//force boot sxcore
			ok = copy_me("romfs:/boot.dat", "sdmc:/boot.dat") && ok;
			ok = copy_me("romfs:/boot.ini", "sdmc:/boot.ini") && ok;

			//force boot HWFLY PicoFly
			ok = copy_me("romfs:/TegraExplorer.bin", "sdmc:/payload.bin") && ok;
		}

		if (!ok) {
			restore_boot_files();
			remove("sdmc:/startup.te");
			printf(LG["ERROR3"]);
			return false;
		}

		led_on(1);
		spsmInitialize();
		spsmShutdown(false);
		return true;
	}else{
        //Erista boot
		led_on(1);
		bpcInitialize();
		if(init_slp()){reboot_to_payload("romfs:/TegraExplorer.bin");}
		bpcExit();
		// Only reached when the reboot did not happen: do not leave the wipe
		// script armed on the SD.
		remove("sdmc:/startup.te");
		printf(LG["ERROR4"]);
		return false;
	}
}

enum NewCleanResult {
	NewClean_Done = 0,    // index replaced, console is powering off
	NewClean_NoAccess,    // the indexer save could not be opened, nothing changed
	NewClean_Failed,      // error after opening, nothing was committed
};

// 17.0.0+: reset the save data index from inside Horizon.
// The new index is made of the console's own records for 8000000000000120
// and 8000000000000053 (see imkvdb.hpp), the same result romfs/startup.te
// produces, instead of a canned file with a fixed size and no 0053 record.
NewCleanResult NewClean(){
    const u64 uidsave = 0x8000000000000000;
    AccountUid uid = {};
    FsFileSystem dataW;

    Result rc = fsOpen_SystemSaveData (&dataW,FsSaveDataSpaceId_System,uidsave,uid);
    if(R_FAILED(rc)) {
        printf("fsOpen_SystemSaveData() 0x%x\n", rc);
        return NewClean_NoAccess;
    }

    // fsdev owns dataW from here, also when the mount fails.
    if (fsdevMountDevice("bro", dataW) == -1) {
        printf(LG["ERROR5"], "mount");
        return NewClean_Failed;
    }

    std::vector<u8> oldIndex, newIndex;
    const char *failed = NULL;
    if (!read_file("bro:/imkvdb.arc", oldIndex)) failed = "read";
    else if (!imkvdb::BuildCleanIndex(oldIndex, newIndex)) failed = "parse";
    else if (!write_file("bro:/imkvdb.arc", newIndex)) failed = "write";
    else if (R_FAILED(rc = fsdevCommitDevice("bro"))) failed = "commit";

    // Closes the save. Anything written but not committed is discarded.
    fsdevUnmountDevice("bro");

    if (failed != NULL) {
        printf(LG["ERROR5"], failed);
        return NewClean_Failed;
    }

    led_on(1);
	spsmInitialize();
	spsmShutdown(false);
    return NewClean_Done;
}

// The TegraExplorer wipe needs /switch/prod.keys on patched consoles
// (TegraExplorer cannot derive keys there).
void StartTegraWipe(bool keysok){
    if (is_patched && !keysok){
        printf(LG["ERROR1"]);
        return;
    }
    SetupClean();
}


int main(int argc, char **argv)
{
	set_LANG();
	romfsInit();
	consoleInit(NULL);
	Result init_rc = Init_Services();
	CheckHardware();
	PadState pad;
	padConfigureInput(8, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    bool keysok = IsExist("/switch/prod.keys");
    int armed = 0; // 0: nothing, 1: A (clean), 2: X/Y (TegraExplorer wipe)
    Result rc = 0;
	SetSysFirmwareVersion ver;
	if (R_SUCCEEDED(rc = setsysGetFirmwareVersion(&ver))) {
		verM = (int)ver.major;
	}
    //main menu
    consoleClear();
    printf("\x1b[32;1m*\x1b[0m %s v%s Kronos2308, Hard Reset \n\n",TITLE, VERSION);
    printf(LG["WARNING1"]);
    printf(LG["WARNING2"]);
    printf(LG["WARNING3"]);
    printf(LG["MESSAGE1"]);
    if (R_FAILED(init_rc)) {
        printf("Init_Services() Failed: 0x%x.\n\n", init_rc);
    }
    if (R_FAILED(rc)) {
        printf("setsysGetFirmwareVersion() Failed: 0x%x.\n\n", rc);
    }
    if (is_patched){
        printf("\n\n %s",LG["INFO0"]);
    }
    if (is_emummc){
        printf(LG["INFO1"]);
    }
    if (is_patched && !keysok){
        printf(LG["ERROR1"]);
    }
    printf(LG["PROMPT1"]);
    printf(LG["PROMPT2"]);

	//keys
	while (appletMainLoop())
	{
		padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);

		int pressed = 0;
		if (kDown & KEY_A) pressed = 1;
		else if (kDown & (KEY_X | KEY_Y)) pressed = 2;

		if (pressed != 0 && pressed != armed){
			//first press only asks, and asks once
			armed = pressed;
			printf(LG["CONFIRM1"]);
		}
		else if (pressed == 1){
			armed = 0;
			if (verM >= 17){
				if (is_emummc){
					//this would edit the emuMMC index, not sysNAND
					printf(LG["INFO1"]);
				} else if (NewClean() == NewClean_NoAccess){
					StartTegraWipe(keysok);
				}
			} else {
				StartTegraWipe(keysok);
			}
		}
		else if (pressed == 2){
			armed = 0;
			StartTegraWipe(keysok);
		}

        consoleUpdate(NULL);

		//exit
		if (kDown & KEY_B)
		{
			break;
		}
	}

	//cansel
	close_Services();
	consoleExit(NULL);
	return 0;
}
