#ifndef XAVRX_VITA_SELF_H
#define XAVRX_VITA_SELF_H
#include <stddef.h>
#include <stdint.h>

#define VITA_SELF_MAX_SEGMENTS 8u

typedef enum {
    VITA_SELF_OK = 0,
    VITA_SELF_ERR_TOO_SMALL,
    VITA_SELF_ERR_MAGIC,
    VITA_SELF_ERR_VERSION,
    VITA_SELF_ERR_TYPE,
    VITA_SELF_ERR_HEADER_RANGE,
    VITA_SELF_ERR_ELF_RANGE,
    VITA_SELF_ERR_OFFSET_RANGE,
    VITA_SELF_ERR_SEGMENT_TABLE,
    VITA_SELF_ERR_SEGMENT_RANGE,
    VITA_SELF_ERR_SEGMENT_FORMAT
} VitaSelfStatus;

typedef struct {
    uint64_t offset;
    uint64_t length;
    uint32_t compression;
    uint32_t unknown;
    uint64_t encryption;
} VitaSelfSegmentInfo;

typedef struct {
    uint16_t flags;
    uint32_t metadata_offset;
    uint64_t header_length;
    uint64_t self_file_size;
    uint64_t embedded_elf_offset;
    uint64_t embedded_elf_size;
    uint64_t self_offset;
    uint64_t appinfo_offset;
    uint64_t program_headers_offset;
    uint64_t section_headers_offset;
    uint64_t section_info_offset;
    uint64_t sce_version_offset;
    uint64_t control_info_offset;
    uint64_t control_info_size;
    int has_appinfo;
    uint64_t auth_id;
    uint32_t vendor_id;
    uint32_t self_type;
    uint64_t app_version;
    uint16_t segment_count;
    VitaSelfSegmentInfo segments[VITA_SELF_MAX_SEGMENTS];
} VitaSelfInfo;

/* Metadata-only inspection. Never decrypts, decompresses, or executes segments. */
VitaSelfStatus vita_self_inspect_header(const uint8_t *data, size_t size,
                                        VitaSelfInfo *out);
const char *vita_self_status_string(VitaSelfStatus status);
#endif
