/**
 * TextRenderer.cpp
 *
 * Implementation of text rendering system
 */

#include "TextRenderer.h"
#include <algorithm>
#include <iostream>

// BitmapFont implementation

BitmapFont::BitmapFont(SDL_Renderer* renderer, Size size)
    : renderer_(renderer)
    , size_(size)
{
    // Set character dimensions based on size
    switch (size) {
        case Size::SMALL_3x5:
            charWidth_ = 3;
            charHeight_ = 5;
            break;
        case Size::LARGE_6x8:
            charWidth_ = 6;
            charHeight_ = 8;
            break;
    }
}

BitmapFont::~BitmapFont() {
}

void BitmapFont::drawChar(char c, int x, int y, const Color& color) {
    // Set draw color
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);

    // For now, draw a simple placeholder rectangle for each character
    // TODO: Replace with actual bitmap font data from ROM extraction

    // Draw a filled rectangle as placeholder
    SDL_Rect charRect = {x, y, charWidth_, charHeight_};
    SDL_RenderDrawRect(renderer_, &charRect);

    // Draw a simple pattern based on character
    // This is just a placeholder until we have real font data
    int pattern = (c % 8);
    for (int py = 0; py < charHeight_; py++) {
        for (int px = 0; px < charWidth_; px++) {
            // Simple pattern based on character value
            if ((px + py + pattern) % 3 == 0) {
                drawPixel(x + px, y + py, color);
            }
        }
    }
}

const uint8_t* BitmapFont::getCharBitmap(char c) const {
    // TODO: Return actual bitmap data for character
    // This will be filled in when we extract font data from ROM
    (void)c;
    return nullptr;
}

void BitmapFont::drawPixel(int x, int y, const Color& color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer_, x, y);
}

// TextRenderer implementation

TextRenderer::TextRenderer(SDL_Renderer* renderer)
    : renderer_(renderer)
{
    // Fonts will be created on demand
    std::cout << "TextRenderer initialized" << std::endl;
}

TextRenderer::~TextRenderer() {
    // Clean up fonts
    for (auto& pair : fonts_) {
        delete pair.second;
    }
    fonts_.clear();
    std::cout << "TextRenderer destroyed" << std::endl;
}

void TextRenderer::drawText(const std::string& text, int x, int y,
                           BitmapFont::Size size, const Color& color,
                           TextAlignment alignment) {
    if (text.empty()) return;

    BitmapFont* font = getFont(size);
    if (!font) return;

    // Calculate aligned position
    int drawX = getAlignedX(text, x, size, alignment);
    int drawY = y;

    // Draw each character
    for (char c : text) {
        if (c >= 32 && c <= 126) {  // Printable ASCII only
            font->drawChar(c, drawX, drawY, color);
            drawX += font->getCharWidth() + 1;  // +1 for spacing
        } else if (c == '\n') {
            // New line
            drawX = getAlignedX(text, x, size, alignment);
            drawY += font->getCharHeight() + 2;  // +2 for line spacing
        } else if (c == ' ') {
            // Space
            drawX += font->getCharWidth() + 1;
        }
    }
}

void TextRenderer::drawText(const std::string& text, const Vector2& pos,
                           BitmapFont::Size size, const Color& color,
                           TextAlignment alignment) {
    drawText(text, static_cast<int>(pos.x), static_cast<int>(pos.y),
             size, color, alignment);
}

void TextRenderer::addMessage(const Message& message) {
    activeMessages_.push_back(message);
    activeMessages_.back().timeRemaining = message.duration;
}

void TextRenderer::showMessage(const std::string& text, const Vector2& pos,
                              float duration, const Color& color,
                              BitmapFont::Size size) {
    Message msg;
    msg.text = text;
    msg.position = pos;
    msg.color = color;
    msg.fontSize = size;
    msg.duration = duration;
    msg.timeRemaining = duration;
    msg.alignment = TextAlignment::CENTER;
    addMessage(msg);
}

void TextRenderer::update(float deltaTime) {
    // Update timed messages
    for (auto& msg : activeMessages_) {
        if (msg.duration > 0.0f) {
            msg.timeRemaining -= deltaTime;
        }
    }

    // Remove expired messages
    activeMessages_.erase(
        std::remove_if(activeMessages_.begin(), activeMessages_.end(),
            [](const Message& msg) {
                return msg.duration > 0.0f && msg.timeRemaining <= 0.0f;
            }),
        activeMessages_.end()
    );
}

void TextRenderer::render() {
    // Render all active messages
    for (const Message& msg : activeMessages_) {
        drawText(msg.text, msg.position, msg.fontSize, msg.color, msg.alignment);
    }
}

void TextRenderer::clearMessages() {
    activeMessages_.clear();
}

int TextRenderer::measureText(const std::string& text, BitmapFont::Size size) const {
    if (text.empty()) return 0;

    BitmapFont* font = const_cast<TextRenderer*>(this)->getFont(size);
    if (!font) return 0;

    int width = 0;
    for (char c : text) {
        if (c >= 32 && c <= 126) {
            width += font->getCharWidth() + 1;
        } else if (c == ' ') {
            width += font->getCharWidth() + 1;
        }
    }

    return width;
}

BitmapFont* TextRenderer::getFont(BitmapFont::Size size) {
    // Check if font already exists
    auto it = fonts_.find(size);
    if (it != fonts_.end()) {
        return it->second;
    }

    // Create new font
    BitmapFont* font = new BitmapFont(renderer_, size);
    fonts_[size] = font;
    return font;
}

int TextRenderer::getAlignedX(const std::string& text, int x, BitmapFont::Size size,
                             TextAlignment alignment) const {
    if (alignment == TextAlignment::LEFT) {
        return x;
    }

    int width = measureText(text, size);

    switch (alignment) {
        case TextAlignment::CENTER:
            return x - width / 2;
        case TextAlignment::RIGHT:
            return x - width;
        default:
            return x;
    }
}
