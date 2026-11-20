#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "vita_elf.h"
static void put16(uint8_t *p, uint16_t v) { p[0]=(uint8_t)v; p[1]=(uint8_t)(v>>8); }
static void put32(uint8_t *p, uint32_t v) { p[0]=(uint8_t)v; p[1]=(uint8_t)(v>>8); p[2]=(uint8_t)(v>>16); p[3]=(uint8_t)(v>>24); }
static void make_header(uint8_t *elf, size_t size) {
    memset(elf, 0, size); elf[0]=0x7f; elf[1]='E'; elf[2]='L'; elf[3]='F';
    elf[4]=1; elf[5]=1; elf[6]=1; put32(elf+20, 1); put16(elf+18, 40);
    put32(elf+24, 0x81000000u); put16(elf+40, 52);
}
int main(void) {
    uint8_t elf[128]; VitaElfInfo info;
    make_header(elf, sizeof elf);
    assert(vita_elf_parse_header(elf, 52, &info) == VITA_ELF_OK);
    assert(info.entry == 0x81000000u);
    assert(vita_elf_parse_header(elf, 12, &info) == VITA_ELF_ERR_TOO_SMALL);
    put32(elf+28, 52); put16(elf+42, 32); put16(elf+44, 3);
    assert(vita_elf_parse_header(elf, 128, &info) == VITA_ELF_OK);
    assert(vita_elf_parse_header(elf, 100, &info) == VITA_ELF_ERR_PROGRAM_TABLE);
    put32(elf+32, 80); put16(elf+46, 40); put16(elf+48, 2);
    assert(vita_elf_parse_header(elf, 128, &info) == VITA_ELF_ERR_SECTION_TABLE);
    make_header(elf, sizeof elf); elf[0]=0;
    assert(vita_elf_parse_header(elf, sizeof elf, &info) == VITA_ELF_ERR_MAGIC);
    puts("Vita ELF parser tests: PASS"); return 0;
}