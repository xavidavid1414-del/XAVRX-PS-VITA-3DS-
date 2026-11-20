#include <3ds.h>
#include <stdio.h>
#include "vyron_config.h"
int main(void) {
    gfxInitDefault(); consoleInit(GFX_TOP, NULL);
    printf("%s v%d.%d.%d\n", XAVRX_PROJECT_NAME, XAVRX_VERSION_MAJOR, XAVRX_VERSION_MINOR, XAVRX_VERSION_PATCH);
    printf("Milestone 2: ELF32 ARM validation\nParser checks header fields and table bounds.\n\n");
    printf("Not implemented: SELF decryption, Vita services, ARMv7 execution, GPU emulation.\n");
    printf("This prototype does NOT run Vita games.\nPress START to exit.\n");
    while (aptMainLoop()) {
        hidScanInput(); if (hidKeysDown() & KEY_START) break;
        gfxFlushBuffers(); gfxSwapBuffers(); gspWaitForVBlank();
    }
    gfxExit(); return 0;
}