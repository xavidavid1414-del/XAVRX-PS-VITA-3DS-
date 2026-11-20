# XAVRX PS Vita 3DS

Experimental research project for the New Nintendo 2DS XL. This is **not a functional PS Vita emulator**; full Vita emulation is beyond the practical capabilities of 3DS-family hardware.

## Current milestone: ELF32 ARM header inspection

- [x] devkitPro / libctru homebrew scaffold
- [x] Basic top-screen status display and START-to-exit input loop
- [x] ELF32 little-endian ARM header parser (metadata only)
- [x] Host-side unit tests for valid and invalid ELF headers
- [ ] Verify the 3DSX build in GitHub Actions
- [ ] Research Vita SELF packaging and executable formats
- [ ] Evaluate a strictly limited ARMv7 instruction experiment
- [ ] Benchmark memory and performance constraints

The parser checks ELF identification, 32-bit class, little-endian encoding, version, ARM machine type, and ELF header size. It reads entry-point and table metadata. It does **not** load, decrypt, link, or execute a Vita program.

## Build on 3DS

Install devkitPro with the Nintendo 3DS development packages (3ds-dev) and run `make`.

This should produce `XAVRX-PS-VITA-3DS.3dsx`.

## Run host parser tests

With a C compiler, run `make test`.

## Project rules

- Never label stubs or metadata inspection as emulation.
- Do not claim game compatibility without reproducible tests.
- Do not include copyrighted Vita firmware, keys, or commercial game files.
- Benchmark each subsystem on real hardware before making performance claims.
- Document limitations and keep each milestone small and testable.