# Sinistar Cross-Platform Modernization Plan

## Executive Summary

This document outlines a comprehensive plan to create a modern, cross-platform version of the classic 1982 Sinistar arcade game that runs on both macOS and Windows (PC). The original codebase consists of ~26,600 lines of Motorola 6809 assembly code designed for Williams arcade hardware.

---

## Current State Analysis

### Technology Stack (Original)
- **Language**: 6809 Assembly Language (V17 assembler)
- **Platform**: Williams arcade cabinet hardware (1982)
- **Processor**: Motorola 6809 (8-bit, 1 MHz)
- **Memory**: ~32KB RAM
- **Display**: 256x244 pixel arcade display
- **Architecture**: Task-based multitasking with DMA system

### Key Components
1. **Executive Loop** - Main game loop with task dispatch
2. **Warrior AI** - Multiple mission types (Attack, Chase, Follow, Intercept, Mining)
3. **Collision Detection** - Pixel-perfect collision system
4. **Physics Engine** - Distance calculations, velocity, momentum
5. **Display System** - DMA-based rendering, character fonts (3x5, 6x8)
6. **Audio System** - Williams audio hardware interface
7. **Score System** - High score persistence via CMOS

### Code Organization
- **SAM** (54 files) - Core systems, display, objects, scoring
- **WITT** (84 files) - Warrior AI, collision detection, distance
- **FALS** (12 files) - Additional game logic
- **MICA** (36 files) - High-score, attract mode, UI

---

## Recommended Approach: Modern Game Engine Recreation

### Strategy
Rather than attempting to emulate the 6809 processor, we'll **recreate the game logic** using a modern cross-platform framework while maintaining the original game's behavior and feel.

### Why This Approach?
✅ Clean, maintainable codebase
✅ Native performance on modern hardware
✅ Easier to extend and modify
✅ Better debugging and development tools
✅ Modern graphics and audio capabilities
✅ Cross-platform from day one

---

## Technology Stack Recommendation

### Primary Framework: **SDL2 + C++**

#### Core Technologies
| Component | Technology | Rationale |
|-----------|-----------|-----------|
| **Language** | C++17/20 | Performance, cross-platform, object-oriented |
| **Graphics/Input/Audio** | SDL2 | Industry standard, cross-platform, proven |
| **Build System** | CMake | Cross-platform build configuration |
| **Graphics Rendering** | SDL2_gfx / Custom | Sprite rendering, pixel operations |
| **Audio** | SDL2_mixer | Multi-channel audio mixing |
| **Asset Management** | Custom + stb_image | Image loading, resource management |
| **Math Library** | GLM or custom | Vector math, physics calculations |
| **Configuration** | JSON (nlohmann/json) | Game settings, high scores |

#### Alternative Stack (If preferred)
- **Rust + SDL2**: For memory safety and modern language features
- **C# + MonoGame/FNA**: Easier development, managed memory
- **C++ + SFML**: Similar to SDL2, more C++-friendly API

---

## Development Phases

## Phase 1: Foundation & Architecture (Week 1-2)

### 1.1 Project Setup
- [ ] Initialize Git repository structure
- [ ] Set up CMake build system for Windows/macOS
- [ ] Configure CI/CD (GitHub Actions) for cross-platform builds
- [ ] Set up development environment documentation

### 1.2 Core Architecture
```
sinistar-modern/
├── src/
│   ├── core/           # Game loop, task system, timing
│   ├── entities/       # Game objects (Player, Warriors, Sinistar, etc.)
│   ├── systems/        # Collision, Physics, Rendering, Audio
│   ├── ai/             # Warrior AI, pathfinding
│   ├── graphics/       # Rendering, sprites, effects
│   ├── audio/          # Sound effects, music
│   ├── input/          # Keyboard, gamepad, mouse
│   └── ui/             # Menus, HUD, messages
├── assets/
│   ├── sprites/        # Game graphics
│   ├── sounds/         # Audio files
│   ├── fonts/          # Text rendering
│   └── config/         # Game configuration
├── tests/              # Unit tests
└── docs/               # Documentation
```

