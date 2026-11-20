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
    put64(data + 16, 0x80);       /* header length */
    put64(data + 24, 0x40);       /* embedded ELF size */
    put64(data + 32, size);       /* SELF file size */
    put64(data + 64, 0xc0);      /* ELF offset, independent of header length */
}
int main(void) {
    uint8_t data[256];
    VitaSelfInfo info;
    make_self(data, sizeof data);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_OK);
    assert(info.header_length == 0x80);
    assert(info.embedded_elf_offset == 0xc0);
    assert(info.embedded_elf_size == 0x40);
    assert(info.flags == 0x00c0);
    assert(info.self_file_size == sizeof data);
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
    put64(data + 16, 0x1000);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_HEADER_RANGE);
    make_self(data, sizeof data);
    put64(data + 64, 0x1000);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_ELF_RANGE);
    make_self(data, sizeof data);
    put64(data + 72, 0xf0); put64(data + 112, 0x20);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_OFFSET_RANGE);
    puts("Vita SELF extended-header tests: PASS");
    return 0;
}
