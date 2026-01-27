# Sinistar Sprite Assets

This directory contains sprite images for the game. Sprites are loaded by the SpriteManager and rendered using SDL2_image.

## Required Sprites

### Sinistar Boss Face (HIGHEST PRIORITY)
**Filename:** `sinistar_face.png`
**Dimensions:** 384x64 pixels (6 frames @ 64x64 each, horizontal strip)
**Format:** PNG with alpha transparency

**Frame Layout:**
```
[Frame 0][Frame 1][Frame 2][Frame 3][Frame 4][Frame 5]
  Closed   Open1    Open2    Open3   EyeGlow1 EyeGlow2
```

**Design Specifications:**
- Circular/oval demon/dragon face
- Cream/white base color with purple/blue horns
- Dark red eyes with bright red accents
- Jaw animation: closed → fully open (showing teeth and red interior)
- Size: ~64x64 pixels to match arcade scale (~30px radius in-game)

**Reference:** See the arcade screenshots provided - detailed menacing face with jaw animation

### Player Ship (Next Priority)
**Filename:** `player_ship.png`
**Dimensions:** 48x16 pixels (3 frames @ 16x16 each)
**Frames:** 0=No thrust, 1=Thrust frame 1, 2=Thrust frame 2
**Colors:** Cyan/blue with bright engine thrust

### Enemy Warriors
**Filename:** `warrior.png`
**Dimensions:** 36x12 pixels (3 rotation frames @ 12x12 each)
**Colors:** Red/orange angular fighter

### Worker Ships
**Filename:** `worker.png`
**Dimensions:** 28x14 pixels (2 frames @ 14x14 each)
**Frames:** 0=Empty, 1=Carrying crystal (green glow)
**Colors:** Blue/red hexagonal cargo ship

### Crystal Planetoids
**Filenames:**
- `crystal_small.png` (96x16 - 6 frames @ 16x16)
- `crystal_medium.png` (144x24 - 6 frames @ 24x24)
- `crystal_large.png` (192x32 - 6 frames @ 32x32)

**Frames:** 6 rotation frames for spinning animation
**Colors:** Green/cyan with yellow/white highlights

## Creating Sprites

### Recommended Tools:
- **Aseprite** - Professional pixel art editor (paid)
- **GIMP** - Free image editor with pixel art support
- **Piskel** - Free browser-based pixel art tool

### Format Guidelines:
- Use PNG format with alpha transparency
- Power-of-2 friendly dimensions when possible
- Horizontal sprite sheets (frames side-by-side)
- Index colors or 32-bit RGBA
- No compression for fast loading

### Color Palette (Arcade-Inspired):
- Background: Black (0, 0, 0)
- Player: Cyan/Blue (0, 255, 255) / (0, 150, 255)
- Enemies: Red (255, 0, 0) / Orange (255, 100, 0)
- Crystals: Green (0, 255, 0) / Cyan (0, 255, 200)
- Sinistar: Cream/White (255, 240, 200), Red eyes (255, 0, 0)
- Explosions: Orange (255, 150, 0), Yellow (255, 255, 0)

## Placeholder Behavior

If a sprite file is not found, the game will:
1. Fall back to geometric rendering (current implementation)
2. Log a warning message
3. Continue running normally

This allows incremental sprite development without breaking the game.

## Testing Sprites

After adding sprite files:
1. Rebuild the game: `cd build && make`
2. Sprites are automatically copied to `build/bin/assets/sprites/`
3. Run the game and check console output for sprite loading messages
4. The SpriteManager logs: "Loaded sprite 'filename' (WxH)"

## Asset Creation Status

- [ ] sinistar_face.png - **HIGH PRIORITY** - Most iconic visual!
- [ ] player_ship.png
- [ ] warrior.png
- [ ] worker.png
- [ ] crystal_small.png
- [ ] crystal_medium.png
- [ ] crystal_large.png

**Start with sinistar_face.png for maximum visual impact!**
