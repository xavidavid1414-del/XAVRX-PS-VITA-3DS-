#ifndef XAVRX_VITA_SELF_H
#define XAVRX_VITA_SELF_H
#include <stddef.h>
#include <stdint.h>

typedef enum {
    VITA_SELF_OK = 0,
    VITA_SELF_ERR_TOO_SMALL,
    VITA_SELF_ERR_MAGIC,
    VITA_SELF_ERR_VERSION,
    VITA_SELF_ERR_TYPE,
    VITA_SELF_ERR_HEADER_RANGE,
    VITA_SELF_ERR_ELF_RANGE
} VitaSelfStatus;

typedef struct {
    uint16_t flags;
    uint32_t metadata_offset;
    uint64_t embedded_elf_offset;
    uint64_t embedded_elf_size;
} VitaSelfInfo;

/* Inspects the public 0x20-byte Vita SCE header only.
   Does not decrypt, verify signatures, or parse protected metadata. */
VitaSelfStatus vita_self_inspect_header(const uint8_t *data, size_t size,
                                        VitaSelfInfo *out);
const char *vita_self_status_string(VitaSelfStatus status);
#endif