### 1.3 Core Classes Design
```cpp
class GameEngine {
    // Main game loop, task management
};

class TaskManager {
    // Replicate original task-based multitasking
};

class GameObject {
    // Base class for all game entities
    Vector2 position, velocity;
    int status, character;
};

class CollisionSystem {
    // Pixel-perfect collision detection
};

class DisplaySystem {
    // Rendering pipeline
};
```

---

## Phase 2: Core Systems Implementation (Week 3-4)

### 2.1 Game Loop & Task System
- [ ] Implement main executive loop matching original behavior
- [ ] Create task scheduling system (ADDTASK, DROPTASK equivalent)
- [ ] Implement timing system (60 FPS target, matching arcade)
- [ ] Vector hook system (POWVEC, IAMVEC, GAMVEC, EXEVEC, INTVEC)

### 2.2 Math & Physics System
- [ ] Port distance calculation algorithms (DISTCALC.SRC)
- [ ] Implement velocity and momentum system (VELOCITY.SRC)
- [ ] Bounce physics (BOUNCE.SRC)
- [ ] Angle calculations and vector math
- [ ] Wrapping coordinates for screen boundaries

### 2.3 Display System
- [ ] SDL2 window and renderer setup (256x244 logical resolution)
- [ ] Implement sprite rendering system
- [ ] Port character font system (3x5 and 6x8 fonts)
- [ ] Message display system (PHRASE.SRC)
- [ ] Color palette management
- [ ] Screen scaling for modern displays

---

## Phase 3: Game Entities (Week 5-7)

### 3.1 Player Ship
- [ ] Player movement and controls
- [ ] Ship rotation and thrust
- [ ] Shield system
- [ ] Weapon firing (Sinibombs)
- [ ] Player state management
- [ ] Lives and respawn system

### 3.2 Enemy Warriors
- [ ] Warrior base class
- [ ] AI mission system:
  - [ ] Attack mission
  - [ ] Chase mission
  - [ ] Follow mission
  - [ ] Intercept mission
  - [ ] Drift mission
  - [ ] Mining mission
  - [ ] Guard mission
- [ ] Warrior spawning system
- [ ] Formation flying
- [ ] Crystal carrying behavior

### 3.3 Sinistar Entity
- [ ] Sinistar construction sequence
- [ ] Movement toward player
- [ ] Attack behavior
- [ ] Destruction sequence
- [ ] Voice/audio triggers ("RUN COWARD!", "BEWARE, I LIVE!")

### 3.4 Other Entities
- [ ] Workers (crystal carriers)
- [ ] Crystals (collectibles with animation)
- [ ] Sinibombs (player weapons)
- [ ] Planetoids/asteroids
- [ ] Explosions and fragments
- [ ] Visual effects (particle systems)

---

## Phase 4: Collision Detection (Week 8)

### 4.1 Collision System
- [ ] Bounding box collision (broad phase)
- [ ] Pixel-perfect collision (PIXCHK.SRC equivalent)
- [ ] Collision matrix (what collides with what)
- [ ] Collision response handlers
- [ ] Optimize with spatial partitioning

### 4.2 Testing
- [ ] Collision detection test suite
- [ ] Performance profiling
- [ ] Edge case testing

---

## Phase 5: Audio System (Week 9)

### 5.1 Sound Effects
- [ ] Extract/recreate original sound effects
- [ ] SDL2_mixer integration
- [ ] Sound effect triggers based on game events
- [ ] Volume control and mixing

### 5.2 Voice Samples
- [ ] Extract Sinistar voice samples
- [ ] Implement voice trigger system
- [ ] Priority-based audio playback

### 5.3 Background Music
- [ ] Optional background music (if desired)
- [ ] Music/SFX volume controls

---

## Phase 6: UI & Game Flow (Week 10-11)

