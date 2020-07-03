#include "common.h"
#include "types.h"

menu_entry mainmenu_main[] = {
    {"[SD:/] SD CARD", COLOR_GREEN, ISMENU},
    {"[EMMC:/] EMMC", COLOR_ORANGE, ISMENU},
    {"[EMMC:/] EMUMMC", COLOR_BLUE, ISMENU},
    {"\nMount/Unmount SD", COLOR_WHITE, ISMENU},
    {"Herramientas", COLOR_VIOLET, ISMENU},
    {"Formatear SD", COLOR_VIOLET, ISMENU},
    {"\nCreditos", COLOR_WHITE, ISMENU},
    {"Exit", COLOR_WHITE, ISMENU}
};

menu_entry mainmenu_shutdown[] = {
    {"Atras", COLOR_WHITE, ISMENU},
    {"\nReiniciar to RCM", COLOR_VIOLET, ISMENU},
    {"Reiniciar normal", COLOR_ORANGE, ISMENU},
    {"Apagar\n", COLOR_BLUE, ISMENU},
    {"Reiniciar a Hekate", COLOR_GREEN, ISMENU},
    {"Reiniciar a StarDust", COLOR_GREEN, ISMENU}
};

menu_entry mainmenu_tools[] = {
    {"Atras", COLOR_WHITE, ISMENU},
    {"\nDisplay Console Info", COLOR_GREEN, ISMENU},
    {"Display GPIO pins", COLOR_VIOLET, ISMENU},
    {"Dumpear Firmware", COLOR_BLUE, ISMENU},
    {"Dumpear Saves", COLOR_YELLOW, ISMENU}
};

menu_entry mainmenu_format[] = {
    {"Atras\n", COLOR_WHITE, ISMENU},
    {"Formatear SD en FAT32", COLOR_RED, ISMENU},
    {"Formatear para Crear EmuMMC (FAT32/RAW)", COLOR_RED, ISMENU}
};

menu_entry utils_mmcChoice[] = {
    {"Atras\n", COLOR_WHITE, ISMENU},
    {"SysMMC", COLOR_ORANGE, ISMENU},
    {"EmuMMC", COLOR_BLUE, ISMENU}
};

menu_entry fs_menu_file[] = {
    {NULL, COLOR_GREEN, ISMENU | ISSKIP},
    {NULL, COLOR_VIOLET, ISMENU | ISSKIP},
    {NULL, COLOR_VIOLET, ISMENU | ISSKIP},
    {"\n\n\nAtras", COLOR_WHITE, ISMENU},
    {"\nCopiar", COLOR_BLUE, ISMENU},
    {"Mover", COLOR_BLUE, ISMENU},
    {"Renombrar", COLOR_BLUE, ISMENU},
    {"\nBorrar fichero\n", COLOR_RED, ISMENU},
    {"Lanzar Payload", COLOR_ORANGE, ISMENU},
    {"Lanzar Script", COLOR_YELLOW, ISMENU},
    {"Visor Hex", COLOR_GREEN, ISMENU},
    {"\nExtraer BIS", COLOR_YELLOW, ISMENU},
    {"Sign Save", COLOR_ORANGE, {ISMENU}}

};

menu_entry fs_menu_folder[] = {
    {NULL, COLOR_VIOLET, ISMENU | ISSKIP},
    {"\nAtras", COLOR_WHITE, ISMENU},
    {"Volver al menu principal\n", COLOR_BLUE, ISMENU},
    {"Copiar", COLOR_VIOLET, ISMENU},
    {"Borrar Carpeta\n", COLOR_RED, ISMENU},
    {"Renombrar Carpeta", COLOR_BLUE, ISMENU},
    {"Crear Carpeta", COLOR_BLUE, ISMENU}
};

menu_entry fs_menu_startdir[] = {
    {"Folder -> previous folder  ", COLOR_ORANGE, ISMENU},
    {"Clipboard -> Current folder", COLOR_ORANGE, ISMENU},
    {"Current folder menu        ", COLOR_ORANGE, ISMENU}
};

gpt_entry_rule gpt_fs_rules[] = {
    {"PRODINFOF", 0 | isFS},
    {"SAFE", 1 | isFS },
    {"SYSTEM", 2 | isFS},
    {"USER", 3 | isFS},
    {NULL, 0}
};

menu_entry mmcmenu_start[] = {
    {"Atras", COLOR_ORANGE, ISMENU},
    {"Dumpear Fichero Partision", COLOR_ORANGE, ISMENU},
    {"Clipboard -> Partition\n", COLOR_ORANGE, ISMENU},
    {"BOOT0/1", COLOR_BLUE, isBOOT | ISMENU}
};

menu_entry mmcmenu_filemenu[] = {
    {"Part:", COLOR_ORANGE, ISSKIP | ISMENU},
    {NULL, COLOR_VIOLET, ISSKIP | ISMENU},
    {"\nAtras", COLOR_WHITE, ISMENU},
    {"Dumpear a la SD", COLOR_YELLOW, ISMENU}
};