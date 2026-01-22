# Sinistar Modern

A cross-platform remake of the classic 1982 arcade game **Sinistar** by Williams Electronics.

![Sinistar](https://img.shields.io/badge/Status-In%20Development-yellow)
![Platform](https://img.shields.io/badge/Platform-macOS%20%7C%20Windows-blue)
![C++](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B)
![SDL2](https://img.shields.io/badge/SDL2-2.0+-red?logo=sdl)

---

## About

This project recreates the legendary arcade game Sinistar using modern C++ and SDL2, making it playable on current macOS and Windows systems while preserving the original gameplay and feel.

### Original Game (1982)
- **Developer**: Williams Electronics
- **Platform**: Arcade cabinet (Motorola 6809 processor)
- **Features**: Revolutionary task-based architecture, pixel-perfect collision detection, digitized speech
- **Developers**: Sam Dicker, Rich Witt, Noah Falstein, Bob Mical

### This Recreation
- **Language**: C++17
- **Framework**: SDL2 (cross-platform graphics, input, audio)
- **Build System**: CMake
- **Target Platforms**: macOS (Intel + Apple Silicon), Windows 10/11

---

## Features (Planned)

- ✅ **Cross-platform**: Native builds for macOS and Windows
- ✅ **Authentic gameplay**: Faithful recreation of original mechanics
- ⬜ **Pixel-perfect collision**: Matching original arcade accuracy
- ⬜ **Warrior AI**: Seven mission types (Attack, Chase, Follow, Intercept, etc.)
- ⬜ **Classic audio**: Original voice samples and sound effects
- ⬜ **High scores**: Persistent leaderboard
- ⬜ **Controller support**: Gamepad and keyboard controls
- ⬜ **Modern enhancements**: HD graphics option, CRT shader effects

✅ = Implemented | ⬜ = Planned

---

## Current Status

**Phase 1: Foundation** ✅ **COMPLETE**
- Project structure created
- CMake build system configured
- Core game engine implemented
- Task management system (replicates original ADDTASK/DROPTASK)
- Basic window and rendering system

**Next**: Phase 2 - Core Systems (Physics, Math, Display)

See [`CROSS_PLATFORM_PLAN.md`](../CROSS_PLATFORM_PLAN.md) for the complete development roadmap.

---

## Quick Start

### Prerequisites

- **CMake** 3.15+
- **SDL2** 2.0+
- **C++17 compiler** (Xcode/Clang on macOS, Visual Studio 2019+ on Windows)

### macOS

```bash
# Install dependencies
brew install cmake sdl2

# Build
mkdir build && cd build
cmake ..
cmake --build .

# Run
./bin/Sinistar
```

### Windows

```powershell
# Install dependencies (using vcpkg)
vcpkg install sdl2:x64-windows

# Build
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build . --config Release

# Run
.\bin\Release\Sinistar.exe
```

For detailed build instructions, see [**BUILD.md**](BUILD.md).

---

## Project Structure

```
sinistar-modern/
├── src/
│   ├── core/           # Game loop, task system, timing
│   │   ├── GameEngine.cpp/h
│   │   ├── TaskManager.cpp/h
│   │   └── Timer.cpp
│   ├── entities/       # Game objects (Player, Warriors, Sinistar)
│   │   └── GameObject.cpp/h
│   ├── systems/        # Collision, Physics, Rendering, Audio
│   │   └── RenderSystem.cpp/h
│   ├── ai/             # Warrior AI, pathfinding (planned)
│   ├── graphics/       # Rendering, sprites, effects (planned)
│   ├── audio/          # Sound effects, music (planned)
│   ├── input/          # Keyboard, gamepad (planned)
│   ├── ui/             # Menus, HUD, messages (planned)
│   └── main.cpp        # Entry point
├── assets/
│   ├── sprites/        # Game graphics (to be extracted)
│   ├── sounds/         # Audio files (to be extracted)
│   ├── fonts/          # Text rendering
│   └── config/         # Game configuration
├── tests/              # Unit tests (planned)
├── CMakeLists.txt      # Build configuration
├── BUILD.md            # Detailed build instructions
└── README.md           # This file
```

---

## Architecture

### Core Systems

1. **GameEngine** - Main game loop, runs at 60 FPS like original arcade
2. **TaskManager** - Replicates original task-based multitasking (ADDTASK/DROPTASK)
3. **GameObject** - Base class for all entities (replicates object workspace structure)
4. **RenderSystem** - Handles all graphics rendering

### Design Philosophy

This project **recreates the game logic** rather than emulating the hardware:
- ✅ Maintains original behavior and feel
- ✅ Clean, modern C++ codebase
- ✅ Native performance
- ✅ Easy to extend and modify

---

## Controls (Current)

- **ESC**: Quit

*(Full controls will be added as gameplay is implemented)*

---

## Development Roadmap

See [`CROSS_PLATFORM_PLAN.md`](../CROSS_PLATFORM_PLAN.md) for the complete 12-phase development plan.

**Current Phase**: Phase 1 - Foundation ✅ Complete

**Next Phases**:
1. Phase 2: Core Systems (Math, Physics, Display)
2. Phase 3: Game Entities (Player, Warriors, Sinistar)
3. Phase 4: Collision Detection
4. Phase 5: Audio System

**Estimated Timeline**: 16-20 weeks total

---

## Contributing

This project is in early development. Contributions welcome once core systems are stable.

### Development Setup

1. Read [BUILD.md](BUILD.md) for build instructions
2. Check [CROSS_PLATFORM_PLAN.md](../CROSS_PLATFORM_PLAN.md) for roadmap
3. Browse original source in `../SAM/`, `../WITT/`, `../MICA/`, `../FALS/`

---

## Technical Details

### Original Game Architecture

The original Sinistar used a sophisticated task-based system where every game entity was a "task" executed by the executive loop. We've recreated this architecture in modern C++:

| Original (6809 Assembly) | Modern (C++) |
|-------------------------|--------------|
| ADDTASK | `TaskManager::addTask()` |
| DROPTASK | `TaskManager::removeTask()` |
| EXEC loop | `GameEngine::run()` |
| Object workspace (OSPOS, OLPOS, etc.) | `GameObject` class members |
| DRAWOBJ | `RenderSystem::render()` |

### Performance

- **Target**: 60 FPS (matching original arcade)
- **Resolution**: 256x244 logical (scaled to window size)
- **Rendering**: Hardware-accelerated SDL2 with VSync

---

## License

To be determined. This is a recreation of a copyrighted game.

**Original Sinistar** © 1982 Williams Electronics
**Current Rights**: Warner Bros. Interactive Entertainment

This project is for educational and preservation purposes. Please respect intellectual property rights.

---

## Resources

- **Original Source Code**: See parent directory (`../SAM/`, `../WITT/`, etc.)
- **MAME Reference**: [MAME Sinistar Driver](https://github.com/mamedev/mame/blob/master/src/mame/drivers/williams.cpp)
- **SDL2 Documentation**: https://wiki.libsdl.org/
- **Build Instructions**: [BUILD.md](BUILD.md)
- **Development Plan**: [CROSS_PLATFORM_PLAN.md](../CROSS_PLATFORM_PLAN.md)

---

## Acknowledgments

- **Original Developers**: Sam Dicker, Rich Witt, Noah Falstein, Bob Mical
- **Williams Electronics** for creating the original game
- **MAME Project** for preservation and documentation
- **SDL2 Team** for the excellent cross-platform framework

---

**Version**: 1.0.0-alpha
**Status**: Foundation Phase Complete
**Last Updated**: 2026-01-22

*"RUN COWARD!"* - Sinistar
