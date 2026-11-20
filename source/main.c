#include <3ds.h>
#include <stdio.h>

int main(void)
{
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    printf("XAVRX PS VITA 3DS\n");
    printf("-----------------\n");
    printf("Experimental research prototype\n\n");
    printf("Milestone 0: libctru startup\n");
    printf("Graphics: initialized\n");
    printf("Input: START to exit\n\n");
    printf("This build does NOT run PS Vita games.\n");
    printf("Full Vita emulation is not practical\n");
    printf("on New Nintendo 2DS XL hardware.\n");

    while (aptMainLoop()) {
        hidScanInput();
        const u32 keys = hidKeysDown();

        if (keys & KEY_START) {
            break;
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
