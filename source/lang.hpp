#include <iostream>
#include <string>
#include <map>
#include <switch.h>
#include <unistd.h>
#include <inttypes.h>

// Define a map to hold the language translations
std::map<std::string, const char*> LG;

void set_LANG() {
    setInitialize();
    u64 lcode = 0;
    SetLanguage lang;
    setGetSystemLanguage(&lcode);
    setMakeLanguage(lcode, &lang);

    switch(lang) {
        case 5:
        case 14: // Spanish
            LG["WARNING1"] = "\n\x1b[30;1m TU CONSOLA SERA COMPLETAMENTE LIMPIADA: SAVES, JUEGOS, ETC  \x1b[0m\n";
            LG["WARNING2"] = "\n\x1b[30;1m SE REALIZARA UN HARD RESET LUEGO SE APAGARA LA CONSOLA \x1b[0m\n";
            LG["WARNING3"] = "\n\x1b[30;1m SI NO SABES LO QUE HACES, PRESIONA B PARA ABORTAR \x1b[0m\n";
            LG["MESSAGE1"] = "\n\n\x1b[30;1m-------- LO DEVORARE TODO --------\x1b[0m\n\n";
            LG["PROMPT1"] = "\n\nPULSA A PARA LIMPIAR\n\n";
            LG["REMINDER1"] = "\x1b[33;1m*\x1b[0m Recuerda Desinstalar Incognito Desde Incognito-RCM\n\n";
            LG["REMINDER2"] = "\x1b[33;1m*\x1b[0m Luego del Reinicio ve a Hekate -> More configs -> Haku33\n\n";
            LG["INFO0"] = " No se puede Reiniciar al Payload en una consola Mariko\n";
            LG["ERROR2"] = "\n\x1b[33;1m*\x1b[0m Si se congela mucho tiempo, Es que ha fallado. Pulsa POWER 15s \n\ny haslo de nuevo\n\n";
            LG["STATUS1"] = "\x1b[32;1m*\x1b[0m Deshabilitando FTP de SXOS\n";
            LG["INFO1"] = "\x1b[32;1m*\x1b[0m Esto no esta pensado para usarse en EMU\n";
            LG["ERROR1"] = "\x1b[32;1m*\x1b[0m NO Existe /switch/prod.keys, usa LockPick_RCM\n";
            LG["CONFIRM1"] = "\x1b[33;1m*\x1b[0m ESTAS SEGURO??\n";
            break;
        default: // English (default)
            LG["WARNING1"] = "\n\x1b[30;1m YOUR CONSOLE WILL BE COMPLETELY CLEANED: SAVES, GAMES, ETC  \x1b[0m\n";
            LG["WARNING2"] = "\n\x1b[30;1m A HARD RESET WILL BE PERFORMED AFTER THE CONSOLE WILL BE OFF \x1b[0m\n";
            LG["WARNING3"] = "\n\x1b[30;1m IF YOU DON'T KNOW WHAT YOU DO, PRESS B FOR ABORT \x1b[0m\n";
            LG["MESSAGE1"] = "\n\n\x1b[30;1m-------- I WILL CONSUME EVERYTHING --------\x1b[0m\n\n";
            LG["PROMPT1"] = "\n\nPRESS A TO CLEAN\n\n";
            LG["REMINDER1"] = "\x1b[33;1m*\x1b[0m Remember Uninstall Incognito from Incognito-RCM\n\n";
            LG["REMINDER2"] = "\x1b[33;1m*\x1b[0m After the Reboot go to Hekate -> More configs -> Haku33\n\n";
            LG["INFO0"] = " Reboot to payload cannot be used on a Mariko system\n";
            LG["ERROR2"] = "\n\x1b[33;1m*\x1b[0m If it freezes for a long time, It has failed. Press POWER 15s \n\nand try again \n\n";
            LG["STATUS1"] = "\x1b[32;1m*\x1b[0m Disabling SXOS FTP\n";
            LG["INFO1"] = "\x1b[32;1m*\x1b[0m This is not intended to be used at EMU\n";
            LG["ERROR1"] = "\x1b[32;1m*\x1b[0m NO Existe /switch/prod.keys, usa LockPick_RCM\n";
            LG["CONFIRM1"] = "\x1b[33;1m*\x1b[0m ARE YOU SURE??\n";
            break;
    }
    setsysExit();
}
