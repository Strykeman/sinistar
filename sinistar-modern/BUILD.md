# Sinistar Modern - Build Instructions

This document provides detailed instructions for building Sinistar Modern on macOS and Windows.

## Prerequisites

### Both Platforms
- **CMake** 3.15 or later
- **C++17 compatible compiler**
- **SDL2** 2.0.x or later

### macOS
- **Xcode Command Line Tools** or **Xcode**
- **Homebrew** (recommended for dependencies)

### Windows
- **Visual Studio 2019 or later** (Community Edition is free)
  - OR **MinGW-w64** with GCC 7.0+
- **vcpkg** (recommended for SDL2 installation)

---

## Installing Dependencies

### macOS (via Homebrew)

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake sdl2 sdl2_mixer sdl2_image
```

### Windows (via vcpkg)

```powershell
# Clone vcpkg (if not already installed)
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install SDL2 packages
.\vcpkg install sdl2:x64-windows
.\vcpkg install sdl2-mixer:x64-windows
.\vcpkg install sdl2-image:x64-windows

# Integrate vcpkg with Visual Studio
.\vcpkg integrate install
```

### Windows (Manual SDL2 Installation)

Alternatively, you can download SDL2 manually:

1. Download SDL2 development libraries from:
   - https://www.libsdl.org/download-2.0.php
   - SDL2-devel-2.x.x-VC.zip (Visual Studio)
   - SDL2-devel-2.x.x-mingw.tar.gz (MinGW)

2. Extract to a known location (e.g., `C:\SDL2`)

3. Set environment variable (optional):
   ```powershell
   setx SDL2_DIR "C:\SDL2"
   ```

---

## Building the Project

### macOS

#### Using Command Line

```bash
# Navigate to the project directory
cd sinistar-modern

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# Run the game
./bin/Sinistar
```

#### Using Xcode

```bash
# Generate Xcode project
mkdir build-xcode
cd build-xcode
cmake -G Xcode ..

# Open in Xcode
open Sinistar.xcodeproj

# Build and run from Xcode (Product > Run or Cmd+R)
```

### Windows

#### Using Visual Studio

```powershell
# Navigate to the project directory
cd sinistar-modern

# Create build directory
mkdir build
cd build

# Configure with CMake (if using vcpkg)
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"

# OR without vcpkg (if SDL2_DIR is set)
cmake ..

# Build
cmake --build . --config Release

# Run the game
.\bin\Release\Sinistar.exe
```

#### Using Visual Studio GUI

1. Open Visual Studio 2019/2022
2. Select "Open a local folder"
3. Navigate to `sinistar-modern` directory
4. Visual Studio will auto-detect CMakeLists.txt
5. Select build configuration (Debug/Release)
6. Build > Build All (Ctrl+Shift+B)
7. Run (F5)

#### Using MinGW

```bash
# Create build directory
mkdir build
cd build

# Configure with MinGW Makefiles
cmake -G "MinGW Makefiles" ..

# Build
mingw32-make

# Run
./bin/Sinistar.exe
```

---

## Build Configurations

### Debug Build (Default)

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

Debug builds include:
- Debug symbols
- No optimization
- Assertions enabled
- Verbose logging

### Release Build

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

Release builds include:
- Full optimization
- No debug symbols
- Smaller binary size
- Maximum performance

---

## CMake Options

You can customize the build with these options:

```bash
# Disable tests
cmake -DBUILD_TESTS=OFF ..

# Specify SDL2 location manually (if not found automatically)
cmake -DSDL2_DIR=/path/to/sdl2 ..

# Generate compile_commands.json (for IDE support)
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
```

---

## Troubleshooting

### macOS: "SDL2 not found"

```bash
# Ensure Homebrew SDL2 is in PATH
export CMAKE_PREFIX_PATH="/usr/local:$CMAKE_PREFIX_PATH"

# Or specify SDL2 location explicitly
cmake -DSDL2_DIR=/usr/local/lib/cmake/SDL2 ..
```

### macOS: Apple Silicon (M1/M2/M3)

The project builds natively on Apple Silicon:

```bash
# Build for native architecture (arm64)
cmake ..
cmake --build .

