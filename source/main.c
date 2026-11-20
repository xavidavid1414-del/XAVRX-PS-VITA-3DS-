#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "vyron_config.h"
#include "vita_elf.h"
#include "vita_self.h"

#define MAX_FILES 24
#define MAX_NAME 128
#define MAX_PATH 256
#define MAX_FILE_SIZE (16u * 1024u * 1024u)

typedef struct {
    char name[MAX_NAME];
    int is_directory;
} BrowserEntry;

static BrowserEntry entries[MAX_FILES];
static unsigned entry_count;
static unsigned total_entries;
static unsigned selected_entry;
static char current_dir[MAX_PATH] = "sdmc:/";
static char scan_message[96] = "Scanning SD card...";
static char inspect_message[128] = "Select a file and press A.";

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

static int make_path(char *out, size_t capacity, const char *name) {
    int n = snprintf(out, capacity, "%s%s%s", current_dir,
                     current_dir[strlen(current_dir) - 1] == '/' ? "" : "/", name);
    return n >= 0 && (size_t)n < capacity;
}

static int compare_entries(const void *left, const void *right) {
    const BrowserEntry *a = (const BrowserEntry *)left;
    const BrowserEntry *b = (const BrowserEntry *)right;
    const unsigned char *pa, *pb;
    if (a->is_directory != b->is_directory)
        return b->is_directory - a->is_directory;
    pa = (const unsigned char *)a->name;
    pb = (const unsigned char *)b->name;
    while (*pa && *pb) {
        int ca = *pa, cb = *pb;
        if (ca >= 'A' && ca <= 'Z') ca += 'a' - 'A';
        if (cb >= 'A' && cb <= 'Z') cb += 'a' - 'A';
        if (ca != cb) return ca - cb;
        ++pa;
        ++pb;
    }
    return (int)*pa - (int)*pb;
}

static void scan_directory(void) {
    DIR *dir = opendir(current_dir);
    struct dirent *entry;
    entry_count = 0;
    total_entries = 0;
    selected_entry = 0;
    if (!dir) {
        snprintf(scan_message, sizeof(scan_message), "Cannot open current folder.");
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        char path[MAX_PATH];
        struct stat st;
        int is_dir;
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (!make_path(path, sizeof(path), entry->d_name))
            continue;
        if (stat(path, &st) != 0)
            continue;
        is_dir = S_ISDIR(st.st_mode) ? 1 : 0;
        if (!is_dir && !has_extension(entry->d_name, ".elf") &&
            !has_extension(entry->d_name, ".self"))
            continue;
        {
            BrowserEntry candidate;
            snprintf(candidate.name, sizeof(candidate.name), "%s", entry->d_name);
            candidate.is_directory = is_dir;
            ++total_entries;
            if (entry_count < MAX_FILES) {
                entries[entry_count++] = candidate;
                qsort(entries, entry_count, sizeof(entries[0]), compare_entries);
            } else if (compare_entries(&candidate, &entries[MAX_FILES - 1]) < 0) {
                entries[MAX_FILES - 1] = candidate;
                qsort(entries, entry_count, sizeof(entries[0]), compare_entries);
            }
        }
    }
    closedir(dir);
    if (total_entries > entry_count)
        snprintf(scan_message, sizeof(scan_message), "Showing %u of %u items (limit %u)",
                 entry_count, total_entries, MAX_FILES);
    else
        snprintf(scan_message, sizeof(scan_message), "%u item(s) in this folder", entry_count);
    snprintf(inspect_message, sizeof(inspect_message), "A: open folder or inspect ELF/SELF.");
}

static void go_parent_directory(void) {
    size_t len;
    char *last_slash;
    if (strcmp(current_dir, "sdmc:/") == 0)
        return;
    len = strlen(current_dir);
    while (len > 6 && current_dir[len - 1] == '/')
        current_dir[--len] = '\0';
    last_slash = strrchr(current_dir + 6, '/');
    if (last_slash)
        last_slash[1] = '\0';
    else
        strcpy(current_dir, "sdmc:/");
    scan_directory();
}

static void inspect_selected(void) {
    char path[MAX_PATH];
    FILE *fp;
    long length;
    unsigned char *data;
    if (!make_path(path, sizeof(path), entries[selected_entry].name)) {
        snprintf(inspect_message, sizeof(inspect_message), "Selected path is too long.");
        return;
    }
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
    data = (unsigned char *)malloc(length > 0 ? (size_t)length : 1u);
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

    if (has_extension(entries[selected_entry].name, ".elf")) {
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

static void activate_selected(void) {
    if (entry_count == 0)
        return;
    if (entries[selected_entry].is_directory) {
        char next_path[MAX_PATH];
        if (!make_path(next_path, sizeof(next_path), entries[selected_entry].name)) {
            snprintf(inspect_message, sizeof(inspect_message), "Folder path is too long.");
            return;
        }
        if (strlen(next_path) + 1 >= sizeof(current_dir)) {
            snprintf(inspect_message, sizeof(inspect_message), "Folder path limit reached.");
            return;
        }
        snprintf(current_dir, sizeof(current_dir), "%s/", next_path);
        scan_directory();
    } else {
        inspect_selected();
    }
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
    case 3: {
        unsigned start = selected_entry > 8 ? selected_entry - 8 : 0;
        unsigned end = start + 17;
        unsigned i;
        printf("       SD CARD BROWSER\n\n");
        printf("Folder: %s\n%s\n\n", current_dir, scan_message);
        if (entry_count == 0) {
            printf("No folders or ELF/SELF files here.\n");
        } else {
            if (end > entry_count) end = entry_count;
            for (i = start; i < end; ++i)
                printf("%c %s%s\n", i == selected_entry ? '>' : ' ',
                       entries[i].name, entries[i].is_directory ? "/" : "");
        }
        printf("\nResult: %s\n", inspect_message);
        break;
    }
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
        printf("UP / DOWN    : Select item\n");
        printf("A            : Open / inspect\n");
        printf("B            : Parent folder\n");
        printf("Y            : Rescan folder\n");
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
    scan_directory();

    while (aptMainLoop()) {
        hidScanInput();
        u32 down = hidKeysDown();

        if (down & KEY_START)
            break;
        if (down & KEY_R) {
            page = (page + 1) % 5;
            redraw = 1;
        } else if (down & KEY_L) {
            page = (page + 4) % 5;
            redraw = 1;
        }
        if (page == 3) {
            if ((down & KEY_DOWN) && entry_count > 0) {
                selected_entry = (selected_entry + 1) % entry_count;
                redraw = 1;
            } else if ((down & KEY_UP) && entry_count > 0) {
                selected_entry = (selected_entry + entry_count - 1) % entry_count;
                redraw = 1;
            }
            if (down & KEY_A) {
                activate_selected();
                redraw = 1;
            }
            if (down & KEY_B) {
                go_parent_directory();
                redraw = 1;
            }
            if (down & KEY_Y) {
                scan_directory();
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
