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
    data[0] = 0x53; data[1] = 0x43; data[2] = 0x45; data[3] = 0;
    put32(data + 4, 3);       /* Vita SCE header version */
    put16(data + 8, 0x00c0);  /* flags / SDK type */
    put16(data + 10, 1);      /* SELF header type */
    put32(data + 12, 0x20);   /* metadata offset */
    put64(data + 16, 0x80);   /* embedded ELF offset */
    put64(data + 24, 0x40);   /* embedded ELF size */
}
int main(void) {
    uint8_t data[256];
    VitaSelfInfo info;
    make_self(data, sizeof data);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_OK);
    assert(info.embedded_elf_offset == 0x80);
    assert(info.embedded_elf_size == 0x40);
    assert(info.flags == 0x00c0);
    assert(vita_self_inspect_header(data, 12, &info) == VITA_SELF_ERR_TOO_SMALL);

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
    put64(data + 24, 0x1000);
    assert(vita_self_inspect_header(data, sizeof data, &info) == VITA_SELF_ERR_ELF_RANGE);
    puts("Vita SELF header tests: PASS");
    return 0;
}
