#ifndef XAVRX_VITA_ELF_H
#define XAVRX_VITA_ELF_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    VITA_ELF_OK = 0,
    VITA_ELF_ERR_TOO_SMALL,
    VITA_ELF_ERR_MAGIC,
    VITA_ELF_ERR_CLASS,
    VITA_ELF_ERR_ENDIAN,
    VITA_ELF_ERR_VERSION,
    VITA_ELF_ERR_MACHINE,
    VITA_ELF_ERR_HEADER_SIZE
} VitaElfStatus;

typedef struct {
    uint32_t entry;
    uint32_t program_header_offset;
    uint32_t section_header_offset;
    uint16_t program_header_entry_size;
    uint16_t program_header_count;
    uint16_t section_header_entry_size;
    uint16_t section_header_count;
} VitaElfInfo;

/* Parses ELF32 little-endian ARM metadata only; it does not load or execute code. */
VitaElfStatus vita_elf_parse_header(const uint8_t *data, size_t size, VitaElfInfo *out);
const char *vita_elf_status_string(VitaElfStatus status);

#endif