### 6.1 Menus
- [ ] Main menu
- [ ] Options/settings menu
- [ ] High score display
- [ ] Attract mode (demo gameplay)
- [ ] Pause menu

### 6.2 HUD
- [ ] Score display
- [ ] Lives counter
- [ ] Crystal/bomb counter
- [ ] Shield indicator
- [ ] Sinistar construction progress

### 6.3 Messages
- [ ] Game messages system
- [ ] Color and size support
- [ ] Message queuing and timing

---

## Phase 7: Game Progression & Scoring (Week 12)

### 7.1 Scoring System
- [ ] Point values for different actions
- [ ] Combo/multiplier system (if original had one)
- [ ] Extra life awards
- [ ] Score display and updates

### 7.2 High Score System
- [ ] Persistent high score storage (JSON file)
- [ ] Name entry system
- [ ] High score table display
- [ ] Platform-specific save locations

### 7.3 Level Progression
- [ ] Wave/level system
- [ ] Difficulty scaling
- [ ] Planetoid field generation
- [ ] Worker spawn rates
- [ ] Warrior spawn rates

---

## Phase 8: Input & Controls (Week 13)

### 8.1 Input Abstraction
- [ ] Keyboard controls
- [ ] Gamepad/controller support (SDL2 GameController)
- [ ] Mouse support (optional)
- [ ] Input rebinding system
- [ ] Multiple control schemes

### 8.2 Platform-Specific
- [ ] macOS keyboard mappings
- [ ] Windows keyboard mappings
- [ ] Controller button mappings
- [ ] Input configuration persistence

---

## Phase 9: Polish & Optimization (Week 14-15)

### 9.1 Performance
- [ ] Profile and optimize hot paths
- [ ] Memory usage optimization
- [ ] Target 60 FPS on both platforms
- [ ] Reduce CPU usage in idle states

### 9.2 Visual Polish
- [ ] Screen transitions
- [ ] Particle effects enhancement
- [ ] Optional HD graphics mode
- [ ] CRT shader effects (optional, for authenticity)
- [ ] Smooth animation interpolation

### 9.3 Audio Polish
- [ ] Audio ducking and mixing
- [ ] Sound effect polish
- [ ] Voice sample timing

---

## Phase 10: Platform-Specific Features (Week 16)

### 10.1 Windows
- [ ] Installer creation (NSIS or Inno Setup)
- [ ] Windows icon and resources
- [ ] DirectX vs OpenGL renderer selection
- [ ] Registry settings (optional)
- [ ] Windows code signing (for distribution)

### 10.2 macOS
- [ ] .app bundle creation
- [ ] macOS icon (ICNS)
- [ ] Info.plist configuration
- [ ] Gatekeeper compatibility
- [ ] macOS code signing and notarization
- [ ] DMG creation for distribution

### 10.3 Cross-Platform
- [ ] Settings sync format
- [ ] Asset packaging
- [ ] Consistent save locations

---

## Phase 11: Testing & Quality Assurance (Week 17-18)

### 11.1 Functional Testing
- [ ] All game mechanics verification
- [ ] AI behavior testing
- [ ] Collision detection accuracy
- [ ] Score system verification
- [ ] High score persistence

### 11.2 Platform Testing
- [ ] Windows 10/11 testing
- [ ] macOS (Intel and Apple Silicon)
- [ ] Different screen resolutions
- [ ] Controller compatibility
- [ ] Performance testing on various hardware

### 11.3 Bug Fixing
- [ ] Track and fix all discovered bugs
- [ ] Edge case handling
- [ ] Memory leak testing
- [ ] Crash recovery

---

## Phase 12: Documentation & Release (Week 19-20)

### 12.1 Documentation
- [ ] README with installation instructions
- [ ] Build documentation for developers
- [ ] User manual/how to play
- [ ] Control reference
- [ ] Troubleshooting guide

