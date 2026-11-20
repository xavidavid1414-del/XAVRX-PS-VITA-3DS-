# XAVRX PS Vita 3DS

Experimental research project exploring whether a small, measurable subset of PlayStation Vita software behavior can be studied on the New Nintendo 2DS XL.

> **Status: early prototype scaffold. This is not a working PS Vita emulator.** Full PS Vita emulation on a New Nintendo 2DS XL is beyond the practical capabilities of the hardware. The Vita uses a substantially more powerful multi-core ARM platform, a dedicated GPU, and far more memory than the 3DS family.

## Current milestone

- [x] Initial devkitPro/libctru homebrew application scaffold
- [x] Basic top-screen status display and input loop
- [ ] Build verification on devkitPro
- [ ] Research notes for Vita executable formats and system interfaces
- [ ] Carefully scoped CPU / memory experiments
- [ ] Compatibility tests using original, self-created test programs

## Build

Install devkitPro with the Nintendo 3DS development packages (`3ds-dev`), then run:

```sh
make
```

The build should produce `XAVRX-PS-VITA-3DS.3dsx` and a `.cia` only if the corresponding packaging target is configured. The current Makefile builds a 3DSX homebrew application.

## Project rules

- Never label a stub, placeholder, or mock as an implemented Vita feature.
- Do not claim game compatibility without reproducible tests.
- Do not include copyrighted Vita firmware, keys, or commercial game files.
- Record hardware limitations and benchmark every new subsystem.
- Prefer small, testable research milestones over claims of universal compatibility.

## Current functionality

The application initializes libctru graphics, displays project status, and exits when START is pressed. It does not load or execute PS Vita games.
