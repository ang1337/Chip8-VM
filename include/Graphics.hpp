#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <stdint.h>

class Graphics {
    public:
        explicit Graphics(const uint8_t, const uint8_t, const uint8_t, const std::string&);
        ~Graphics() = default;
        template <uint8_t display_width, uint8_t display_height>
        void redraw_screen(const std::array<std::array<uint8_t, display_width>, display_height>&) noexcept;
        sf::RenderWindow window;
    private:
        const uint8_t scale_factor;
};

template <uint8_t display_width, uint8_t display_height>
inline void Graphics::redraw_screen(const std::array<std::array<uint8_t, display_width>, display_height> &display) noexcept {
    window.clear();
    sf::RectangleShape px(sf::Vector2f(scale_factor, scale_factor));
    for (unsigned row {}; row < display_height; row++) {
        for (unsigned col {}; col < display_width; col++) {
            if (display[row][col]) {
                px.setPosition(col * scale_factor, row * scale_factor);
                px.setFillColor(sf::Color::White);
                window.draw(px);
            } 
        }
    }
    window.display();
}