### 12.2 Licensing
- [ ] Verify rights to recreate Sinistar
- [ ] License file (consider GPL, MIT, or proprietary)
- [ ] Attribution for original creators
- [ ] Third-party library licenses

### 12.3 Distribution
- [ ] GitHub releases
- [ ] Itch.io (if distributing publicly)
- [ ] Steam (if commercial)
- [ ] Installer packages for both platforms

---

## Technical Challenges & Solutions

### Challenge 1: Replicating Arcade Timing
**Problem**: Original runs on 1 MHz 6809, needs consistent behavior
**Solution**: Implement fixed timestep game loop (60 FPS), decouple rendering

### Challenge 2: Warrior AI Translation
**Problem**: 84 files of complex assembly AI logic
**Solution**:
1. Create AI behavior tree or state machine framework
2. Translate each mission type systematically
3. Extract constants and tuning values
4. Extensive testing against original behavior

### Challenge 3: Pixel-Perfect Collision
**Problem**: PIXCHK.SRC has complex bit-level operations
**Solution**:
1. Create sprite collision masks
2. Implement efficient bitmask collision checks
3. Use spatial partitioning to reduce checks

### Challenge 4: Original Graphics Assets
**Problem**: Graphics are embedded in assembly/ROM
**Solution**:
1. Extract sprites from ROM dumps or MAME
2. Recreate sprites pixel-by-pixel if necessary
3. Consider using MAME artwork files
4. Option: Create HD reimagined graphics

### Challenge 5: Original Audio
**Problem**: Audio data in Williams audio hardware format
**Solution**:
1. Extract from ROM using MAME
2. Convert to WAV/OGG format
3. Record from actual arcade machine (if available)
4. Recreate synthetically if needed

---

## Asset Extraction Strategy

### From Original ROM/Hardware
1. Use **MAME** (Multiple Arcade Machine Emulator) tools
2. Extract from Sinistar ROM dumps (legally obtained)
3. Tools:
   - `romcmp` for ROM verification
   - `chdman` for CHD files
   - MAME debugger for sprite sheets
   - VGMTrans for audio

### Graphics
- Character ROM contains sprite data
- Extract to PNG sprite sheets
- Document sprite dimensions and animation frames
- Create sprite atlas for efficient rendering

### Audio
- Extract voice samples (WAV format)
- Extract sound effects
- Document trigger points in code

---

## Build System (CMake Example)

```cmake
cmake_minimum_required(VERSION 3.15)
project(SinistarModern VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)

# Platform-specific settings
if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE sinistar.icns)
    set(APP_ICON ${CMAKE_SOURCE_DIR}/assets/sinistar.icns)
    set_source_files_properties(${APP_ICON} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
endif()

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# Executable
if(APPLE)
    add_executable(Sinistar MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${APP_ICON})
elseif(WIN32)
    add_executable(Sinistar WIN32 ${SOURCES} ${HEADERS})
else()
    add_executable(Sinistar ${SOURCES} ${HEADERS})
endif()

# Link libraries
target_link_libraries(Sinistar
    SDL2::SDL2
    SDL2_mixer::SDL2_mixer
    SDL2_image::SDL2_image
)

# Include directories
target_include_directories(Sinistar PRIVATE src/)

# Copy assets to build directory
add_custom_command(TARGET Sinistar POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:Sinistar>/assets
)

# Installation
install(TARGETS Sinistar DESTINATION bin)
install(DIRECTORY assets DESTINATION share/sinistar)
```

---

## Risk Mitigation

### Legal Risks
- **Risk**: Copyright infringement
- **Mitigation**:
  - Consult with IP lawyers
  - Contact Warner Bros. (current rights holder)
  - Consider clean-room implementation
  - Non-commercial/educational use only

### Technical Risks
- **Risk**: Cannot replicate original behavior exactly
- **Mitigation**:
  - Extensive playtesting
  - Compare with MAME emulation
  - Adjust until "feels right"
  - Document intentional differences

