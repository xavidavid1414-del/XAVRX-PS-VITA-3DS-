#include "vita_elf.h"

static uint16_t read_u16le(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}
static uint32_t read_u32le(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static int table_fits(uint32_t offset, uint16_t entry_size, uint16_t count,
                      size_t size, uint16_t minimum_entry_size) {
    uint64_t end;
    if (count == 0) return 1;
    if (entry_size < minimum_entry_size || offset == 0) return 0;
    end = (uint64_t)offset + (uint64_t)entry_size * (uint64_t)count;
    return end <= (uint64_t)size;
}
static int is_power_of_two(uint32_t value) {
    return value != 0 && (value & (value - 1u)) == 0;
}
static int validate_load_segments(const uint8_t *data, size_t size,
                                  uint32_t offset, uint16_t entry_size,
                                  uint16_t count, uint16_t *load_count) {
    uint16_t i;
    *load_count = 0;
    for (i = 0; i < count; ++i) {
        const uint8_t *ph = data + offset + (uint32_t)i * entry_size;
        uint32_t type = read_u32le(ph);
        uint32_t file_offset, virtual_address, file_size, memory_size, alignment;
        uint64_t file_end;
        if (type != 1u) continue; /* PT_LOAD */
        file_offset = read_u32le(ph + 4);
        virtual_address = read_u32le(ph + 8);
        file_size = read_u32le(ph + 16);
        memory_size = read_u32le(ph + 20);
        alignment = read_u32le(ph + 28);
        file_end = (uint64_t)file_offset + (uint64_t)file_size;
        if (file_end > (uint64_t)size || file_size > memory_size) return 0;
        if (alignment > 1u &&
            (!is_power_of_two(alignment) ||
             (file_offset % alignment) != (virtual_address % alignment))) return 0;
        ++(*load_count);
    }
    return 1;
}
VitaElfStatus vita_elf_parse_header(const uint8_t *data, size_t size, VitaElfInfo *out) {
    uint16_t ehsize, phentsize, phnum, shentsize, shnum, load_count;
    uint32_t phoff, shoff;
    if (data == 0 || out == 0 || size < 52) return VITA_ELF_ERR_TOO_SMALL;
    if (data[0] != 0x7f || data[1] != 'E' || data[2] != 'L' || data[3] != 'F')
        return VITA_ELF_ERR_MAGIC;
    if (data[4] != 1) return VITA_ELF_ERR_CLASS;
    if (data[5] != 1) return VITA_ELF_ERR_ENDIAN;
    if (data[6] != 1 || read_u32le(data + 20) != 1) return VITA_ELF_ERR_VERSION;
    if (read_u16le(data + 18) != 40) return VITA_ELF_ERR_MACHINE;
    ehsize = read_u16le(data + 40);
    if (ehsize < 52 || ehsize > size) return VITA_ELF_ERR_HEADER_SIZE;
    phoff = read_u32le(data + 28);
    phentsize = read_u16le(data + 42);
    phnum = read_u16le(data + 44);
    shoff = read_u32le(data + 32);
    shentsize = read_u16le(data + 46);
    shnum = read_u16le(data + 48);
    if (!table_fits(phoff, phentsize, phnum, size, 32))
        return VITA_ELF_ERR_PROGRAM_TABLE;
    if (!table_fits(shoff, shentsize, shnum, size, 40))
        return VITA_ELF_ERR_SECTION_TABLE;
    if (!validate_load_segments(data, size, phoff, phentsize, phnum, &load_count))
        return VITA_ELF_ERR_LOAD_SEGMENT;
    out->entry = read_u32le(data + 24);
    out->program_header_offset = phoff;
    out->section_header_offset = shoff;
    out->program_header_entry_size = phentsize;
    out->program_header_count = phnum;
    out->section_header_entry_size = shentsize;
    out->section_header_count = shnum;
    out->load_segment_count = load_count;
    return VITA_ELF_OK;
}
const char *vita_elf_status_string(VitaElfStatus status) {
    switch (status) {
    case VITA_ELF_OK: return "Valid ELF32 ARM header and load segment bounds";
    case VITA_ELF_ERR_TOO_SMALL: return "File/header too small";
    case VITA_ELF_ERR_MAGIC: return "Invalid ELF magic";
    case VITA_ELF_ERR_CLASS: return "Not ELF32";
    case VITA_ELF_ERR_ENDIAN: return "Not little-endian";
    case VITA_ELF_ERR_VERSION: return "Unsupported ELF version";
    case VITA_ELF_ERR_MACHINE: return "Not an ARM executable";
    case VITA_ELF_ERR_HEADER_SIZE: return "Invalid ELF header size";
    case VITA_ELF_ERR_PROGRAM_TABLE: return "Program header table is invalid or out of bounds";
    case VITA_ELF_ERR_SECTION_TABLE: return "Section header table is invalid or out of bounds";
    case VITA_ELF_ERR_LOAD_SEGMENT: return "Invalid PT_LOAD segment bounds or alignment";
    default: return "Unknown ELF parser error";
    }
}
