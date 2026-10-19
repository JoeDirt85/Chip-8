# CHIP-8 Emulator
 
A CHIP-8 interpreter written in C++, using SDL for the window, rendering, and keyboard input.
 
<!-- Add a screenshot of the IBM logo ROM or a game here: ![screenshot](docs/screenshot.png) -->
 
## Features
 
- CHIP-8 CPU core with the standard opcode set
- CPU runs at about 700 instructions per second, timers tick at 60 Hz
- 64x32 display scaled up to a larger window
- Keyboard mapping for the 16-key hex keypad
## Requirements
 
- Windows with Visual Studio 2026
- SDL3, installed with [vcpkg](https://github.com/microsoft/vcpkg) (triplet `x64-windows`)
- Build platform: x64
## Building
 
1. Clone the repository.
2. Install [vcpkg](https://github.com/microsoft/vcpkg), run `vcpkg integrate install`, then install SDL3 with `vcpkg install sdl3:x64-windows`. [If the repo includes a `vcpkg.json`, replace this step with: SDL3 is restored automatically on build.]
3. Open `Chip-8.sln` in Visual Studio and build.
## Running
 
Pass the path to a ROM as the only argument:
 
```
Chip-8.exe path\to\rom.ch8
```
 
From Visual Studio, put the ROM path in Project Properties > Debugging > Command Arguments.
 
ROMs are not included in this repository.
 
## Controls
 
The CHIP-8 keypad is mapped onto the left side of a QWERTY keyboard:
 
| CHIP-8 | | | |
|---|---|---|---|
| `1` | `2` | `3` | `C` |
| `4` | `5` | `6` | `D` |
| `7` | `8` | `9` | `E` |
| `A` | `0` | `B` | `F` |
 
| Keyboard | | | |
|---|---|---|---|
| `1` | `2` | `3` | `4` |
| `Q` | `W` | `E` | `R` |
| `A` | `S` | `D` | `F` |
| `Z` | `X` | `C` | `V` |
 
`Esc` quits.
 
## Project structure
 
- `chip_8.h` / `chip_8.cpp`: the emulator core (memory, registers, opcodes, timers)
- `platform.h` / `platform.cpp`: the SDL layer (window, texture, input)
- `main.cpp`: argument handling, the main loop, and frame timing
## Testing
 
Developed against the [chip8-test-suite](https://github.com/Timendus/chip8-test-suite) by Timendus, starting with the IBM logo ROM.