### Scope Risks
- **Risk**: Project takes longer than estimated
- **Mitigation**:
  - Prioritize core gameplay first
  - Polish features can be added later
  - Release iteratively (MVP → Full Game)

---

## Success Criteria

### Must Have (MVP)
✅ Runs on Windows and macOS
✅ Player ship controls and movement
✅ Warrior enemies with basic AI
✅ Sinistar entity with construction and attack
✅ Collision detection working
✅ Scoring system
✅ Sound effects and voice samples
✅ Game win/loss conditions

### Should Have
✅ All original warrior AI missions
✅ High score persistence
✅ Controller support
✅ Options/settings menu
✅ Attract mode
✅ Screen scaling options

### Nice to Have
✅ HD graphics mode
✅ CRT shader effects
✅ Achievements
✅ Online leaderboards
✅ Level editor
✅ Replay system

---

## Alternative Approaches (For Consideration)

### Approach A: Emulation
- Use existing 6809 emulator core
- Emulate Williams arcade hardware
- **Pros**: Authentic behavior guaranteed
- **Cons**: Complex, platform-specific, performance overhead

### Approach B: MAME Integration
- Build on top of MAME codebase
- Create custom frontend
- **Pros**: Proven emulation accuracy
- **Cons**: GPL licensing, heavy dependency

### Approach C: Web Technologies
- JavaScript/TypeScript + HTML5 Canvas
- Electron for desktop packaging
- **Pros**: Cross-platform, easy distribution
- **Cons**: Performance concerns, larger file size

---

## Recommended Workflow

### Development Iteration
1. **Study Original**: Analyze assembly for specific feature
2. **Design Modern**: Design C++ equivalent architecture
3. **Implement**: Write code with unit tests
4. **Test**: Compare behavior with MAME
5. **Tune**: Adjust constants and timing
6. **Document**: Note differences and decisions

### Version Control Strategy
- `main` - Stable releases
- `develop` - Integration branch
- `feature/*` - Individual features
- `platform/windows` - Windows-specific work
- `platform/macos` - macOS-specific work

### Testing Strategy
- Unit tests for math/physics functions
- Integration tests for game systems
- Manual playtesting for feel/balance
- Platform-specific testing on real hardware

---

## Next Steps

1. **Decision**: Choose technology stack (recommend SDL2 + C++)
2. **Setup**: Initialize repository and build system
3. **Prototype**: Create minimal game loop with one entity
4. **Asset Extraction**: Extract graphics and audio from ROM
5. **Core Systems**: Implement task manager and physics
6. **Iterate**: Build entity by entity, system by system

---

## Resources & References

### Original Code Analysis
- This repository's assembly source code
- Comments and equate files for constants

### Emulation Reference
- MAME Sinistar driver: `mame/src/mame/drivers/williams.cpp`
- MAME Sinistar ROM set verification

### Technical Documentation
- Motorola 6809 instruction set reference
- Williams arcade hardware documentation
- DMA system documentation

### Development Tools
- SDL2 documentation: https://wiki.libsdl.org/
- CMake documentation: https://cmake.org/documentation/
- MAME tools: https://www.mamedev.org/

### Community
- MAME forums for technical questions
- Arcade preservation communities
- Retro gaming development Discord servers

---

## Conclusion

This plan provides a comprehensive roadmap for creating a modern, cross-platform version of Sinistar. The recommended approach of recreating the game logic in C++ with SDL2 balances authenticity with maintainability and performance.

**Estimated Timeline**: 16-20 weeks for full implementation
**Recommended Team Size**: 2-3 developers
**Primary Skills Needed**: C++, SDL2, game development, assembly reading (for translation)

The phased approach allows for iterative development with playable builds at each major milestone. Priority should be given to core gameplay mechanics before polish and platform-specific features.

---

**Document Version**: 1.0
**Date**: 2026-01-22
**Author**: Cross-Platform Modernization Planning
**Status**: Ready for Review and Implementation
