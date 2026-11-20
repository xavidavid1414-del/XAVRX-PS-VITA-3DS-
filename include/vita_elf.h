#ifndef XAVRX_VITA_ELF_H
#define XAVRX_VITA_ELF_H
#include <stddef.h>
#include <stdint.h>
typedef enum {
    VITA_ELF_OK = 0, VITA_ELF_ERR_TOO_SMALL, VITA_ELF_ERR_MAGIC,
    VITA_ELF_ERR_CLASS, VITA_ELF_ERR_ENDIAN, VITA_ELF_ERR_VERSION,
    VITA_ELF_ERR_MACHINE, VITA_ELF_ERR_HEADER_SIZE,
    VITA_ELF_ERR_PROGRAM_TABLE, VITA_ELF_ERR_SECTION_TABLE,
    VITA_ELF_ERR_LOAD_SEGMENT, VITA_ELF_ERR_TYPE, VITA_ELF_ERR_VITA_LIMIT
} VitaElfStatus;
typedef struct {
    uint32_t entry, program_header_offset, section_header_offset;
    uint16_t elf_type;
    uint16_t program_header_entry_size, program_header_count;
    uint16_t section_header_entry_size, section_header_count;
    uint16_t load_segment_count, relocation_segment_count;
} VitaElfInfo;
/* Validates Vita ELF32 metadata and segment file bounds only.
   It never loads, decrypts, links, or executes code. */
VitaElfStatus vita_elf_parse_header(const uint8_t *data, size_t size, VitaElfInfo *out);
const char *vita_elf_status_string(VitaElfStatus status);
#endif
