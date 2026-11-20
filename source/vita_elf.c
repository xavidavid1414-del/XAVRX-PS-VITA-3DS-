#include "vita_elf.h"

static uint16_t read_u16le(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read_u32le(const uint8_t *p)
{
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

VitaElfStatus vita_elf_parse_header(const uint8_t *data, size_t size, VitaElfInfo *out)
{
    if (data == 0 || out == 0 || size < 52)
        return VITA_ELF_ERR_TOO_SMALL;
    if (data[0] != 0x7f || data[1] != 'E' || data[2] != 'L' || data[3] != 'F')
        return VITA_ELF_ERR_MAGIC;
    if (data[4] != 1)
        return VITA_ELF_ERR_CLASS;
    if (data[5] != 1)
        return VITA_ELF_ERR_ENDIAN;
    if (data[6] != 1)
        return VITA_ELF_ERR_VERSION;
    if (read_u16le(data + 18) != 40)
        return VITA_ELF_ERR_MACHINE; /* EM_ARM */

    if (read_u16le(data + 40) < 52)
        return VITA_ELF_ERR_HEADER_SIZE;

    out->entry = read_u32le(data + 24);
    out->program_header_offset = read_u32le(data + 28);
    out->section_header_offset = read_u32le(data + 32);
    out->program_header_entry_size = read_u16le(data + 42);
    out->program_header_count = read_u16le(data + 44);
    out->section_header_entry_size = read_u16le(data + 46);
    out->section_header_count = read_u16le(data + 48);
    return VITA_ELF_OK;
}

const char *vita_elf_status_string(VitaElfStatus status)
{
    switch (status) {
    case VITA_ELF_OK: return "Valid ELF32 ARM header";
    case VITA_ELF_ERR_TOO_SMALL: return "File/header too small";
    case VITA_ELF_ERR_MAGIC: return "Invalid ELF magic";
    case VITA_ELF_ERR_CLASS: return "Not ELF32";
    case VITA_ELF_ERR_ENDIAN: return "Not little-endian";
    case VITA_ELF_ERR_VERSION: return "Unsupported ELF version";
    case VITA_ELF_ERR_MACHINE: return "Not an ARM executable";
    case VITA_ELF_ERR_HEADER_SIZE: return "Invalid ELF header size";
    default: return "Unknown ELF parser error";
    }
}
