#include <3ds.h>
#include <stdio.h>
#include "vyron_config.h"

static void draw_page(PrintConsole *top, PrintConsole *bottom, int page) {
    consoleSelect(top);
    consoleClear();
    printf("%s v%d.%d.%d\n", XAVRX_PROJECT_NAME,
           XAVRX_VERSION_MAJOR, XAVRX_VERSION_MINOR, XAVRX_VERSION_PATCH);
    printf("================================\n");

    switch (page) {
    case 0:
        printf("        PROJECT STATUS\n\n");
        printf("Vita ELF/SELF inspector\n");
        printf("Build target: Nintendo 3DS\n\n");
        printf("[OK] ELF32 ARM header checks\n");
        printf("[OK] SELF public header checks\n");
        printf("[OK] AppInfo and segment metadata\n");
        printf("[OK] Host test suite\n");
        printf("[OK] 3DSX CI build\n");
        break;
    case 1:
        printf("       ELF INSPECTION\n\n");
        printf("Checks performed:\n");
        printf("- ELF magic and version\n");
        printf("- 32-bit little-endian format\n");
        printf("- ARM machine identifier\n");
        printf("- Program/section table bounds\n");
        printf("- PT_LOAD range and alignment\n");
        printf("- Vita executable type limits\n");
        break;
    case 2:
        printf("       SELF INSPECTION\n\n");
        printf("Public metadata inspected:\n");
        printf("- SCE header (0x80 bytes)\n");
        printf("- Embedded ELF range\n");
        printf("- AppInfo record (0x20 bytes)\n");
        printf("- Segment offsets and lengths\n");
        printf("- Compression/encryption flags\n");
        break;
    default:
        printf("       PROJECT LIMITS\n\n");
        printf("This is NOT a Vita emulator.\n");
        printf("No game execution or compatibility.\n\n");
        printf("Not implemented:\n");
        printf("- SELF decryption/signature checks\n");
        printf("- Vita OS/kernel services\n");
        printf("- ARMv7 CPU or Vita GPU emulation\n");
        printf("- Loading or executing Vita games\n");
        break;
    }

    consoleSelect(bottom);
    consoleClear();
    printf("XAVRX PS VITA 3DS - MENU\n");
    printf("--------------------------------\n");
    printf("LEFT / RIGHT : Change page\n");
    printf("START        : Exit\n\n");
    printf("Page %d / 4\n", page + 1);
    printf("Metadata-only research build.\n");
}

int main(void) {
    PrintConsole topScreen, bottomScreen;
    int page = 0;
    int redraw = 1;

    gfxInitDefault();
    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);

    while (aptMainLoop()) {
        hidScanInput();
        u32 down = hidKeysDown();

        if (down & KEY_START)
            break;
        if (down & KEY_RIGHT) {
            page = (page + 1) % 4;
            redraw = 1;
        } else if (down & KEY_LEFT) {
            page = (page + 3) % 4;
            redraw = 1;
        }

        if (redraw) {
            draw_page(&topScreen, &bottomScreen, page);
            redraw = 0;
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
