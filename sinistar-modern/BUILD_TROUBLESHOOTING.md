# Build Troubleshooting Guide

## Verified Working Build (Linux)

**Status**: ✅ **100% SUCCESS**
**Binary**: 490KB
**Date**: 2026-01-23
**Compiler**: GCC 13.3.0
**SDL2**: 2.30.0

All 18 source files compiled successfully with only cosmetic warnings.

---

## macOS Build Instructions

### Prerequisites

```bash
# Install dependencies via Homebrew
brew install sdl2 sdl2_mixer sdl2_image cmake

# Verify installations
brew list | grep sdl2
which cmake
```

### Build Steps

```bash
# 1. Navigate to project
cd ~/sinistar/sinistar-modern

# 2. Pull latest code
git pull origin claude/sinistar-cross-platform-plan-0aPGq

# 3. Clean any old build files
rm -rf build

# 4. Create fresh build directory
mkdir build
cd build

# 5. Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 6. Build (use all CPU cores)
make -j$(sysctl -n hw.ncpu)

# 7. Run!
./bin/Sinistar
```

---

## Common Build Errors & Solutions

### Error: "SDL2 not found"

**Solution**:
```bash
brew install sdl2 sdl2_mixer sdl2_image
```

If Homebrew installed but CMake can't find it:
```bash
export CMAKE_PREFIX_PATH=/opt/homebrew
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Error: "command not found: cmake"

**Solution**:
```bash
brew install cmake
```

### Error: Compilation fails with "unknown type name"

**Solution**: Make sure you have a recent C++17 compatible compiler:
```bash
# Check your compiler version
clang++ --version

# Should be at least:
# - Clang 5.0+ (macOS typically has recent version)
# - GCC 7.0+
```

### Error: "No such file or directory: ./bin/Sinistar"

**Cause**: Build failed before creating binary.

**Solution**:
1. Check the error messages above the "No such file" message
2. Most likely missing SDL2 dependencies (see above)
3. Try verbose build to see full errors:
   ```bash
   make VERBOSE=1
   ```

### Error: Build succeeds but won't run

**Solution**: Check SDL2 libraries are linked:
```bash
otool -L bin/Sinistar

# Should show SDL2 libraries like:
# @rpath/SDL2.framework/Versions/A/SDL2
# /opt/homebrew/lib/libSDL2_mixer.dylib
```

---

## Verification Checklist

After building, verify:

```bash
# 1. Binary exists
ls -lh bin/Sinistar

# 2. Binary is executable
file bin/Sinistar

# 3. SDL2 dependencies available
brew list sdl2

# 4. No obvious errors in build log
make 2>&1 | grep -i error

# 5. Run the game (should open window)
./bin/Sinistar
```

---

## Expected Warnings (Safe to Ignore)

These warnings are cosmetic and don't affect functionality:

1. **Virtual function hiding**: `'virtual void GameObject::render()' was hidden`
   - Not an error, just a C++ design pattern notice

2. **Initialization order**: `'GameEngine::logicalHeight_' will be initialized after`
   - Members initialize in declaration order, no runtime impact

3. **ALSA warnings** (Linux only): Audio device messages
   - Game works fine, just no sound in headless environment

---

## What Success Looks Like

**Console output should include**:
```
[100%] Built target Sinistar
Preparing assets directory
```

**Binary should be**:
- Size: ~350-500KB
- Type: Executable (Mach-O on Mac, ELF on Linux)
- Permissions: `-rwxr-xr-x`

**When run**:
- Window opens (768x732)
- Main menu appears
- No crashes or error popups

---

## Still Having Issues?

### Get detailed error info:

```bash
# Clean build with full output
rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug 2>&1 | tee cmake.log
make VERBOSE=1 2>&1 | tee build.log

# Check the log files for specific errors
cat cmake.log build.log
```

### System info:

```bash
# Compiler version
clang++ --version

# CMake version (need 3.15+)
cmake --version

# SDL2 version
brew info sdl2

# Architecture
uname -m  # Should be arm64 (Apple Silicon) or x86_64 (Intel)
```

---

## Quick Test Command

Copy-paste this single command to try everything:

```bash
cd ~/sinistar/sinistar-modern && \
git pull origin claude/sinistar-cross-platform-plan-0aPGq && \
rm -rf build && mkdir build && cd build && \
cmake .. -DCMAKE_BUILD_TYPE=Release && \
make -j$(sysctl -n hw.ncpu) && \
echo "Build successful! Run with: ./bin/Sinistar"
```

If this fails, copy the full error output for diagnosis.
