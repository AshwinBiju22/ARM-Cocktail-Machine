# ARMv8 Assembler, LED Control, and Cocktail Machine Project

## Overview
This project was developed as part of a group assignment, combining **low-level programming**, **hardware interfacing**, and **embedded systems**.  
It consists of three main parts:
1. **A C-based ARMv8 assembler** – converts ARMv8 assembly instructions into machine code.
2. **An ARMv8 assembly LED control program** – flashes an LED via Raspberry Pi GPIO pins.
3. **An extension project: Classy Unit Mixer** – an automated cocktail mixing machine controlled by a Raspberry Pi.

---

## Features

### 1️ARMv8 Assembler
- **Two-pass architecture** for improved clarity and debugging.
- Uses **hashmap-based symbol table** for efficient label lookups.
- **Intermediate representation (IR)** design decouples parsing and assembling.
- Tokenization and parsing combined in `parseLine` for streamlined processing.
- Bitwise encoding of instructions inspired by the reverse of the emulator’s decoding process.
- **Testing & Debugging**:
  - Systematic test case isolation using gdb.
  - Binary search approach to pinpoint issues.
  - Clear, compartmentalized code for easier fixes.

---

### LED Control Program
- Fully parametric: arbitrary **pin selection** and **timing** via `.int` directives.
- Dynamic calculation of memory addresses and bitmasks for **GPFSEL**, **GPSET**, and **GPCLR** registers.
- Simple **on/off loop** with delay cycles.
- Supports both low (<32) and high (≥32) GPIO pins.

---

### Extension: Classy Unit Mixer 🍹
**An automatic cocktail mixing machine**:
- **Hardware**:
  - 5 peristaltic pumps wired in parallel, each controlled by a MOSFET.
  - CAD-designed wooden chassis, laser-cut and slotted together.
  - Flexible food-safe tubing with custom 3D-printed connectors.
  - Reliable 5A power supply after troubleshooting voltage/current issues.
- **Software**:
  - Built with **pigpio** library for GPIO control.
  - **Finite State Machine (FSM)** architecture for menu and dispensing logic.
  - Multithreaded dispensing for speed (parallel pump control).
  - Debounce logic for button inputs.
  - Configurable via `#define` macros for easy recipe changes.
  - Installed as a **systemd service** for auto-start and crash recovery.
- **Testing**:
  - Flow rate calibration for accurate pouring.
  - Breadboard prototyping with LEDs and buttons before final assembly.

---

## Tech Stack
- **Languages**: C, ARMv8 Assembly
- **Hardware**: Raspberry Pi, peristaltic pumps, MOSFETs, laser-cut chassis
- **Libraries**: `pigpio` (GPIO control), standard C libraries
- **Tools**: gdb, valgrind, CAD software (Autodesk Fusion), laser cutter, 3D printer
- **OS**: Raspberry Pi OS (Linux)

---
## Video Demonstration

Demo : https://drive.google.com/file/d/1PTIabSDr8Tl54NqXwSD3Z43qlzNFioip/view?usp=sharing

Fun Promo Video : https://drive.google.com/file/d/1I7sgab0-bYzxRG-lEcvISB-Q01in2dsy/view?usp=sharing

---

# Reminders for Team:

## Branching:

### Pulling branch
```bash
git checkout "<branch>"
git pull origin "<branch>"
```

### Pushing branch
```bash
git add .
git commit -m "<message>"
git push origin "<branch>"
```

### Merging
Merge using Gitlab

## Conventional Commit Syntax:
```
****************************************
** <type>(scope): <short description> **
****************************************
```
### Commit Types
```md
| Type       | Use Case                                                                 |
|------------|--------------------------------------------------------------------------|
| `feat`     | A new feature (e.g: `feat: add user login functionality`)                |
| `fix`      | A bug fix (e.g: `fix: correct password validation logic`)                |
| `perf`     | A code change that improves performance                                  |
| `refactor` | A code change that neither fixes a bug nor adds a feature                |
| `chore`    | Maintenance tasks (e.g: `build system config, tooling, etc`)             |
| `docs`     | Documentation only changes                                               |
| `style`    | Changes that don’t affect meaning (e.g: `whitespace, formatting, etc`)   |
| `test`     | Adding or updating tests                                                 |
| `build`    | Changes that affect the build system or dependencies (e.g: `npm`)        |
| `ci`       | Changes to CI/CD configuration (e.g: `GitHub Actions, CircleCI`)         |
| `revert`   | Reverting a previous commit                                              |
```
