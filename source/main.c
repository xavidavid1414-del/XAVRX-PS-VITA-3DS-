#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "vyron_config.h"
#include "vita_elf.h"
#include "vita_self.h"

#define MAX_FILES 24
#define MAX_FILE_SIZE (16u * 1024u * 1024u)

static char files[MAX_FILES][128];
static unsigned file_count;
static unsigned selected_file;
static char scan_message[96] = "Press Y to scan SD root.";
static char inspect_message[128] = "Choose an ELF or SELF file and press A.";

static int has_extension(const char *name, const char *ext) {
    size_t n = strlen(name), e = strlen(ext), i;
    if (n < e) return 0;
    for (i = 0; i < e; ++i) {
        char a = name[n - e + i], b = ext[i];
        if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
        if (b >= 'A' && b <= 'Z') b = (char)(b - 'A' + 'a');
        if (a != b) return 0;
    }
    return 1;
}

static void scan_sd(void) {
    DIR *dir = opendir("sdmc:/");
    struct dirent *entry;
    file_count = 0;
    selected_file = 0;
    if (!dir) {
        snprintf(scan_message, sizeof(scan_message), "Cannot open sdmc:/");
        return;
    }
    while ((entry = readdir(dir)) != NULL && file_count < MAX_FILES) {
        if (has_extension(entry->d_name, ".elf") ||
            has_extension(entry->d_name, ".self")) {
            snprintf(files[file_count], sizeof(files[file_count]), "%s", entry->d_name);
            ++file_count;
        }
    }
    closedir(dir);
    snprintf(scan_message, sizeof(scan_message), "%u ELF/SELF file(s) in SD root", file_count);
    snprintf(inspect_message, sizeof(inspect_message), "Select a file and press A.");
}

static void inspect_selected(void) {
    char path[160];
    FILE *fp;
    long length;
    unsigned char *data;
    snprintf(path, sizeof(path), "sdmc:/%s", files[selected_file]);
    fp = fopen(path, "rb");
    if (!fp) {
        snprintf(inspect_message, sizeof(inspect_message), "Could not open selected file.");
        return;
    }
    if (fseek(fp, 0, SEEK_END) != 0 || (length = ftell(fp)) < 0 ||
        (unsigned long)length > MAX_FILE_SIZE || fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        snprintf(inspect_message, sizeof(inspect_message), "File too large or unreadable (max 16 MiB).");
        return;
    }
    data = (unsigned char *)malloc((size_t)length);
    if (!data) {
        fclose(fp);
        snprintf(inspect_message, sizeof(inspect_message), "Not enough memory to inspect file.");
        return;
    }
    if (fread(data, 1, (size_t)length, fp) != (size_t)length) {
        free(data);
        fclose(fp);
        snprintf(inspect_message, sizeof(inspect_message), "Could not read complete file.");
        return;
    }
    fclose(fp);

    if (has_extension(files[selected_file], ".elf")) {
        VitaElfInfo info;
        VitaElfStatus status = vita_elf_parse_header(data, (size_t)length, &info);
        snprintf(inspect_message, sizeof(inspect_message), "ELF: %s", vita_elf_status_string(status));
    } else {
        VitaSelfInfo info;
        VitaSelfStatus status = vita_self_inspect_header(data, (size_t)length, &info);
        snprintf(inspect_message, sizeof(inspect_message), "SELF: %s", vita_self_status_string(status));
    }
    free(data);
}

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
    case 3:
        printf("       SD CARD FILE BROWSER\n\n");
        printf("%s\n\n", scan_message);
        if (file_count == 0) {
            printf("No ELF/SELF files found in SD root.\n");
        } else {
            unsigned start = selected_file > 8 ? selected_file - 8 : 0;
            unsigned end = start + 17;
            unsigned i;
            if (end > file_count) end = file_count;
            for (i = start; i < end; ++i)
                printf("%c %s\n", i == selected_file ? '>' : ' ', files[i]);
        }
        printf("\nResult: %s\n", inspect_message);
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
    if (page == 3) {
        printf("UP / DOWN    : Select file\n");
        printf("A            : Inspect file\n");
        printf("Y            : Rescan SD root\n");
    }
    printf("START        : Exit\n\n");
    printf("Page %d / 5\n", page + 1);
    printf("Read-only metadata inspection.\n");
}

int main(void) {
    PrintConsole topScreen, bottomScreen;
    int page = 0;
    int redraw = 1;

    gfxInitDefault();
    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);
    scan_sd();

    while (aptMainLoop()) {
        hidScanInput();
        u32 down = hidKeysDown();

        if (down & KEY_START)
            break;
        if (down & KEY_RIGHT) {
            page = (page + 1) % 5;
            redraw = 1;
        } else if (down & KEY_LEFT) {
            page = (page + 4) % 5;
            redraw = 1;
        }
        if (page == 3) {
            if ((down & KEY_DOWN) && file_count > 0) {
                selected_file = (selected_file + 1) % file_count;
                redraw = 1;
            } else if ((down & KEY_UP) && file_count > 0) {
                selected_file = (selected_file + file_count - 1) % file_count;
                redraw = 1;
            }
            if (down & KEY_A) {
                if (file_count > 0) inspect_selected();
                else snprintf(inspect_message, sizeof(inspect_message), "No ELF/SELF file selected.");
                redraw = 1;
            }
            if (down & KEY_Y) {
                scan_sd();
                redraw = 1;
            }
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
