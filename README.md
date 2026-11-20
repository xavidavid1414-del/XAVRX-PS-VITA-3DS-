# XAVRX PS Vita 3DS

Experimental research project for the New Nintendo 2DS XL. This is **not a functional PS Vita emulator**; full Vita emulation is beyond the practical capabilities of 3DS-family hardware.

## Current milestone: Vita ELF and SELF header inspection

- [x] devkitPro / libctru homebrew scaffold
- [x] Dual-screen homebrew interface with five navigable information pages
- [x] ELF32 little-endian ARM header validation
- [x] Recognize Vita ET_SCE_EXEC and ET_SCE_RELEXEC executable types
- [x] Program-header and section-header table bounds checks
- [x] PT_LOAD segment file-range, filesz/memsz, and alignment validation
- [x] Enforce documented Vita program-header and segment-count limits
- [x] Inspect the public 0x80-byte SCE SELF header and validate extended offsets
- [x] Host tests for valid headers, truncated tables, and malformed load segments
- [x] Verify 3DSX build and host tests in GitHub Actions
- [x] Add on-device project, ELF, SELF, SD browser, and limitations pages
- [x] Implement a conservative Vita SELF base-header inspector
- [ ] Evaluate a limited ARM instruction experiment only if hardware budget permits
- [ ] Benchmark memory and performance constraints

The parser checks ELF identification, ELF version, 32-bit class, little-endian encoding, ARM machine type, supported Vita ELF types (ET_SCE_EXEC and ET_SCE_RELEXEC), header size, declared program/section table ranges, PT_LOAD segment file bounds, memory-size consistency, alignment, and documented Vita segment-count limits. It reads metadata only. The SELF inspector validates the documented public 0x80-byte SCE header layout, including the separate header length, total SELF size, embedded-ELF offset/size, and bounds of public metadata offsets. It also reads the bounded 0x20-byte app-info record (auth ID, vendor ID, SELF type, and app version) and up to eight segment-info records, validating stored ranges, compression values (plain or zlib), and encryption-request states (0, 1, or 2). It never decompresses or decrypts segment contents, verifies signatures, or executes code. It does **not** decrypt SELF files, load segments, link libraries, emulate the Vita kernel, or execute Vita games.

## Build on 3DS

Install devkitPro with the Nintendo 3DS development packages (3ds-dev) and run `make`. The expected output is `build/XAVRX-PS-VITA-3DS.3dsx`.

## Run host parser tests

With a C compiler, run `make test`.

## Project rules

- Never label stubs or metadata inspection as emulation.
- Do not claim game compatibility without reproducible tests.
- Do not include copyrighted Vita firmware, keys, or commercial game files.
- Benchmark each subsystem on real hardware before making performance claims.
- Document limitations and keep each milestone small and testable.

## On-device interface

The current 3DSX opens a dual-screen information menu:

- **Top screen:** project status, ELF checks, SELF metadata, and implementation limits (four pages).
- **Bottom screen:** controls and current page number.
- **D-pad Left/Right:** move between pages.
- **START:** exit to the Homebrew Launcher.

This build is currently an information interface plus host-side metadata parsers. It does not yet browse or open files from the SD card. The next practical milestone is a read-only SD-card file browser that can pass selected ELF/SELF files to the existing validators.


## SD-card file browser

The on-device browser scans the SD-card root for up to 24 files ending in `.elf` or `.self`. Use D-pad Up/Down to select, A to inspect, and Y to rescan. Inspection is read-only and limited to files no larger than 16 MiB; the selected file is passed to the existing metadata validator and is never executed, decrypted, or modified. This first version does not recurse into subdirectories.
