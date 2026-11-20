# XAVRX PS Vita 3DS

Experimental research project for the New Nintendo 2DS XL. This is **not a functional PS Vita emulator**; full Vita emulation is beyond the practical capabilities of 3DS-family hardware.

## Current milestone: Vita ELF32 metadata validation

- [x] devkitPro / libctru homebrew scaffold
- [x] Basic top-screen status display and START-to-exit input loop
- [x] ELF32 little-endian ARM header validation
- [x] Recognize Vita ET_SCE_EXEC and ET_SCE_RELEXEC executable types
- [x] Program-header and section-header table bounds checks
- [x] PT_LOAD segment file-range, filesz/memsz, and alignment validation
- [x] Enforce documented Vita program-header and segment-count limits
- [x] Host tests for valid headers, truncated tables, and malformed load segments
- [x] Verify 3DSX build and host tests in GitHub Actions
- [ ] Implement a conservative Vita SELF container-header inspector
- [ ] Evaluate a limited ARM instruction experiment only if hardware budget permits
- [ ] Benchmark memory and performance constraints

The parser checks ELF identification, ELF version, 32-bit class, little-endian encoding, ARM machine type, supported Vita ELF types (ET_SCE_EXEC and ET_SCE_RELEXEC), header size, declared program/section table ranges, PT_LOAD segment file bounds, memory-size consistency, alignment, and documented Vita segment-count limits. It reads metadata only. The SELF container itself is not parsed yet. It does **not** decrypt SELF files, load segments, link libraries, emulate the Vita kernel, or execute Vita games.

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