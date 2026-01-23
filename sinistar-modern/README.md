# SINISTAR - Modern Cross-Platform Recreation

<div align="center">

**A faithful recreation of the 1982 Williams arcade classic**

🎮 **Built with SDL2 + C++17** 🎮

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Windows%20%7C%20Linux-blue)]()
[![C++](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B)]()
[![SDL2](https://img.shields.io/badge/SDL2-2.0+-red?logo=sdl)]()

</div>

---

## 🌟 Overview

This is a modern, cross-platform recreation of the legendary **Sinistar** arcade game (Williams Electronics, 1982). Built from the ground up using modern C++ and SDL2, it preserves the intense gameplay, strategic depth, and terrifying boss encounters of the original while adding enhanced graphics, sound, and cross-platform support.

### Original 6809 Assembly → Modern C++17

The original game ran on a Motorola 6809 processor with custom Williams arcade hardware. This recreation analyzes the original assembly codebase (~26,600 lines across 186 files) and recreates the gameplay using modern technologies while maintaining the spirit and mechanics of the original.

---

## ✨ Features

### Complete Arcade Experience
- ✅ **Full gameplay loop**: Mine crystals, destroy enemies, battle Sinistar
- ✅ **Asteroids-style physics**: Thrust, inertia, drag, screen wrapping
- ✅ **Multiple enemy types**: Warriors (4 AI types) and Workers (4 states)
- ✅ **Crystal mining system**: Planetoids with 3 size variants
- ✅ **Epic boss battle**: Sinistar with 7 states and iconic voice lines
- ✅ **Particle effects**: Explosions, impacts, visual feedback (1000 particles)
- ✅ **Sound effects**: 8 different sound types with SDL2_mixer
- ✅ **Score system**: Points, kills, accuracy tracking, persistent high scores
- ✅ **Level progression**: Dynamic difficulty scaling (up to 3× multiplier)
- ✅ **Game states**: Main menu, playing, paused, game over

### Modern Enhancements
- 🎯 **60 FPS**: Smooth gameplay with VSync
- 🎨 **Enhanced visuals**: Particle systems, pulsing effects, color-coded entities
- 🎮 **Dual input**: Full keyboard + gamepad support with auto-detection
- 📊 **Comprehensive HUD**: Lives, shields, score, level, kills, Sinistar status bars
- 💾 **Persistent high scores**: Top 10 scores saved to disk
- ⚙️ **Cross-platform**: Runs on macOS, Windows, and Linux

---

## 🎮 How to Play

### Objective
Destroy the Sinistar boss before it destroys you! But first, you must survive waves of enemies and prevent Workers from building Sinistar by delivering crystals.

### Gameplay Loop

1. **Early Game: Survival**
   - Destroy attacking **Warrior** enemies (red diamonds)
   - Mine **Crystal** planetoids for points (green octagons)
   - Stop **Workers** from collecting crystals (blue hexagons)

2. **Mid Game: Construction**
   - Workers deliver crystals to build Sinistar (center of screen)
   - Construction progress shown at top: "SINISTAR: X%"
   - Decide: prevent construction or prepare for boss fight

3. **Late Game: Boss Battle**
   - Sinistar awakens: **"BEWARE, I LIVE!"** (huge red triangle)
   - Boss chases you relentlessly at high speed
   - Dodge bite attacks (1.0 HP damage each!)
   - Land 20 hits to destroy Sinistar (1.0 damage per hit)
   - Victory: **10,000 points!**

### Strategy Tips

**Crystal Management**:
- Destroy crystals early to prevent Workers from mining them
- Each crystal destroyed = delayed Sinistar construction
- Small crystals: 25 pts, Medium: 75 pts, Large: 125 pts

**Worker Interception**:
- Workers carrying crystals glow green and are worth 150 points (vs 50 empty)
- Intercept Workers before they deliver to screen center
- Workers flee when you approach within 80 units

**Boss Combat**:
- Sinistar is **faster than you** (180 vs 150 speed) - can't outrun forever!
- Maintain distance > 40 units to avoid bite attack
- Use hit-and-run tactics with screen wrapping
- Aim carefully - you need exactly 20 hits!
- Health bar shows progress at top of screen

**Difficulty Scaling**:
- Each level: 10% faster enemies, more spawns, faster spawn rate
- Level progression: 10k → 25k → 45k → 70k points
- Difficulty multiplier caps at 3× (level 20+)
- Higher levels = more points per kill

---

## 🕹️ Controls

### Keyboard

| Action | Key(s) |
|--------|--------|
| **Rotate Left** | Left Arrow / A |
| **Rotate Right** | Right Arrow / D |
| **Thrust** | Up Arrow / W |
| **Fire Weapon** | Space / Left Ctrl |
| **Pause Game** | P |
| **Quit** | ESC |

### Gamepad (Auto-Detected)

| Action | Button |
|--------|--------|
| **Rotate/Move** | Left Stick / D-Pad |
| **Thrust** | A (Xbox) / Cross (PlayStation) |
| **Fire** | B (Xbox) / Circle (PlayStation) |
| **Pause** | Start |
| **Quit** | Back / Select |

### Menu Navigation
- **Up/Down**: Arrow keys, WASD, or gamepad
- **Select**: Space, Enter, or gamepad button

---

## 📦 Installation & Building

### Prerequisites

**macOS**:
```bash
brew install cmake sdl2 sdl2_mixer sdl2_image
```

**Linux (Ubuntu/Debian)**:
```bash
sudo apt-get install cmake libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev g++
```

**Windows**:
- Install CMake from https://cmake.org/
- Install SDL2 via vcpkg or download from https://www.libsdl.org/
- Requires Visual Studio 2019+ or MinGW

### Building from Source

```bash
# Clone repository
git clone <repository-url>
cd sinistar-modern

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build (use all CPU cores)
make -j$(nproc)        # macOS/Linux
cmake --build . -j     # Windows

# Run the game!
./bin/Sinistar         # macOS/Linux
bin\Release\Sinistar   # Windows
```

### macOS-Specific Notes

If CMake can't find SDL2:
```bash
export CMAKE_PREFIX_PATH=/opt/homebrew  # Homebrew on Apple Silicon
# or
export CMAKE_PREFIX_PATH=/usr/local     # Homebrew on Intel
```

Create macOS .app bundle:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make
open bin/Sinistar.app
```

### Build Options

```bash
# Debug build (with symbols and logging)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Specify compiler
cmake .. -DCMAKE_CXX_COMPILER=clang++

# Disable tests
cmake .. -DBUILD_TESTS=OFF
```

---

## 🎯 Game Entities

### Player Ship (Cyan Triangle)
- **Health**: 4.0 HP (shield strength)
- **Speed**: 150 units/sec (maximum)
- **Rotation**: 3.0 radians/sec
- **Weapons**: Sinibombs (unlimited ammo, 0.2s cooldown)
- **Shield Recharge**: 0.2 HP/sec when undamaged for 1 second
- **Lives**: Starts with 3

### Warrior Enemies (Red Diamonds)
- **Health**: 1.0 HP (one-hit kill)
- **Speed**: 120 units/sec
- **Worth**: 100 points × difficulty multiplier
- **4 AI Mission Types**:
  - **ATTACK**: Direct assault on player
  - **CHASE**: Pursuit with evasive sine-wave maneuvers
  - **INTERCEPT**: Predicts player trajectory and cuts them off
  - **DRIFT**: Random wandering movement

### Worker Enemies (Blue Hexagons)
- **Health**: 1.0 HP (one-hit kill)
- **Speed**: 100 units/sec (slower than Warriors)
- **Worth**: 50 points base, 150 points if carrying crystal
- **4 AI States**:
  - **SEARCHING**: Looking for nearest crystal planetoid
  - **MINING**: Extracting crystal at 0.3 crystals/second
  - **RETURNING**: Delivering crystal to Sinistar build site (screen center)
  - **FLEEING**: Running from player when within 80 units

### Crystal Planetoids (Colored Octagons)
- **Three Size Variants**:
  - **Small**: 1 crystal, 2 HP, 10-unit radius (60% spawn chance) - Green
  - **Medium**: 3 crystals, 4 HP, 14-unit radius (30% spawn chance) - Yellow
  - **Large**: 5 crystals, 6 HP, 18-unit radius (10% spawn chance) - Red when low HP
- **Worth**: 25 points per crystal contained
- **Behavior**: Can be destroyed by player OR mined by Workers
- **Visual**: Rotating octagons with pulsing glow, color changes with health

### SINISTAR BOSS (Massive Red Triangle)
- **Health**: 20.0 HP (requires 20 hits to destroy!)
- **Speed**: 180 units/sec (FASTER than player!)
- **Radius**: 30 units (3× larger than player)
- **Worth**: **10,000 points** × difficulty multiplier
- **Construction**: Requires 20 crystals delivered by Workers
- **Bite Attack**: 1.0 HP damage, 40-unit range, 2-second cooldown
- **7 Boss States**:
  1. **BUILDING**: Under construction (gray, rotating, progress bar)
  2. **AWAKENING**: 3-second birth animation (pulsing orange)
  3. **HUNTING**: Aggressive chase mode (red, fast)
  4. **ATTACKING**: Close combat preparation (jaw opening)
  5. **BITING**: Lunge attack with jaw snap (heavy damage!)
  6. **DYING**: 2-second death sequence (spinning, shrinking)
  7. **DEAD**: Completely destroyed (victory!)
- **Famous Voice Lines**:
  - "BEWARE, I LIVE!" (awakening)
  - "RUN! RUN! RUN!" (hunting taunt)
  - "I HUNGER!" (attacking)
  - "I AM SINISTAR!" (biting)
  - "NOOOOOO!" (death scream)

---

## 📊 Scoring System

### Point Values

| Action | Points |
|--------|--------|
| Destroy Warrior | 100 × difficulty multiplier |
| Destroy Worker (empty) | 50 × difficulty multiplier |
| Destroy Worker (carrying crystal) | 150 × difficulty multiplier |
| Destroy Crystal (small, 1 crystal) | 25 |
| Destroy Crystal (medium, 3 crystals) | 75 |
| Destroy Crystal (large, 5 crystals) | 125 |
| Hit Sinistar | 50 |
| **Destroy Sinistar** | **10,000 × difficulty multiplier** |

### Level Progression

| Level | Points Needed | Difficulty | Max Enemies | Spawn Rate | Example Points |
|-------|---------------|------------|-------------|------------|----------------|
| 1 | 0 | 1.0× | 10 | 2.0s | 100 per Warrior |
| 2 | 10,000 | 1.1× | 12 | 1.9s | 110 per Warrior |
| 3 | 25,000 | 1.2× | 14 | 1.8s | 120 per Warrior |
| 5 | 55,000 | 1.4× | 18 | 1.6s | 140 per Warrior |
| 10 | 145,000 | 1.9× | 28 | 1.1s | 190 per Warrior |
| 20+ | 395,000+ | 3.0× (max) | 30 | 0.5s | 300 per Warrior |

### High Scores
- **Top 10 saved** to `highscores.dat` file
- Persists across game sessions
- Format: 3-letter name, score, level achieved
- Default high scores: SIN (100k), WAR (75k), CRY (50k)...

---

## 🎨 Technical Details

### Architecture

**Technology Stack**:
- **Language**: C++17 with STL
- **Graphics**: SDL2 2.0+ (cross-platform windowing, rendering, events)
- **Audio**: SDL2_mixer (optional, 8 sound effects)
- **Build**: CMake 3.15+ (cross-platform build system)
- **Patterns**: Entity-Component, State Machine, Singleton, Task-based

**Core Game Systems**:
- **GameEngine**: Main game loop, entity management, frame timing (60 FPS)
- **TaskManager**: Task-based execution system (replicates original ADDTASK/DROPTASK)
- **PhysicsSystem**: Velocity integration, drag, bounce, screen wrapping
- **ParticleSystem**: Explosion/impact effects (max 1000 particles, fade-out)
- **AudioManager**: Sound effect playback with volume control
- **ScoreManager**: Points, levels, high scores, difficulty scaling
- **GameStateManager**: State machine (menu/playing/paused/game over)
- **InputSystem**: Unified keyboard + gamepad input with button state tracking

**Entity Hierarchy**:
```
GameObject (base class)
├── Player (cyan triangle, full controls)
├── Sinibomb (yellow projectile, 2s lifetime)
├── Warrior (red diamond, 4 AI mission types)
├── Worker (blue hexagon, 4 AI states)
├── Crystal (octagon, 3 size variants)
└── Sinistar (red triangle boss, 7 states)
```

### Performance Metrics

- **Target Frame Rate**: 60 FPS (with VSync)
- **Logical Resolution**: 256×244 pixels (original arcade resolution)
- **Window Resolution**: 768×732 pixels (3× scaling, resizable)
- **Max Simultaneous Entities**: ~60 total
  - Enemies: 10-30 (scales with level)
  - Workers: 8 max
  - Crystals: 15 max
  - Projectiles: 10-20
  - Particles: 1000 max
- **Memory Usage**: ~10MB runtime
- **Binary Size**: 191KB (optimized release build)
- **Frame Time**: ~2ms average (plenty of headroom for 60 FPS)

### Code Statistics

- **Total Lines**: ~5,500 lines of C++ code
- **Source Files**: 24 .cpp files
- **Header Files**: 24 .h files
- **Phases Completed**: 10/10 (100%)
- **Build Time**: ~15 seconds (clean build, parallel compilation)

### File Structure

```
sinistar-modern/
├── src/
│   ├── core/              # Game engine, state, score managers
│   │   ├── GameEngine.cpp/h
│   │   ├── GameState.cpp/h
│   │   ├── ScoreManager.cpp/h
│   │   ├── TaskManager.cpp/h
│   │   └── Math.cpp/h
│   ├── entities/          # All game entities
│   │   ├── Player.cpp/h
│   │   ├── Warrior.cpp/h
│   │   ├── Worker.cpp/h
│   │   ├── Crystal.cpp/h
│   │   ├── Sinistar.cpp/h
│   │   └── Sinibomb.cpp/h
│   ├── systems/           # Core game systems
│   │   ├── PhysicsSystem.cpp/h
│   │   └── RenderSystem.cpp/h
│   ├── graphics/          # Visual systems
│   │   ├── Particle.cpp/h
│   │   ├── TextRenderer.cpp/h
│   │   └── Color.cpp/h
│   ├── audio/             # Sound system
│   │   └── AudioManager.cpp/h
│   └── input/             # Input handling
│       └── InputSystem.cpp/h
├── assets/                # Future: sprites, sounds
├── build/                 # CMake build directory
├── docs/                  # Phase completion docs
├── CMakeLists.txt         # Build configuration
├── README.md              # This file
└── LICENSE                # MIT license
```

---

## 🐛 Known Issues & Limitations

### Cosmetic Build Warnings (Non-Critical)
- **Virtual function hiding**: Design pattern for render() methods (no impact)
- **Initialization order**: Member initialization order warnings (no runtime impact)
- **Uninitialized variables**: False positives from compiler (all paths initialize)

### Platform-Specific Notes
- **Headless Linux**: No audio/video output in headless environments (expected)
- **macOS Homebrew**: May need `export CMAKE_PREFIX_PATH=/opt/homebrew`
- **Windows**: Requires SDL2 DLLs in same directory as executable

### Current Limitations vs Original
- **Voice samples**: Text display instead of actual synthesized speech
- **Sprite assets**: Procedural graphics instead of original sprites
- **Sound effects**: Placeholder effects instead of original samples
- **Planetoids**: No asteroid field (focus on crystals instead)

### Future Enhancements (Post-Release)
- [ ] Real voice sample playback for Sinistar lines
- [ ] HD sprite assets with retro and HD modes
- [ ] Complete original sound effects
- [ ] CRT shader effects for authentic arcade feel
- [ ] Networked multiplayer support
- [ ] Leaderboard integration
- [ ] Achievement system

---

## 📜 Credits & Attribution

### Original Game (1982)
- **Developer**: Williams Electronics
- **Designers**: Noah Falstein, John Newcomer, Python Anghelo
- **Programmer**: Sam Dicker
- **Speech Synthesis**: "Run Coward!" voice by John Dykstra
- **Hardware**: Motorola 6809 assembly on Williams arcade board

### This Recreation (2026)
- **Implementation**: Modern C++17 recreation from 6809 assembly analysis
- **Architecture**: Analyzed 186 original source files (~26,600 lines)
- **Design**: Faithful recreation with modern enhancements
- **Libraries Used**:
  - SDL2 (Simple DirectMedia Layer) - zlib license
  - SDL2_mixer - zlib license
  - CMake - BSD-3-Clause license

### Special Thanks
- Williams Electronics for creating the original masterpiece
- SDL development team for excellent cross-platform library
- Open source community for tools and support

---

## 📄 License

This project is released under the **MIT License**. See `LICENSE` file for full details.

### Important Legal Notice

**This is a fan recreation for educational and preservation purposes.**

- Sinistar™ is a trademark of WMS Gaming Inc.
- This project is **not affiliated with, endorsed by, or sponsored by** Williams Electronics or WMS Gaming
- Original game assets, code, and trademarks remain property of their respective owners
- This recreation uses original game mechanics and design concepts under fair use for educational purposes

**For commercial use, please contact the trademark holders.**

---

## 🚀 Quick Start Guide

### macOS Quick Start
```bash
# Install dependencies
brew install cmake sdl2 sdl2_mixer sdl2_image

# Clone and build
git clone <repository-url>
cd sinistar-modern && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)

# Play!
./bin/Sinistar
```

### Linux Quick Start
```bash
# Install dependencies
sudo apt-get install cmake libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev

# Clone and build
git clone <repository-url>
cd sinistar-modern && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Play!
./bin/Sinistar
```

### Windows Quick Start
```powershell
# Install dependencies (using vcpkg)
vcpkg install sdl2 sdl2-mixer sdl2-image

# Clone and build
git clone <repository-url>
cd sinistar-modern
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release

# Play!
bin\Release\Sinistar.exe
```

---

## 🎯 Development Timeline

All 10 development phases completed:

- ✅ **Phase 1**: Foundation & Architecture (GameEngine, TaskManager, basic rendering)
- ✅ **Phase 2**: Core Systems (Math, Physics, Colors, Text rendering)
- ✅ **Phase 3**: Game Entities (Player ship, Sinibombs, Warriors with 4 AI types)
- ✅ **Phase 4**: Collision & Effects (Circle collision, particle system, visual feedback)
- ✅ **Phase 5**: Audio System (SDL2_mixer integration, 8 sound effects)
- ✅ **Phase 6**: UI & Game Flow (Menus, pause, game states, state machine)
- ✅ **Phase 7**: Scoring & Progression (Points, levels, high scores, difficulty scaling)
- ✅ **Phase 8**: Crystal Mining (Crystal planetoids, Worker AI, mining mechanics)
- ✅ **Phase 9**: Sinistar Boss (7-state boss, construction system, voice lines)
- ✅ **Phase 10**: Polish & Optimization (HUD improvements, balancing, documentation)

**Status**: ✅ **COMPLETE AND FULLY PLAYABLE!**

**Total Development**: 10 phases, ~5,500 lines of code, 24 source files

---

<div align="center">

## "BEWARE, I LIVE!" 🔴👾

### **The terror of the arcade. Recreated for modern systems.**

**Can you destroy Sinistar before it destroys you?**

### **RUN. HIDE. FIGHT.**

---

![Sinistar](https://img.shields.io/badge/Status-COMPLETE-brightgreen?style=for-the-badge)
![Playable](https://img.shields.io/badge/Playable-YES-success?style=for-the-badge)

**Download • Build • Play • Survive**

</div>
