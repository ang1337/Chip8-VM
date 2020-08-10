#include "../include/Graphics.hpp"
#include <string>

Graphics::Graphics(const uint8_t width, const uint8_t height, const uint8_t scale_factor, const std::string &title) 
    : window(sf::VideoMode(width * scale_factor, height * scale_factor), title.data()),
      scale_factor(scale_factor) {
        // centralize the window
        auto desktop { sf::VideoMode::getDesktopMode() };
        window.setPosition(sf::Vector2i(desktop.width / 4, desktop.height / 4));
    }

