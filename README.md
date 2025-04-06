# NTHP Game Engine

This is a personal project of mine, spanning about 3 years. It has gone through mutliple
iterations, with this one being the most usable and stable.

The engine runtime itself is written entirely in C++ (very C-Style C++), and is usable as
a header-only library. However, most of my time is spent on improving the proprietary scripting language
that's integrated into the runtime itself. Script source files can be compiled, linked, and executed
by the runtime to make games, without having to recompile the binaries.

## How to build

  NTHP is pretty bare, and has been tested on Windows 10/11, as well as Linux.
  Use the included build systems for either platform (winMake on Windows, Makefile for Linux).
  Both these files must be configured with dependency paths to build correctly; use
  `make` on Linux and `nmake` (from **Visual Studio Build Tools**) on Windows

## Dependencies

### Windows
1. Manual installations of:
    - **SDL2**
    - SDL2_mixer
    - SDL2_image (optional)

2. Standard installation of:
    - Visual Studio Build Tools VC++ for x64 (includes both "cl" and "nmake")

  The install locations for the manual dependencies must be configured in the build system
  (under variables **LibPath**)

  SDL_image can be disabled in the build system as standard image formats are not used
  by the runtime. the Project Manager (pm.exe, pm) requires SDL_image to allow conversions of
  standard formats (PNG, JPEG) into ST or CST formats usable by the runtime.

  Run `winb.bat` to build, which targets 'winMake' and passes any command line parameters to nmake.

### Linux
1. Standard package manager installations of:
    - **SDL2**
    - SDL2_mixer
    - SDL2_image (optional)
    - make
    - g++

Assumes the library binaries are already configured by the package manager. The build system is quite simple,
so worst case some modifications will be needed.

Run `make` to build.

### See build system comments for more details.

# Executables

If the build is successful, 3 programs should be created: **run**, **run_debug**, and **pm**. Run and run_debug are
the base runtime executables, with run_debug including debug symbols and output (for debugging, no shit). Both these programs
take no parameters from the command line, and will attempt to execute a linked script file called **"m_prog.p"** in the local directory. Nothing will
happen if this file isn't found.

PM (stands for Project Manager) is a tool to prepare anything externally required by the engine. PM can compile and link script files, convert textures,
and debug any script executable, using an integrated debugger. The integrated debugger can use breakpoints, step through instructions, write memory,
jump, and import compiler symbols from any script build system (run "help" in the PM command line for more).
    
    