# Or build universal binary (arm64 + x86_64)
cmake -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" ..
cmake --build .
```

### Windows: "Cannot find SDL2"

If using manual installation:

```powershell
# Set SDL2_DIR environment variable
$env:SDL2_DIR="C:\SDL2\cmake"

# Or specify in cmake command
cmake -DSDL2_DIR="C:\SDL2\cmake" ..
```

### Windows: Missing DLLs at Runtime

If you get "SDL2.dll not found" when running:

1. Copy SDL2.dll to the executable directory:
   ```
   From: C:\vcpkg\installed\x64-windows\bin\SDL2.dll
   To: sinistar-modern\build\bin\Release\
   ```

2. Or add SDL2 bin directory to PATH:
   ```powershell
   $env:PATH += ";C:\vcpkg\installed\x64-windows\bin"
   ```

### Linux (Bonus - Not Officially Supported Yet)

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install cmake libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev

# Build
mkdir build && cd build
cmake ..
make

# Run
./bin/Sinistar
```

---

## Verifying the Build

After building successfully, you should see:

1. **Window Opens**: A resizable window titled "Sinistar"
2. **Display**: Black screen with white border and red test rectangle
3. **Console Output**:
   ```
   Sinistar Modern v1.0.0
   SDL 2.x.x initialized
   Window created: 768x732
   Logical resolution: 256x244
   Game engine initialized
   TaskManager initialized
   RenderSystem initialized
   Starting game loop (60 FPS target)...
   ```

4. **Controls**:
   - **ESC**: Quit the application
   - **Window resize**: Should maintain aspect ratio

If you see this, congratulations! The foundation is working correctly.

---

## Next Steps

Now that you have a working build:

1. **Run the test build** to verify everything works
2. **Read** `CROSS_PLATFORM_PLAN.md` for the development roadmap
3. **Start implementing** game entities and systems
4. **Extract assets** from the original ROM (see Asset Extraction section)

---

## IDE Setup

### Visual Studio Code

1. Install extensions:
   - C/C++ (Microsoft)
   - CMake Tools (Microsoft)

2. Open `sinistar-modern` folder

3. Configure CMake:
   - Ctrl+Shift+P → "CMake: Configure"
   - Select compiler/toolchain

4. Build:
   - Ctrl+Shift+P → "CMake: Build" or F7

5. Debug:
   - F5 (creates launch.json automatically)

### CLion

1. Open `sinistar-modern` directory
2. CLion auto-detects CMakeLists.txt
3. Build > Build Project (Ctrl+F9)
4. Run > Run 'Sinistar' (Shift+F10)

---

## Clean Build

To start fresh:

```bash
# Remove build directory
rm -rf build

# Recreate and rebuild
mkdir build
cd build
cmake ..
cmake --build .
```

Or use CMake's clean:

```bash
cd build
cmake --build . --target clean
```

---

## Creating Distribution Builds

### macOS (.app bundle)

```bash
# Build release with app bundle
mkdir build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Result: build-release/bin/Sinistar.app
# Double-click to run or:
open ./bin/Sinistar.app
```

### Windows (Installer)

After building release version, you can use:
- NSIS (Nullsoft Scriptable Install System)
- Inno Setup
- WiX Toolset

(Instructions TBD - will be added in Phase 10)

---

## Performance Tips

1. **Always use Release builds** for performance testing
2. **VSync is enabled** by default (60 FPS cap)
3. **Profile before optimizing** - use profiling tools to find bottlenecks

---

## Getting Help

If you encounter build issues:

1. Check this document's **Troubleshooting** section
2. Verify all **dependencies are installed** correctly
3. Check **CMake version**: `cmake --version` (need 3.15+)
4. Check **compiler version**:
   - macOS: `clang --version`
   - Windows: Check Visual Studio version
5. Try a **clean build** (delete build directory)

---

**Document Version**: 1.0
**Last Updated**: 2026-01-22
**Supports**: macOS (Intel/Apple Silicon), Windows 10/11
