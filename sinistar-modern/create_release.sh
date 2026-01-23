#!/bin/bash
# Sinistar Modern v1.0.0 - Release Package Creator
# Creates distribution packages for Linux, macOS, and Windows

set -e

VERSION="1.0.0"
RELEASE_DIR="releases"
PROJECT_NAME="sinistar-modern"

echo "==================================="
echo "  Sinistar Modern Release Builder"
echo "  Version: $VERSION"
echo "==================================="
echo

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Function to print status
status() {
    echo -e "${GREEN}[✓]${NC} $1"
}

warning() {
    echo -e "${YELLOW}[!]${NC} $1"
}

error() {
    echo -e "${RED}[✗]${NC} $1"
}

# Create release directory
mkdir -p "$RELEASE_DIR"

# Detect current platform
PLATFORM=$(uname -s)
case "$PLATFORM" in
    Linux*)     PLATFORM_NAME="linux";;
    Darwin*)    PLATFORM_NAME="macos";;
    MINGW*)     PLATFORM_NAME="windows";;
    *)          PLATFORM_NAME="unknown";;
esac

status "Building for platform: $PLATFORM_NAME"

# Clean and build
echo
echo "Building project..."
cd build
make clean > /dev/null 2>&1 || true
make -j$(nproc) > build.log 2>&1

if [ $? -eq 0 ]; then
    status "Build completed successfully"
else
    error "Build failed! Check build/build.log for details"
    exit 1
fi

# Get binary size
BINARY_SIZE=$(ls -lh bin/Sinistar | awk '{print $5}')
status "Binary size: $BINARY_SIZE"

cd ..

# Create release package
PACKAGE_NAME="${PROJECT_NAME}-v${VERSION}-${PLATFORM_NAME}"
PACKAGE_DIR="$RELEASE_DIR/$PACKAGE_NAME"

echo
echo "Creating release package: $PACKAGE_NAME"

# Clean old package if exists
rm -rf "$PACKAGE_DIR"
mkdir -p "$PACKAGE_DIR"

# Copy binary
status "Copying binary..."
cp build/bin/Sinistar "$PACKAGE_DIR/"

# Copy assets (if any exist)
if [ -d "build/bin/assets" ] && [ "$(ls -A build/bin/assets)" ]; then
    status "Copying assets..."
    cp -r build/bin/assets "$PACKAGE_DIR/"
fi

# Copy documentation
status "Copying documentation..."
cp README.md "$PACKAGE_DIR/"
cp PHASE8_COMPLETE.md "$PACKAGE_DIR/" 2>/dev/null || true
cp PHASE9_COMPLETE.md "$PACKAGE_DIR/" 2>/dev/null || true
cp PHASE10_COMPLETE.md "$PACKAGE_DIR/" 2>/dev/null || true
cp RELEASE_BUILD_REPORT.md "$PACKAGE_DIR/" 2>/dev/null || true

# Create run script for Linux/macOS
if [ "$PLATFORM_NAME" != "windows" ]; then
    cat > "$PACKAGE_DIR/run.sh" << 'EOF'
#!/bin/bash
# Sinistar Modern Launcher

cd "$(dirname "$0")"

# Check for SDL2
if ! command -v sdl2-config &> /dev/null; then
    echo "SDL2 not found! Please install SDL2:"
    echo "  Ubuntu/Debian: sudo apt-get install libsdl2-2.0-0 libsdl2-mixer-2.0-0"
    echo "  Fedora: sudo dnf install SDL2 SDL2_mixer"
    echo "  macOS: brew install sdl2 sdl2_mixer"
    echo
    read -p "Press Enter to exit..."
    exit 1
fi

# Run the game
./Sinistar

EOF
    chmod +x "$PACKAGE_DIR/run.sh"
    status "Created run script"
fi

# Create installation instructions
cat > "$PACKAGE_DIR/INSTALL.txt" << EOF
===========================================
  SINISTAR MODERN v$VERSION
  Installation Instructions
===========================================

SYSTEM REQUIREMENTS:
- OS: Linux, macOS 10.14+, or Windows 10+
- RAM: 128 MB minimum
- Display: 800x600 minimum resolution
- Libraries: SDL2, SDL2_mixer

-------------------------------------------
LINUX INSTALLATION:
-------------------------------------------

1. Install dependencies:
   Ubuntu/Debian:
     sudo apt-get install libsdl2-2.0-0 libsdl2-mixer-2.0-0

   Fedora:
     sudo dnf install SDL2 SDL2_mixer

   Arch:
     sudo pacman -S sdl2 sdl2_mixer

