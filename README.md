# XAVRX PS Vita 3DS

Experimental research project for the New Nintendo 2DS XL. This is **not a functional PS Vita emulator**; full Vita emulation is beyond the practical capabilities of 3DS-family hardware.

## Current milestone: Vita ELF/SELF metadata inspector

- [x] devkitPro / libctru homebrew scaffold
- [x] Dual-screen interface with five navigable pages
- [x] ELF32 little-endian ARM header validation
- [x] Recognition of Vita ET_SCE_EXEC and ET_SCE_RELEXEC types
- [x] Program/section table bounds checks
- [x] PT_LOAD file-range, filesz/memsz, and alignment validation
- [x] Vita program-header and segment-count limits
- [x] Public 0x80-byte SCE SELF header inspection
- [x] AppInfo and segment metadata range validation
- [x] Host tests for ELF and SELF parsers
- [x] GitHub Actions host tests and 3DSX build
- [x] On-device project, ELF, SELF, SD browser, and limitations pages
- [x] Read-only SD browser with folder navigation and ELF/SELF inspection
- [x] Stable alphabetical selection of the first 24 matching entries in large folders

The ELF parser checks identification, version, 32-bit class, little-endian encoding, ARM machine type, supported Vita ELF types, header size, program/section table bounds, PT_LOAD file bounds, memory-size consistency, alignment, and documented Vita segment-count limits.

The SELF inspector validates the public SCE header, embedded-ELF range, metadata offsets, the bounded 0x20-byte AppInfo record, and up to eight segment-info records. It validates stored ranges and recognized compression/encryption metadata values. It does not decrypt or decompress content, verify signatures, load segments, or execute code.

## On-device interface

The 3DSX opens a dual-screen menu with five pages:

1. Project status
2. ELF inspection details
3. SELF inspection details
4. SD-card browser
5. Project limitations

Use **L/R shoulder buttons** to change pages. Press START to exit.

### SD-card browser

The browser lists directories and files ending in `.elf` or `.self` in the current folder.

- **Up/Down:** select an entry
- **A:** enter a folder or inspect an ELF/SELF file
- **B:** return to the parent folder
- **Y:** rescan the current folder

It scans the whole current directory, then displays at most 24 entries, prioritizing directories and sorting names alphabetically without case sensitivity. If more than 24 entries match, the interface reports the total and the number displayed. Folder navigation is manual; it does not recursively scan the entire SD card.

Inspection is read-only and limited to files no larger than 16 MiB. Files are passed to the existing metadata validators and are never executed, decrypted, decompressed, or modified.

## Build on 3DS

Install devkitPro with the Nintendo 3DS development packages (3ds-dev), then run:

```sh
make
```

Expected output: `build/XAVRX-PS-VITA-3DS.3dsx`.

## Run host parser tests

With a C compiler, run:

```sh
make test
```

## Project rules and limitations

- Never describe metadata inspection as Vita emulation.
- Do not claim game compatibility without reproducible tests.
- Do not include copyrighted Vita firmware, keys, or commercial game files.
- Benchmark each subsystem on real hardware before making performance claims.
- Keep milestones small, documented, and testable.

The project currently provides a 3DS homebrew interface and read-only ELF/SELF metadata inspection. It does **not** emulate the Vita CPU or GPU, provide Vita OS/kernel services, decrypt SELF files, or run Vita games.