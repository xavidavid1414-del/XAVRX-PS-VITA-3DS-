#include <3ds.h>
#include <stdio.h>
#include "vyron_config.h"
int main(void) {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    printf("%s v%d.%d.%d\\n", XAVRX_PROJECT_NAME, XAVRX_VERSION_MAJOR, XAVRX_VERSION_MINOR, XAVRX_VERSION_PATCH);
    printf("Milestone 1: ELF32 ARM header parser\\nParser module added; host tests run in CI.\\n\\n");
    printf("Not implemented: SELF decryption, Vita services, ARMv7 execution, GPU emulation.\\n");
    printf("This prototype does NOT run Vita games.\\nPress START to exit.\\n");
    while (aptMainLoop()) { hidScanInput(); if (hidKeysDown() & KEY_START) break; gfxFlushBuffers(); gfxSwapBuffers(); gspWaitForVBlank(); }
    gfxExit(); return 0;
}