2. Make the binary executable:
   chmod +x Sinistar

3. Run the game:
   ./run.sh

   OR directly:
   ./Sinistar

-------------------------------------------
macOS INSTALLATION:
-------------------------------------------

1. Install Homebrew (if not installed):
   /bin/bash -c "\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

2. Install dependencies:
   brew install sdl2 sdl2_mixer

3. Make the binary executable:
   chmod +x Sinistar

4. Run the game:
   ./run.sh

   OR directly:
   ./Sinistar

-------------------------------------------
WINDOWS INSTALLATION:
-------------------------------------------

1. Ensure SDL2.dll and SDL2_mixer.dll are in the same directory as Sinistar.exe
   (These should be included in the package)

2. Double-click Sinistar.exe to run

-------------------------------------------
CONTROLS:
-------------------------------------------

Keyboard:
  Arrow Keys / WASD - Thrust in direction
  Space - Fire Sinibomb
  B - Drop Sinibomb (proximity bomb)
  P - Pause game
  ESC - Quit to menu

Gamepad:
  Left Stick / D-Pad - Thrust
  A Button (South) - Fire Sinibomb
  B Button (East) - Drop Sinibomb
  Start - Pause
  Back - Quit to menu

-------------------------------------------
TROUBLESHOOTING:
-------------------------------------------

Linux:
- If you see "error while loading shared libraries":
  Make sure SDL2 packages are installed (see step 1)

- If you see "Permission denied":
  Run: chmod +x Sinistar

macOS:
- If you see "cannot be opened because the developer cannot be verified":
  Right-click -> Open, then click "Open" in the dialog

- For Apple Silicon Macs, you may need Rosetta 2:
  softwareupdate --install-rosetta

Windows:
- If DLL errors appear, ensure SDL2.dll and SDL2_mixer.dll are present

- If the game window doesn't appear, try running as administrator

-------------------------------------------
DOCUMENTATION:
-------------------------------------------

README.md - Complete game manual
PHASE8_COMPLETE.md - Crystal mining system details
PHASE9_COMPLETE.md - Sinistar boss entity details
PHASE10_COMPLETE.md - Final polish and features
RELEASE_BUILD_REPORT.md - Build verification report

-------------------------------------------
SUPPORT:
-------------------------------------------

For issues, questions, or feedback:
https://github.com/Strykeman/sinistar

-------------------------------------------

"BEWARE, I LIVE!"

Enjoy the game!
EOF

    status "Created installation instructions"

# Create version file
cat > "$PACKAGE_DIR/VERSION.txt" << EOF
Sinistar Modern v$VERSION
Build Date: $(date +"%Y-%m-%d %H:%M:%S")
Platform: $PLATFORM_NAME
Binary Size: $BINARY_SIZE
Git Commit: $(git rev-parse --short HEAD 2>/dev/null || echo "unknown")

Status: RELEASE
Quality: Production-ready
EOF

    status "Created version file"

# Create archive
echo
echo "Creating archive..."
cd "$RELEASE_DIR"

if command -v tar &> /dev/null; then
    tar -czf "${PACKAGE_NAME}.tar.gz" "$PACKAGE_NAME"
    ARCHIVE_SIZE=$(ls -lh "${PACKAGE_NAME}.tar.gz" | awk '{print $5}')
    status "Created: ${PACKAGE_NAME}.tar.gz ($ARCHIVE_SIZE)"
fi

if command -v zip &> /dev/null; then
    zip -r -q "${PACKAGE_NAME}.zip" "$PACKAGE_NAME"
    ARCHIVE_SIZE=$(ls -lh "${PACKAGE_NAME}.zip" | awk '{print $5}')
    status "Created: ${PACKAGE_NAME}.zip ($ARCHIVE_SIZE)"
fi

cd ..

echo
echo "==================================="
echo -e "${GREEN}Release package created successfully!${NC}"
echo "==================================="
echo
echo "Package location: $RELEASE_DIR/$PACKAGE_NAME/"
echo "Archives created in: $RELEASE_DIR/"
echo
echo "Package contents:"
ls -lh "$PACKAGE_DIR" | tail -n +2 | awk '{printf "  - %-20s %s\n", $9, $5}'
echo
echo "Ready for distribution!"
echo
echo "\"BEWARE, I LIVE!\""
