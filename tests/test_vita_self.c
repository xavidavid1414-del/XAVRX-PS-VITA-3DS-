#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "vita_self.h"

static void put16(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8);
}
static void put32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24);
}
static void put64(uint8_t *p, uint64_t v) {
    put32(p, (uint32_t)v); put32(p + 4, (uint32_t)(v >> 32));
}
static void make_self(uint8_t *data, size_t size) {
    memset(data, 0, size);
    data[0] = 0x53; data[1] = 0x43; data[2] = 0x45;
    put32(data + 4, 3);
    put16(data + 8, 0x00c0);
    put16(data + 10, 1);
    put32(data + 12, 0x80);
    put64(data + 16, 0x1000);
    put64(data + 24, 0x40);
    put64(data + 32, size);
    put64(data + 56, 0x90);       /* app-info record */
    put64(data + 64, 0x100);      /* embedded ELF offset */
    put64(data + 88, 0xb0);       /* segment-info table offset */

    put64(data + 0x90, 0x1122334455667788ULL);
    put32(data + 0x98, 0x12345678);
    put32(data + 0x9c, 2);
    put64(data + 0xa0, 0x0102030405060708ULL);

    data[0x100] = 0x7f; data[0x101] = 'E';
    data[0x102] = 'L'; data[0x103] = 'F';
    data[0x104] = 1; data[0x105] = 1;
    put16(data + 0x100 + 42, 0x20);
    put16(data + 0x100 + 44, 1);

    put64(data + 0xb0, 0x1100);    /* segment data offset */
    put64(data + 0xb8, 0x20);     /* segment stored length */
    put64(data + 0xc0, 1);        /* uncompressed */
    put64(data + 0xc8, 2);        /* plain / not encrypted */
}
int main(void) {
    uint8_t data[0x1200];
    VitaSelfInfo info;
    make_self(data, sizeof data);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_OK);
    assert(info.header_length == 0x1000);
    assert(info.embedded_elf_offset == 0x100);
    assert(info.embedded_elf_size == 0x40);
    assert(info.flags == 0x00c0);
    assert(info.self_file_size == sizeof data);
    assert(info.has_appinfo);
    assert(info.auth_id == 0x1122334455667788ULL);
    assert(info.vendor_id == 0x12345678);
    assert(info.self_type == 2);
    assert(info.app_version == 0x0102030405060708ULL);
    assert(info.segment_count == 1);
    assert(info.segments[0].offset == 0x1100);
    assert(info.segments[0].length == 0x20);
    assert(info.segments[0].compression == 1);
    assert(info.segments[0].encryption == 2);
    assert(vita_self_inspect_header(data, 0x7f, &info) == VITA_SELF_ERR_TOO_SMALL);

    data[0] = 0;
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_MAGIC);
    make_self(data, sizeof data);
    put32(data + 4, 2);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_VERSION);
    make_self(data, sizeof data);
    put16(data + 10, 2);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_TYPE);
    make_self(data, sizeof data);
    put64(data + 16, 0x2000);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_HEADER_RANGE);
    make_self(data, sizeof data);
    put64(data + 64, 0x11f0);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_ELF_RANGE);
    make_self(data, sizeof data);
    put64(data + 56, 0x11f8);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_OFFSET_RANGE);
    make_self(data, sizeof data);
    put64(data + 72, 0x11f0); put64(data + 112, 0x20);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_OFFSET_RANGE);
    make_self(data, sizeof data);
    put64(data + 0xb0, 0x11f8);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_SEGMENT_RANGE);
    make_self(data, sizeof data);
    put64(data + 0xc0, 9);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_SEGMENT_FORMAT);
    make_self(data, sizeof data);
    put16(data + 0x100 + 42, 0x10);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_SEGMENT_TABLE);
    puts("Vita SELF segment metadata tests: PASS");
    return 0;
}
