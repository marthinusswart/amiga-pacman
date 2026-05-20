# Amiga Pac-Man (Experimental Learning Project)

This project is an experimental Pac-Man clone developed for the Commodore Amiga.

## Project Purpose

This codebase serves primarily as a **learning approach to understand low-level Amiga graphics programming**. It explores how classic games were built on the Amiga hardware, blending modern C development (using the Bartman/Abyss VSCode toolchain) with direct hardware manipulation.

## Key Concepts Explored

- **Low-Level Hardware Access:** Direct manipulation of the Amiga's custom chip registers (e.g., `DMACON`, `INTENA`, `ADKCON`).
- **The Copper:** Generating and managing custom Copper lists to control display synchronization and palette colors.
- **Bitplanes & Double Buffering:** Managing planar graphics memory and swapping screen buffers for smooth, tear-free rendering.
- **The Blitter:** Utilizing the Amiga's blitter for high-speed graphics copying, masking, and erasing entity backgrounds.
- **Custom Interrupts:** Safely suspending the Amiga OS multitasking environment to run custom VBlank interrupt handlers.
- **Audio:** Playing classic tracker module music (`.p61`) utilizing ThePlayer 6.1a.
- **Framework Integration:** Leveraging parts of the ACE (Amiga C Engine) framework while intentionally writing custom routines to bypass it where bare-metal learning is desired.
- **Game AI:** Implementing classic arcade pathfinding algorithms (Chase, Ambush, Random) for the ghost behaviors.

## Disclaimer

As an experimental learning repository, this project contains a mix of high-level C logic and bare-metal hardware hacks. Expect to find rough edges, memory management experiments, and non-standard workarounds as part of the educational journey into retro Amiga development.

## Toolchain & Setup

This project is built using a modern cross-compilation toolchain that targets the Amiga's Motorola 680x0 processor from a contemporary development environment (Windows, macOS, or Linux).

### Bartman/Abyss VSCode Extension

The core of the development environment is the **Amiga C/C++ Compile, Debug & Profile** extension for Visual Studio Code, created by Bartman/Abyss. This extension provides a seamless "out-of-the-box" experience by:

- Automatically downloading and configuring the `m68k-amigaos` GCC cross-compiler.
- Integrating with a `makefile` to build the project.
- Providing debugging capabilities that connect directly to an Amiga emulator (like WinUAE).
- Offering performance profiling tools.

### Installation Instructions

1.  Install Visual Studio Code.
2.  Open VS Code and navigate to the **Extensions** view (or press `Ctrl+Shift+X`).
3.  In the search bar, type `Amiga C/C++ Compile, Debug & Profile`.
4.  Find the extension by **Bartosz Błasiak (bartman/abyss)** and click **Install**.
5.  The extension will handle the download and setup of the Amiga cross-compiler toolchain automatically.

### Emulator Configuration

You will also need an Amiga emulator to run and debug the compiled executable.

- **Recommended:** WinUAE (Windows) or FS-UAE (Cross-platform).
- Configure the emulator for a standard Amiga 500 (OCS/ECS chipset) or Amiga 1200 (AGA chipset) environment. The VS Code extension can connect to the emulator's debugger for step-through debugging.
