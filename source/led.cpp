#include <string.h>
#include <switch.h>
#include <stdio.h>

void led_on(int inter)
{
    // Configure our supported input layout: a single player with standard controller styles

    Result rc=0;
    s32 i;
    s32 total_entries;
    HidsysUniquePadId unique_pad_ids[2]={0};
    HidsysNotificationLedPattern pattern;

    rc = hidsysInitialize();
    if (R_FAILED(rc)) {
        printf("hidsysInitialize(): 0x%x\n", rc);
    }
	// Scan the gamepad. This should be done once for each frame
	PadState pad;
	padInitializeDefault(&pad);
	padUpdate(&pad);

    // padGetButtonsDown returns the set of buttons that have been
    // newly pressed in this frame compared to the previous one
	//u64 kDown = padGetButtonsDown(&pad);
	
	switch(inter)
	{
		case 1:

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
	
			break;
			
			
		case 2:
				memset(&pattern, 0, sizeof(pattern));
				// Setup Heartbeat effect pattern data.
				pattern.baseMiniCycleDuration = 0x1;             // 12.5ms.
				pattern.totalMiniCycles = 0xF;                   // 16 mini cycles.
				pattern.totalFullCycles = 0x0;                   // Repeat forever.
				pattern.startIntensity = 0x0;                    // 0%.

				// First beat.
				pattern.miniCycles[0].ledIntensity = 0xF;        // 100%.
				pattern.miniCycles[0].transitionSteps = 0xF;     // 15 steps. Total 187.5ms.
				pattern.miniCycles[0].finalStepDuration = 0x0;   // Forced 12.5ms.
				pattern.miniCycles[1].ledIntensity = 0x0;        // 0%.
				pattern.miniCycles[1].transitionSteps = 0xF;     // 15 steps. Total 187.5ms.
				pattern.miniCycles[1].finalStepDuration = 0x0;   // Forced 12.5ms.

				// Second beat.
				pattern.miniCycles[2].ledIntensity = 0xF;
				pattern.miniCycles[2].transitionSteps = 0xF;
				pattern.miniCycles[2].finalStepDuration = 0x0;
				pattern.miniCycles[3].ledIntensity = 0x0;
				pattern.miniCycles[3].transitionSteps = 0xF;
				pattern.miniCycles[3].finalStepDuration = 0x0;

				// Led off wait time.
				for(i=2; i<4; i++) {
					pattern.miniCycles[i].ledIntensity = 0x0;        // 0%.
					pattern.miniCycles[i].transitionSteps = 0xF;     // 15 steps. Total 187.5ms.
					pattern.miniCycles[i].finalStepDuration = 0xF;   // 187.5ms.
				}
			break;
	
		case 3:
				memset(&pattern, 0, sizeof(pattern));
				// Setup Beacon effect pattern data.
				pattern.baseMiniCycleDuration = 0x1;             // 12.5ms.
				pattern.totalMiniCycles = 0xF;                   // 16 mini cycles.
				pattern.totalFullCycles = 0x0;                   // Repeat forever.
				pattern.startIntensity = 0x0;                    // 0%.

				// First beat.
				pattern.miniCycles[0].ledIntensity = 0xF;        // 100%.
				pattern.miniCycles[0].transitionSteps = 0xF;     // 15 steps. Total 187.5ms.
				pattern.miniCycles[0].finalStepDuration = 0x0;   // Forced 12.5ms.
				pattern.miniCycles[1].ledIntensity = 0x0;        // 0%.
				pattern.miniCycles[1].transitionSteps = 0xF;     // 15 steps. Total 187.5ms.
				pattern.miniCycles[1].finalStepDuration = 0x0;   // Forced 12.5ms.

				// Second beat.
				pattern.miniCycles[2].ledIntensity = 0xF;
				pattern.miniCycles[2].transitionSteps = 0xF;
				pattern.miniCycles[2].finalStepDuration = 0x0;
				pattern.miniCycles[3].ledIntensity = 0x0;
				pattern.miniCycles[3].transitionSteps = 0xF;
				pattern.miniCycles[3].finalStepDuration = 0x0;

				// Led off wait time.
				for(i=4; i<4; i++) {
					pattern.miniCycles[i].ledIntensity = 0x0;        // 0%.
					pattern.miniCycles[i].transitionSteps = 0xF;     // 15 steps. Total 187.5ms.
					pattern.miniCycles[i].finalStepDuration = 0xF;   // 187.5ms.
				}
			break;
		case 0:
		default:
				memset(&pattern, 0, sizeof(pattern));
			break;	
	}

	        total_entries = 0;
            memset(unique_pad_ids, 0, sizeof(unique_pad_ids));

            // Get the UniquePadIds for the specified controller, which will then be used with hidsysSetNotificationLedPattern*.
            // If you want to get the UniquePadIds for all controllers, you can use hidsysGetUniquePadIds instead.
            rc = hidsysGetUniquePadsFromNpad(padIsHandheld(&pad) ? HidNpadIdType_Handheld : HidNpadIdType_No1, unique_pad_ids, 2, &total_entries);
            printf("hidsysGetUniquePadsFromNpad(): 0x%x", rc);
            if (R_SUCCEEDED(rc)) printf(", %d", total_entries);
            printf("\n");

            if (R_SUCCEEDED(rc)) {
                for(i=0; i<total_entries; i++) { // System will skip sending the subcommand to controllers where this isn't available.
                    printf("[%d] = 0x%lx ", i, unique_pad_ids[i].id);

                    // Attempt to use hidsysSetNotificationLedPatternWithTimeout first with a 2 second timeout, then fallback to hidsysSetNotificationLedPattern on failure. See hidsys.h for the requirements for using these.
                     rc = hidsysSetNotificationLedPatternWithTimeout(&pattern, unique_pad_ids[i], 10000000000ULL);
					   printf("hidsysSetNotificationLedPatternWithTimeout(): 0x%x\n", rc);
                    if (R_FAILED(rc)) {
                        rc = hidsysSetNotificationLedPattern(&pattern, unique_pad_ids[i]);
                        printf("hidsysSetNotificationLedPattern(): 0x%x\n", rc);
                    }
                }
            }
        
}

void flash_led_connect()
{
	led_on(1);
}

void flash_led_disconnect()
{
	led_on(0);
}

