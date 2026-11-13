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
#include <stdarg.h>

int verM = 0;

// Appends a line to sdmc:/Haku33.log, so a run can be looked at afterwards.
// romfs/startup.te keeps its own log in sdmc:/Haku33_te.log. The file is
// opened and closed for every line: the console may reboot or power off right
// after. A log that cannot be written is ignored.
static void hlog(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static void hlog(const char *fmt, ...) {
	FILE *f = fopen("sdmc:/Haku33.log", "a");
	if (f == NULL) return;
	va_list ap;
	va_start(ap, fmt);
	vfprintf(f, fmt, ap);
	va_end(ap);
	fputc('\n', f);
	fclose(f);
}


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

// File of zeros that romfs/startup.te copies over the free space of SYSTEM and
// USER after deleting, so the deleted data cannot be read back from a raw dump.
static const char *const ZERO_DIR = "sdmc:/Haku33_zero";
static const char *const ZERO_FILE = "sdmc:/Haku33_zero/zero.bin";
static const size_t ZERO_FILE_MIB = 128;

static void remove_zero_file() {
	remove(ZERO_FILE);
	rmdir(ZERO_DIR);
}

static bool write_zero_file() {
	static u8 block[0x100000]; // zero initialized
	mkdir(ZERO_DIR, 0777);
	FILE *f = fopen(ZERO_FILE, "wb");
	if (f == NULL) return false;
	bool ok = true;
	for (size_t i = 0; i < ZERO_FILE_MIB && ok; i++) ok = fwrite(block, 1, sizeof(block), f) == sizeof(block);
	if (fclose(f) != 0) ok = false;
	if (!ok) remove_zero_file();
	return ok;
}

// ---- Free space overwrite, first pass, done here while Horizon is running.
// TegraExplorer writes to the eMMC at about 2 MB/s, so overwriting the free
// space of a 32 GB console from startup.te alone takes more than three hours.
// Horizon writes much faster, so the free space SYSTEM and USER have now is
// filled with zeros here, into the same Haku33_fill folder startup.te uses.
// startup.te then deletes the data, overwrites only what that freed and the
// margin left here, and removes the folder. A margin is left because Horizon
// is still running. Only for the NAND Horizon is running from, so never when
// running from emuMMC (the wipe target chosen later would be another NAND).
struct FillTarget { FsBisPartitionId id; const char *dev; const char *label; };
static const FillTarget FILL_TARGETS[] = {
	{ FsBisPartitionId_System, "hsys", "SYSTEM" },
	{ FsBisPartitionId_User,   "husr", "USER"   },
};
static const u64 FILL_MARGIN   = 64ULL << 20;   // left free on each partition
static const u64 FILL_FILE_MAX = 1024ULL << 20; // per file, FAT32 allows just under 4 GiB
static const size_t FILL_CHUNK = 0x100000;

static bool mount_bis(const FillTarget &t) {
	FsFileSystem fs;
	Result rc = fsOpenBisFileSystem(&fs, t.id, "");
	if (R_FAILED(rc)) { hlog("%s: fsOpenBisFileSystem 0x%x", t.label, rc); return false; }
	// fsdev owns fs from here, also when the mount fails.
	if (fsdevMountDevice(t.dev, fs) == -1) { hlog("%s: mount failed", t.label); return false; }
	return true;
}

// Removes every file in <dev>:/Haku33_fill and the folder. Returns the number of files removed.
static int clear_fill_dir(const FillTarget &t) {
	char dirpath[64], path[400];
	snprintf(dirpath, sizeof(dirpath), "%s:/Haku33_fill", t.dev);
	int n = 0;
	DIR *d = opendir(dirpath);
	if (d != NULL) {
		// collect first: removing while reading a directory is not reliable
		std::vector<std::string> names;
		struct dirent *e;
		while ((e = readdir(d)) != NULL) {
			if (strcmp(e->d_name, ".") != 0 && strcmp(e->d_name, "..") != 0) names.push_back(e->d_name);
		}
		closedir(d);
		for (const std::string &name : names) {
			snprintf(path, sizeof(path), "%s/%s", dirpath, name.c_str());
			if (remove(path) == 0) n++;
		}
		rmdir(dirpath);
	}
	return n;
}

// Removes what prefill_free_space() left, for when the wipe does not start.
static void remove_prefill() {
	for (const FillTarget &t : FILL_TARGETS) {
		if (!mount_bis(t)) continue;
		int n = clear_fill_dir(t);
		if (n > 0) hlog("%s: removed %d overwrite file(s)", t.label, n);
		fsdevUnmountDevice(t.dev);
	}
}

static bool prefill_one(const FillTarget &t) {
	static u8 block[FILL_CHUNK]; // zero initialized
	if (!mount_bis(t)) return false;
	char dirpath[64], path[96];
	snprintf(dirpath, sizeof(dirpath), "%s:/Haku33_fill", t.dev);
	int old = clear_fill_dir(t); // left by an interrupted run
	if (old > 0) hlog("%s: removed %d leftover overwrite file(s)", t.label, old);
	mkdir(dirpath, 0777);

	s64 free_bytes = 0;
	Result rc = fsFsGetFreeSpace(fsdevGetDeviceFileSystem(t.dev), "/", &free_bytes);
	if (R_FAILED(rc)) {
		hlog("%s: fsFsGetFreeSpace 0x%x", t.label, rc);
		rmdir(dirpath);
		fsdevUnmountDevice(t.dev);
		return false;
	}
	u64 todo = (u64)free_bytes > FILL_MARGIN ? (u64)free_bytes - FILL_MARGIN : 0;
	const u64 total = todo;
	bool ok = true;
	time_t t0 = time(NULL);
	for (int idx = 0; todo > 0 && ok; idx++) {
		snprintf(path, sizeof(path), "%s/h%d", dirpath, idx);
		FILE *f = fopen(path, "wb");
		if (f == NULL) { ok = false; break; }
		setvbuf(f, NULL, _IONBF, 0);
		u64 in_file = todo < FILL_FILE_MAX ? todo : FILL_FILE_MAX;
		while (in_file > 0 && ok) {
			size_t n = in_file < FILL_CHUNK ? (size_t)in_file : FILL_CHUNK;
			if (fwrite(block, 1, n, f) != n) { ok = false; break; }
			in_file -= n; todo -= n;
			if (((total - todo) & ((64ULL << 20) - 1)) == 0 || todo == 0) {
				u64 mb = (total - todo) >> 20, el = (u64)(time(NULL) - t0);
				printf("\r\x1b[K  %s: %llu / %llu MB", t.label, (unsigned long long)mb, (unsigned long long)(total >> 20));
				if (el >= 5 && mb / el > 0) printf(", %llu MB/s, %llu min left", (unsigned long long)(mb / el), (unsigned long long)(((todo >> 20) / (mb / el) + 59) / 60));
				consoleUpdate(NULL);
			}
		}
		if (fclose(f) != 0) ok = false;
	}
	printf("\n");
	u64 secs = (u64)(time(NULL) - t0);
	u64 done_mb = (total - todo) >> 20;
	hlog("%s: %llu MB free, wrote %llu MB of zeros in %llu s (%llu MB/s)%s", t.label,
		(unsigned long long)((u64)free_bytes >> 20), (unsigned long long)done_mb, (unsigned long long)secs,
		(unsigned long long)(secs ? done_mb / secs : done_mb), ok ? "" : ", stopped early by a write error");
	fsdevUnmountDevice(t.dev);
	return ok;
}

static void prefill_free_space() {
	if (is_emummc) { hlog("running from emuMMC: no overwrite from Horizon, startup.te does all of it"); return; }
	printf(LG["STATUS3"]);
	consoleUpdate(NULL);
	appletSetAutoSleepDisabled(true); // this can take longer than the auto sleep time
	for (const FillTarget &t : FILL_TARGETS) {
		if (!prefill_one(t)) { printf(LG["WARNING5"], t.label); consoleUpdate(NULL); }
	}
}

// Starts the TegraExplorer wipe (romfs/startup.te). On success the console
// reboots or powers off and this does not return. Returns false, with the SD
// left as it was, when the wipe could not be started.
bool SetupClean (){
    led_on(2);
    hlog("TegraExplorer wipe: preparing the SD");
    if (!copy_me("romfs:/startup.te", "sdmc:/startup.te")) {
        hlog("could not copy startup.te to the SD, stopped");
        printf(LG["ERROR3"]);
        return false;
    }
    // Without this file the wipe still runs, startup.te then reports that the
    // free space was not overwritten.
    printf(LG["STATUS2"]);
    consoleUpdate(NULL);
    bool zero_ok = write_zero_file();
    hlog("zero file for the free space overwrite: %s", zero_ok ? "written" : "FAILED, free space will not be overwritten");
    if (!zero_ok) {
        printf(LG["WARNING4"]);
        consoleUpdate(NULL);
    }
    bool prefilled = false;
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

		hlog("patched console: boot files %s", ok ? "backed up and replaced" : "could NOT be prepared, restoring");
		if (ok && zero_ok) { prefill_free_space(); prefilled = true; }
		if (!ok) {
			restore_boot_files();
			remove_zero_file();
			remove("sdmc:/startup.te");
			printf(LG["ERROR3"]);
			return false;
		}

		hlog("powering off, TegraExplorer starts at the next power on");
		led_on(1);
		spsmInitialize();
		spsmShutdown(false);
		return true;
	}else{
        //Erista boot
		led_on(1);
		bpcInitialize();
		bool slp = init_slp();
		// Not before the reboot is known to be possible: the files would stay behind.
		if (slp && zero_ok) { prefill_free_space(); prefilled = true; }
		hlog("rebooting to TegraExplorer");
		if(slp){reboot_to_payload("romfs:/TegraExplorer.bin");}
		bpcExit();
		hlog("the reboot to TegraExplorer did not happen, wipe cancelled");
		if (prefilled) remove_prefill();
		// Only reached when the reboot did not happen: do not leave the wipe
		// script armed on the SD.
		remove_zero_file();
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
    hlog("index reset (A): fsOpen_SystemSaveData 0x%x", rc);
    if(R_FAILED(rc)) {
        printf("fsOpen_SystemSaveData() 0x%x\n", rc);
        return NewClean_NoAccess;
    }

    // fsdev owns dataW from here, also when the mount fails.
    if (fsdevMountDevice("bro", dataW) == -1) {
        hlog("index reset failed at: mount");
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
        hlog("index reset failed at: %s (0x%x), nothing was committed", failed, rc);
        printf(LG["ERROR5"], failed);
        return NewClean_Failed;
    }
    hlog("index reset committed (%u -> %u bytes), powering off", (unsigned)oldIndex.size(), (unsigned)newIndex.size());

    led_on(1);
	spsmInitialize();
	spsmShutdown(false);
    return NewClean_Done;
}

// The TegraExplorer wipe needs /switch/prod.keys on patched consoles
// (TegraExplorer cannot derive keys there).
void StartTegraWipe(bool keysok){
    if (is_patched && !keysok){
        hlog("TegraExplorer wipe refused: patched console without /switch/prod.keys");
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
    hlog("---- %s v%s started: firmware %s (0x%x), hardware %s, patched %d, emummc %d, prod.keys %d, services 0x%x",
        TITLE, VERSION, R_SUCCEEDED(rc) ? ver.display_version : "unknown", rc, spl::GetHardwareType(),
        (int)is_patched, (int)is_emummc, (int)keysok, init_rc);
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
			hlog("%s pressed once, asking to confirm", pressed == 1 ? "A" : "X/Y");
			printf(LG["CONFIRM1"]);
		}
		else if (pressed == 1){
			armed = 0;
			hlog("A confirmed");
			if (verM >= 17){
				if (is_emummc){
					//this would edit the emuMMC index, not sysNAND
					hlog("A refused: running on emuMMC");
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
			hlog("X/Y confirmed");
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
