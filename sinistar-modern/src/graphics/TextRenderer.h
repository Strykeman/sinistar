/**
 * TextRenderer.h
 *
 * Text rendering system using bitmap fonts
 * Replicates PHRASE.SRC message display system from original
 */

#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <SDL.h>
#include <string>
#include <map>
#include <vector>
#include "Color.h"
#include "../core/Math.h"

/**
 * BitmapFont - represents a bitmap font (3x5, 6x8, etc.)
 * Original game used character fonts stored in ROM
 */
class BitmapFont {
public:
    /**
     * Font sizes matching original arcade fonts
     */
    enum class Size {
        SMALL_3x5,   // 3x5 pixel font (original small font)
        LARGE_6x8    // 6x8 pixel font (original large font)
    };

    /**
     * Constructor
     * @param renderer SDL renderer
     * @param size Font size
     */
    BitmapFont(SDL_Renderer* renderer, Size size);

    /**
     * Destructor
     */
    ~BitmapFont();

    /**
     * Get character width
     */
    int getCharWidth() const { return charWidth_; }

    /**
     * Get character height
     */
    int getCharHeight() const { return charHeight_; }

    /**
     * Get font size
     */
    Size getSize() const { return size_; }

    /**
     * Draw a single character
     * @param c Character to draw
     * @param x X position
     * @param y Y position
     * @param color Color to draw with
     */
    void drawChar(char c, int x, int y, const Color& color);

private:
    SDL_Renderer* renderer_;
    Size size_;
    int charWidth_;
    int charHeight_;

    /**
     * Get bitmap data for a character
     * Returns pointer to bitmap data (simple placeholder for now)
     * TODO: Load actual font data from ROM extraction
     */
    const uint8_t* getCharBitmap(char c) const;

    /**
     * Draw a pixel
     */
    void drawPixel(int x, int y, const Color& color);
};

/**
 * TextAlignment - text alignment options
 */
enum class TextAlignment {
    LEFT,
    CENTER,
    RIGHT
};

/**
 * Message - represents a text message with properties
 * Replicates message blocks from PHRASE.SRC
 */
struct Message {
    std::string text;
    Vector2 position;
    Color color;
    BitmapFont::Size fontSize;
    TextAlignment alignment;
    float duration;        // How long to display (0 = permanent)
    float timeRemaining;   // Time left to display

    Message()
        : position(0, 0)
        , color(Color::WHITE)
        , fontSize(BitmapFont::Size::LARGE_6x8)
        , alignment(TextAlignment::LEFT)
        , duration(0.0f)
        , timeRemaining(0.0f)
    {}
};

/**
 * TextRenderer - manages text rendering and messages
 * Replicates PHRASE message system from original
 */
class TextRenderer {
public:
    /**
     * Constructor
     * @param renderer SDL renderer
     */
    explicit TextRenderer(SDL_Renderer* renderer);

    /**
     * Destructor
     */
    ~TextRenderer();

    /**
     * Draw text at position
     * @param text Text to draw
     * @param x X position
     * @param y Y position
     * @param size Font size
     * @param color Text color
     * @param alignment Text alignment
     */
    void drawText(const std::string& text, int x, int y,
                  BitmapFont::Size size = BitmapFont::Size::LARGE_6x8,
                  const Color& color = Color::WHITE,
                  TextAlignment alignment = TextAlignment::LEFT);

    /**
     * Draw text at position (Vector2 version)
     */
    void drawText(const std::string& text, const Vector2& pos,
                  BitmapFont::Size size = BitmapFont::Size::LARGE_6x8,
                  const Color& color = Color::WHITE,
                  TextAlignment alignment = TextAlignment::LEFT);

    /**
     * Add a timed message
     * @param message Message to add
     */
    void addMessage(const Message& message);

    /**
     * Add a simple timed message
     * @param text Message text
     * @param pos Position
     * @param duration How long to display
     * @param color Text color
     * @param size Font size
     */
    void showMessage(const std::string& text, const Vector2& pos,
                    float duration, const Color& color = Color::WHITE,
                    BitmapFont::Size size = BitmapFont::Size::LARGE_6x8);

    /**
     * Update timed messages
     * @param deltaTime Time since last update
     */
    void update(float deltaTime);

    /**
     * Render all active messages
     */
    void render();

    /**
     * Clear all messages
     */
    void clearMessages();

    /**
     * Measure text width
     * @param text Text to measure
     * @param size Font size
     * @return Width in pixels
     */
    int measureText(const std::string& text, BitmapFont::Size size) const;

private:
    SDL_Renderer* renderer_;
    std::map<BitmapFont::Size, BitmapFont*> fonts_;
    std::vector<Message> activeMessages_;

    /**
     * Get font for size (creates if needed)
     */
    BitmapFont* getFont(BitmapFont::Size size);

    /**
     * Calculate aligned X position
     */
    int getAlignedX(const std::string& text, int x, BitmapFont::Size size,
                    TextAlignment alignment) const;
};

#endif // TEXTRENDERER_H
