# XAVRX PS Vita 3DS

Experimental research project for the New Nintendo 2DS XL. This is **not a functional PS Vita emulator**; full Vita emulation is beyond the practical capabilities of 3DS-family hardware.

## Current milestone: ELF32 ARM metadata validation

- [x] devkitPro / libctru homebrew scaffold
- [x] Basic top-screen status display and START-to-exit input loop
- [x] ELF32 little-endian ARM header validation
- [x] Program-header and section-header table bounds checks
- [x] Host tests for valid headers and malformed/truncated tables
- [ ] Verify 3DSX build in GitHub Actions
- [ ] Research documented Vita SELF container format
- [ ] Evaluate a limited ARM instruction experiment only if hardware budget permits
- [ ] Benchmark memory and performance constraints

The parser checks ELF identification, ELF version, 32-bit class, little-endian encoding, ARM machine type, header size, and declared program/section table ranges. It reads metadata only. It does **not** decrypt SELF files, load segments, link libraries, emulate the Vita kernel, or execute Vita games.

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