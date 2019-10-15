#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <switch.h>

void reboot_to_payload(void);
bool init_slp(void);
void exit_spl(bool can_reboot);
