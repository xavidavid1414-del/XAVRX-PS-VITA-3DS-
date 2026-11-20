#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "vita_elf.h"

static void put16(uint8_t *p, uint16_t v) { p[0]=(uint8_t)v; p[1]=(uint8_t)(v>>8); }
static void put32(uint8_t *p, uint32_t v)
{
    p[0]=(uint8_t)v; p[1]=(uint8_t)(v>>8);
    p[2]=(uint8_t)(v>>16); p[3]=(uint8_t)(v>>24);
}

int main(void)
{
    uint8_t elf[52];
    VitaElfInfo info;
    memset(elf, 0, sizeof elf);
    elf[0]=0x7f; elf[1]='E'; elf[2]='L'; elf[3]='F';
    elf[4]=1; elf[5]=1; elf[6]=1;
    put16(elf+18, 40); /* EM_ARM */
    put32(elf+24, 0x81000000u);
    put32(elf+28, 52);
    put16(elf+40, 52);
    put16(elf+42, 32);
    put16(elf+44, 3);

    assert(vita_elf_parse_header(elf, sizeof elf, &info) == VITA_ELF_OK);
    assert(info.entry == 0x81000000u);
    assert(info.program_header_offset == 52);
    assert(info.program_header_count == 3);
    assert(vita_elf_parse_header(elf, 12, &info) == VITA_ELF_ERR_TOO_SMALL);

    elf[0] = 0;
    assert(vita_elf_parse_header(elf, sizeof elf, &info) == VITA_ELF_ERR_MAGIC);
    puts("Vita ELF parser tests: PASS");
    return 0;
}
