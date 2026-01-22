/**
 * Color.cpp
 *
 * Implementation of color system
 */

#include "Color.h"

// Color static constants
const Color Color::BLACK = Color(0, 0, 0);
const Color Color::WHITE = Color(255, 255, 255);
const Color Color::RED = Color(255, 0, 0);
const Color Color::GREEN = Color(0, 255, 0);
const Color Color::BLUE = Color(0, 0, 255);
const Color Color::YELLOW = Color(255, 255, 0);
const Color Color::CYAN = Color(0, 255, 255);
const Color Color::MAGENTA = Color(255, 0, 255);
const Color Color::ORANGE = Color(255, 128, 0);
const Color Color::PURPLE = Color(128, 0, 255);
const Color Color::GRAY = Color(128, 128, 128);
const Color Color::DARK_GRAY = Color(64, 64, 64);
const Color Color::LIGHT_GRAY = Color(192, 192, 192);

// ColorPalette implementation

ColorPalette& ColorPalette::getInstance() {
    static ColorPalette instance;
    return instance;
}

ColorPalette::ColorPalette() {
    initializeDefaultPalette();
}

Color ColorPalette::getColor(int index) const {
    if (index >= 0 && index < PALETTE_SIZE) {
        return palette_[index];
    }
    return Color::WHITE;
}

void ColorPalette::setColor(int index, const Color& color) {
    if (index >= 0 && index < PALETTE_SIZE) {
        palette_[index] = color;
    }
}

void ColorPalette::initializeDefaultPalette() {
    // Arcade-inspired 16-color palette
    // Based on typical Williams arcade game colors
    palette_[0] = Color::BLACK;          // Background
    palette_[1] = Color::WHITE;          // Text, UI
    palette_[2] = Color::RED;            // Enemies, danger
    palette_[3] = Color::GREEN;          // Pickups, good
    palette_[4] = Color::BLUE;           // Player, friendly
    palette_[5] = Color::YELLOW;         // Highlights, warnings
    palette_[6] = Color::CYAN;           // Effects
    palette_[7] = Color::MAGENTA;        // Special
    palette_[8] = Color::ORANGE;         // Explosions
    palette_[9] = Color::PURPLE;         // Sinistar
    palette_[10] = Color::GRAY;          // Debris
    palette_[11] = Color::DARK_GRAY;     // Shadows
    palette_[12] = Color::LIGHT_GRAY;    // Highlights
    palette_[13] = Color(255, 192, 128); // Light orange
    palette_[14] = Color(128, 255, 128); // Light green
    palette_[15] = Color(128, 128, 255); // Light blue
}
