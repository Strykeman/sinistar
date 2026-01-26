/**
 * Color.h
 *
 * Color definitions and palette management
 * Based on original arcade color palette
 */

#ifndef COLOR_H
#define COLOR_H

#include <SDL.h>
#include <cstdint>

/**
 * Color - RGBA color representation
 */
struct Color {
    uint8_t r, g, b, a;

    Color() : r(0), g(0), b(0), a(255) {}
    Color(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255)
        : r(r_), g(g_), b(b_), a(a_) {}

    // Create from hex (0xRRGGBB)
    static Color fromHex(uint32_t hex, uint8_t alpha = 255) {
        return Color(
            (hex >> 16) & 0xFF,
            (hex >> 8) & 0xFF,
            hex & 0xFF,
            alpha
        );
    }

    // Convert to SDL_Color
    SDL_Color toSDL() const {
        return SDL_Color{r, g, b, a};
    }

    // Operators
    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    bool operator!=(const Color& other) const {
        return !(*this == other);
    }

    // Predefined colors (arcade palette inspired)
    static const Color BLACK;
    static const Color WHITE;
    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color YELLOW;
    static const Color CYAN;
    static const Color MAGENTA;
    static const Color ORANGE;
    static const Color PURPLE;
    static const Color GRAY;
    static const Color DARK_GRAY;
    static const Color LIGHT_GRAY;
};

/**
 * ColorPalette - manages game color palette
 * Replicates the color RAM system from original arcade
 */
class ColorPalette {
public:
    /**
     * Get singleton instance
     */
    static ColorPalette& getInstance();

    /**
     * Get color by palette index
     * @param index Palette index (0-15 for 16-color palette)
     * @return Color at that index
     */
    Color getColor(int index) const;

    /**
     * Set color at palette index
     * @param index Palette index
     * @param color Color to set
     */
    void setColor(int index, const Color& color);

    /**
     * Initialize with default arcade-style palette
     */
    void initializeDefaultPalette();

private:
    ColorPalette();
    static const int PALETTE_SIZE = 16;
    Color palette_[PALETTE_SIZE];
};

#endif // COLOR_H
