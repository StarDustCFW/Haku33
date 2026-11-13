# Code review findings and fixes

Review of Haku33 at upstream commit `e5fb80e` ("21.0.0", version 7.2).
Line numbers below refer to that commit.

This branch builds with devkitA64 and was run on one console, an original
(Erista) Switch on firmware 23.0.1. The patched / Mariko code paths have **not**
been run on hardware. What was checked, and how, is listed under
[Verification](#verification).

## Findings

| # | Finding | Where (at `e5fb80e`) | In this branch |
|---|---------|----------------------|----------------|
| 1 | On 17.0.0+ the A button only replaces the save index and powers off. No save file, no installed content and nothing on USER is deleted; the system is only told to forget them. The full wipe (`startup.te`) is reached only through X/Y. | `main.cpp:193-218`, `308-309` | **Not changed** (design decision, see below). X is now shown on screen as the full wipe. |
| 2 | The canned index `romfs/imkvdb.arc` holds one record (`8000000000000120`) with a hardcoded size of `0x128000`. A retail console's own index records `0x118000` for that save, and also holds a record for `8000000000000053` (DeviceSettings, battery calibration). `startup.te` keeps 0053 on purpose and warns that losing it is unsafe on Lite. | `romfs/imkvdb.arc`, `main.cpp:200` | Fixed. The new index is built from the console's own records for 0120 and 0053 (`source/imkvdb.hpp`); `romfs/imkvdb.arc` is removed. |
| 3 | X or Y calls `SetupClean()` at once: no confirmation, no `prod.keys` check, not mentioned on screen. On patched consoles this overwrites SD boot files and powers off. Without `prod.keys` the script then stops at `#REQUIRE KEYS`, before it can restore `payload.bin`. | `main.cpp:292-294` | Fixed. X/Y need the same second press as A, and `prod.keys` on patched consoles. |
| 4 | Patched consoles: `boot.dat` and `boot.ini` on the SD are overwritten with no backup and never restored. | `main.cpp:174-175` | Fixed. Originals go to `sd:/Haku33_backup/` and `startup.te` puts them back. |
| 5 | `romfs/boot.ini` is in `.gitignore`, so it is in the 7.2 release but not in the repository. A build from source (or CI) has no such file, and `copy_me()` then truncates the user's `sd:/boot.ini` to 0 bytes, because it opens the destination before checking the source. | `.gitignore:13`, `main.cpp:122-128` | Fixed. `boot.ini` (taken from the 7.2 release) is added, and `copy_me()` no longer touches the destination when the source is missing. |
| 6 | `payload.bin` backup: with no `payload.bin` on the SD a 0 byte `.bak` is created and later "restored"; a second run after a failed one overwrites the good `.bak` with TegraExplorer. No copy is checked before the console powers off. | `main.cpp:178-183` | Fixed. A backup is taken once, a missing file is recorded with a marker, and nothing powers off unless every copy succeeded. |
| 7 | `startup.te`: "Exit" in the first menu does not restore `payload.bin` (the later abort path does). No abort path deletes `sd:/startup.te`, so the wiper starts again whenever TegraExplorer is launched from that SD. The restore deletes the backup even if the copy failed. | `startup.te:5`, `12`, `97-101` | Fixed. |
| 8 | `startup.te` renames `sd:/Nintendo` even when "Wipe emummc" was chosen, and never says the folder was renamed instead of deleted. | `startup.te:222-223` | Fixed. Only for sysmmc, with a message. |
| 9 | `AccountUid uid` is passed uninitialized to `fsOpen_SystemSaveData()`. | `main.cpp:195` | Fixed. |
| 10 | `fsFsClose(&dataW)` runs after `fsdevUnmountDevice()` already closed it (double close), and on an uninitialized struct when the open failed. Results of mount, copy and commit are ignored and "Data Cleared" is printed regardless. | `main.cpp:198-213` | Fixed. Each step is checked; on any failure nothing is committed and the console is not powered off. |
| 11 | No emuMMC check: the A path edits whichever NAND Horizon is running from. `spl::HasEmummc()` and the "not intended to be used at EMU" string exist but are never used. | `main.cpp`, `spl.cpp:67`, `lang.hpp:46` | Fixed. A is refused on emuMMC on 17.0.0+ and the message is shown. |
| 12 | "ARE YOU SURE??" is printed every frame once A was pressed, scrolling the warnings off screen. | `main.cpp:286-288` | Fixed. Printed once. |
| 13 | Without `/switch/prod.keys` A can never proceed, including on paths that do not need keys (17.0.0+ index path, unpatched consoles). The English message for it is in Spanish. | `main.cpp:307`, `lang.hpp:47` | Fixed. Keys are required only for the TegraExplorer path on patched consoles. |
| 14 | `spl::GetHardwareType() == "Iowa"` compares pointers. It works only while the linker merges identical string literals. "Aula" (OLED) is not in the list. | `main.cpp:163` | Fixed (`strcmp`, Aula added). |
| 15 | `HasRCMbugPatched()` and `HasEmummc()` return an uninitialized value when `splGetConfig()` fails. `GetHardwareType()` indexes its table without a bounds check. | `spl.cpp:45-79` | Fixed. |
| 16 | `reboot_to_payload()` clears IRAM before checking that the payload file opens, and a failed reboot shows nothing. `startup.te` stays on the SD. | `reboot.c:45-56`, `main.cpp:186-189` | Fixed. |
| 17 | `Init_Services()` result is ignored; the firmware version error is printed before `consoleClear()` so it is never seen. | `main.cpp:226`, `235-241` | Fixed. |
| 18 | `setInitialize()` is paired with `setsysExit()`; `lang` is used uninitialized if the language query fails. `close_Services()` calls `smExit()` with no matching init. | `lang.hpp:12-16`, `51`, `main.cpp:109` | Fixed. |
| 19 | Dead code: unused 147 KB `g_reboot_payload`, `Logs` (printed, never written), `isXSOS`, `getindex()`, the ZL+ZR+Plus+Minus combo that is computed and never used. | `main.cpp` | Removed. |
| 20 | `led_on()`: `hidsysInitialize()` on every call with no exit; in pattern 2 the "off" loop overwrites the second beat (`i=2`), in pattern 3 it never runs (`i=4; i<4`); debug `printf`s reach the user's screen. | `led.cpp:15`, `74`, `106`, `124-137` | **Not changed** (cosmetic, cannot be checked without hardware). |
| 21 | `reboot.c` copies `0x2F000` bytes to IRAM. Atmosphère's current `reboot_to_payload` uses `0x24000` and `amsBpcSetRebootPayload`. The bundled payload (124954 bytes) fits either way. | `reboot.c:8` | **Not changed.** |
| 23 | Found during the hardware test: the 17.0.0+ A path leaves `sd:/Nintendo` in place. After the index reset the console no longer recognises that folder, and on the next boot with the SD inserted the system deleted data on the card. `startup.te` avoids this by renaming the folder first. | `main.cpp:193-218` | **Not changed.** The folder may be in use by Horizon while Haku33 runs, so a rename from the app needs its own test. Until then, remove the SD card before the first boot after A, or use X. |
| 24 | Found by comparing NAND dumps taken before and after the wipe on the test console: the wipe only deletes directory entries. After it, 99.6% of the old USER save data (563 of 566 MB, all 15 game saves) was still intact in free space. On SYSTEM about 1 GB of old save data sat inside the saves the console recreated, because a new save reuses the same clusters and only writes the parts it needs, and another 109 MB was in free space. Three empty pre-wipe folders also remained in SYSTEM `saveMeta`. None of it is visible to the console; it is readable from a raw dump with the console keys. | `startup.te` | Fixed, **not yet run on a console**. Haku33 writes a 128 MB file of zeros to the SD and `startup.te` copies it to SYSTEM and USER until each is full, then removes the copies; SYSTEM `saveMeta` is deleted and recreated. Adds several minutes to the wipe. The names of deleted saves in the SYSTEM save folder are not cleared. |
| 22 | `stuff/Haku33.te` is an unused script in the old TegraExplorer syntax. The README says emuMMC is not supported while `startup.te` offers "Wipe emummc". The workflow pins `actions/checkout@v1` and `actions/upload-artifact@master`. | `stuff/`, `README.md`, `.github/workflows/Homebrew.yml` | **Not changed.** |

### About finding 1

`NewClean()` is kept as the 17.0.0+ default because that is the author's
design and I cannot test an alternative on hardware. With this branch it writes
the same index `startup.te` would write, but it still deletes nothing. If "as
if it had just left the box" is the goal on 17.0.0+, the TegraExplorer path (X)
is the one that removes the data. Whether Horizon later cleans up the
unindexed save files and content on its own is something only a test on a
console can answer.

## Behaviour changes to be aware of

- X and Y now ask for a second press, like A, and are shown on screen.
- On patched consoles the SD gets a `Haku33_backup` folder while the wipe is
  pending. `startup.te` restores from it and removes it. A backup that could
  not be copied back is left in place and reported.
- The TegraExplorer wipe now overwrites the free space of SYSTEM and USER, which
  takes several minutes on USER. If it is interrupted the partition is left
  full; running the wipe again removes the leftover files first. If the SD has
  no room for the 128 MB block, the wipe still runs and says at the end that
  free space was not overwritten.
- Backing out of `startup.te` (first menu, or any key other than power at the
  prompt) now also deletes `sd:/startup.te`. Errors keep it so the wipe can be
  retried.
- A is refused on emuMMC on 17.0.0+.
- If the index cannot be read, parsed, written or committed, nothing is
  changed and the console stays on.

## Verification

On a console (original Erista Switch, firmware 23.0.1, Atmosphère on sysMMC,
built with devkitA64, bundled TegraExplorer reports 4.2.0):

- X pressed once only prints the confirmation, once. A second press reboots
  into TegraExplorer and starts `startup.te`.
- Backing out in the first menu, and backing out at the power-button prompt,
  both return to TegraExplorer with `startup.te` removed from the SD card.
- A pressed twice: the index is rebuilt and the console powers off. The next
  boot, on stock firmware, reaches first-time setup. See finding 23 for what
  happened to the SD card on that boot.
- X pressed twice, "Wipe sysmmc", power button: the script runs to "Done" and
  powers off. The next boot, on stock firmware with no SD card, starts normally.

Done on a PC:

- **Type check**: every changed source parses with `g++ -fsyntax-only` against
  the current libnx headers, with no errors.
- **`main.cpp` logic**: the file was compiled for the host and linked against
  stand-ins for the libnx calls, with `sdmc:/`, `romfs:/` and the mounted save
  mapped to real directories (the save with commit/rollback behaviour). 14
  scenarios pass under ASan/UBSan: SD preparation with and without existing
  boot files, a second run before a restore, a build without `boot.ini`, an
  empty `payload.bin`, a failed reboot, and the index rebuild on good, damaged
  and incomplete indexes, a failed commit and a save that cannot be opened.
- **`imkvdb.hpp`**: run on the full index (99 records) of a NAND dump from a
  retail console. The output is byte for byte what `startup.te` builds for that
  console (2 records, 292 bytes). 500,000 randomly damaged inputs produce no
  crash or sanitizer report.
- **`startup.te`**: run through TegraExplorer's own script interpreter
  (`source/script`, host build, with `intToStr` made to behave as on the
  console) with the hardware functions replaced by an in-memory SD/eMMC that
  tracks file sizes and free space. Seven paths were run: Exit, declined prompt, full sysmmc
  wipe, full emummc wipe, a failed restore copy, an unpatched console with its
  own `payload.bin`, and a mount failure. The same harness reproduces finding
  7 with the original script.
- The SD layout written by the C++ code was fed to the script harness: the
  script's Exit path returns the SD to its original contents.

Not verified:

- The free space overwrite (finding 24) on a console. In the script harness it
  was run with the partition filling mid-block and exactly on a block boundary,
  with a filesystem that reports a full disk as an error and one that leaves a
  short file, with the zero file missing, with leftovers of an interrupted run,
  and with the copies failing to delete. How long it takes on real eMMC, and
  TegraExplorer's behaviour when a partition really fills up, need hardware.
- Anything on a patched or Mariko console: the SD boot file backup and restore
  (findings 4 and 6) has only been run on a PC.
- The emuMMC refusal, the "Wipe emummc" target, and the error paths.
- What Horizon does over time with the files the A path leaves behind.
- The save index part of `startup.te` (unchanged here) was not run in the
  harness, because the host build of the interpreter has no save support.
- The bundled `TegraExplorer.bin` and `boot.dat` are binaries with no recorded
  version or source; they were not examined beyond their headers.
