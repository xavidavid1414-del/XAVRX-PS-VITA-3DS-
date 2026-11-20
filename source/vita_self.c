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
static int optional_offset_in_range(uint64_t offset, uint64_t file_size) {
    return offset == 0 || offset < file_size;
}

VitaSelfStatus vita_self_inspect_header(const uint8_t *data, size_t size,
                                        VitaSelfInfo *out) {
    uint64_t file_size, elf_offset, elf_size, control_offset, control_size;
    uint64_t header_length, metadata_offset, appinfo_offset;
    if (data == 0 || out == 0 || size < 0x80u)
        return VITA_SELF_ERR_TOO_SMALL;
    if (data[0] != 0x53 || data[1] != 0x43 ||
        data[2] != 0x45 || data[3] != 0x00)
        return VITA_SELF_ERR_MAGIC;
    if (read_u32le(data + 4) != 3u)
        return VITA_SELF_ERR_VERSION;
    if (read_u16le(data + 0x0a) != 1u)
        return VITA_SELF_ERR_TYPE;

    header_length = read_u64le(data + 0x10);
    elf_size = read_u64le(data + 0x18);
    file_size = read_u64le(data + 0x20);
    metadata_offset = read_u32le(data + 0x0c);
    appinfo_offset = read_u64le(data + 0x38);
    elf_offset = read_u64le(data + 0x40);
    control_offset = read_u64le(data + 0x68);
    control_size = read_u64le(data + 0x70);

    if (file_size < 0x80u || file_size > (uint64_t)size ||
        header_length < 0x80u || header_length > file_size ||
        metadata_offset >= file_size)
        return VITA_SELF_ERR_HEADER_RANGE;
    if (elf_offset > file_size || elf_size > file_size - elf_offset)
        return VITA_SELF_ERR_ELF_RANGE;
    if ((control_offset == 0 && control_size != 0) ||
        (control_offset != 0 &&
         (control_offset > file_size || control_size > file_size - control_offset)) ||
        !optional_offset_in_range(read_u64le(data + 0x30), file_size) ||
        !optional_offset_in_range(appinfo_offset, file_size) ||
        !optional_offset_in_range(read_u64le(data + 0x48), file_size) ||
        !optional_offset_in_range(read_u64le(data + 0x50), file_size) ||
        !optional_offset_in_range(read_u64le(data + 0x58), file_size) ||
        !optional_offset_in_range(read_u64le(data + 0x60), file_size))
        return VITA_SELF_ERR_OFFSET_RANGE;
    if (appinfo_offset != 0 &&
        (appinfo_offset > file_size || 0x20u > file_size - appinfo_offset))
        return VITA_SELF_ERR_OFFSET_RANGE;

    out->flags = read_u16le(data + 0x08);
    out->metadata_offset = (uint32_t)metadata_offset;
    out->header_length = header_length;
    out->self_file_size = file_size;
    out->embedded_elf_offset = elf_offset;
    out->embedded_elf_size = elf_size;
    out->self_offset = read_u64le(data + 0x30);
    out->appinfo_offset = appinfo_offset;
    out->program_headers_offset = read_u64le(data + 0x48);
    out->section_headers_offset = read_u64le(data + 0x50);
    out->section_info_offset = read_u64le(data + 0x58);
    out->sce_version_offset = read_u64le(data + 0x60);
    out->control_info_offset = control_offset;
    out->control_info_size = control_size;
    out->has_appinfo = appinfo_offset != 0;
    out->auth_id = out->has_appinfo ? read_u64le(data + appinfo_offset) : 0;
    out->vendor_id = out->has_appinfo ? read_u32le(data + appinfo_offset + 8) : 0;
    out->self_type = out->has_appinfo ? read_u32le(data + appinfo_offset + 12) : 0;
    out->app_version = out->has_appinfo ? read_u64le(data + appinfo_offset + 16) : 0;
    return VITA_SELF_OK;
}

const char *vita_self_status_string(VitaSelfStatus status) {
    switch (status) {
    case VITA_SELF_OK: return "Valid Vita SELF header and public app-info";
    case VITA_SELF_ERR_TOO_SMALL: return "SELF extended header is truncated";
    case VITA_SELF_ERR_MAGIC: return "Not an SCE SELF container";
    case VITA_SELF_ERR_VERSION: return "Unsupported SCE header version";
    case VITA_SELF_ERR_TYPE: return "Unsupported SCE header type";
    case VITA_SELF_ERR_HEADER_RANGE: return "SELF header or file-size field is invalid";
    case VITA_SELF_ERR_ELF_RANGE: return "Embedded ELF range exceeds the SELF file";
    case VITA_SELF_ERR_OFFSET_RANGE: return "A public SELF metadata offset or app-info record is outside the file";
    default: return "Unknown SELF header error";
    }
}
