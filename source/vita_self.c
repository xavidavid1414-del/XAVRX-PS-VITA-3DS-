#include "vita_self.h"

static uint16_t read_u16le(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}
static uint32_t read_u32le(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static uint64_t read_u64le(const uint8_t *p) {
    return (uint64_t)read_u32le(p) | ((uint64_t)read_u32le(p + 4) << 32);
}

VitaSelfStatus vita_self_inspect_header(const uint8_t *data, size_t size,
                                        VitaSelfInfo *out) {
    uint32_t metadata_offset;
    uint64_t header_length, elf_size;
    if (data == 0 || out == 0 || size < 0x20u)
        return VITA_SELF_ERR_TOO_SMALL;
    if (data[0] != 0x53 || data[1] != 0x43 ||
        data[2] != 0x45 || data[3] != 0x00)
        return VITA_SELF_ERR_MAGIC;
    if (read_u32le(data + 4) != 3u)
        return VITA_SELF_ERR_VERSION;
    if (read_u16le(data + 0x0a) != 1u)
        return VITA_SELF_ERR_TYPE;

    metadata_offset = read_u32le(data + 0x0c);
    header_length = read_u64le(data + 0x10);
    elf_size = read_u64le(data + 0x18);

    if (header_length < 0x20u || header_length > (uint64_t)size ||
        metadata_offset >= size)
        return VITA_SELF_ERR_HEADER_RANGE;
    if (elf_size > (uint64_t)size - header_length)
        return VITA_SELF_ERR_ELF_RANGE;

    out->flags = read_u16le(data + 0x08);
    out->metadata_offset = metadata_offset;
    out->embedded_elf_offset = header_length;
    out->embedded_elf_size = elf_size;
    return VITA_SELF_OK;
}

const char *vita_self_status_string(VitaSelfStatus status) {
    switch (status) {
    case VITA_SELF_OK: return "Valid Vita SCE header bounds";
    case VITA_SELF_ERR_TOO_SMALL: return "SELF header is truncated";
    case VITA_SELF_ERR_MAGIC: return "Not an SCE SELF container";
    case VITA_SELF_ERR_VERSION: return "Unsupported SCE header version";
    case VITA_SELF_ERR_TYPE: return "Unsupported SCE header type";
    case VITA_SELF_ERR_HEADER_RANGE: return "SELF metadata/header offset is invalid";
    case VITA_SELF_ERR_ELF_RANGE: return "Embedded ELF range exceeds the file";
    default: return "Unknown SELF header error";
    }
}
