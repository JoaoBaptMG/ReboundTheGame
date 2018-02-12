#pragma once

#include <SFML/Graphics.hpp>
#include <generic_ptrs.hpp>
#include <memory>
#include "unicode/HarfBuzzWrapper.hpp"

class FontHandler final
{
    std::unique_ptr<const char[]> fontData;
    sf::Font font;
    HarfBuzzWrapper harfBuzzWrapper;
    
public:
    FontHandler(std::unique_ptr<const char[]> &&data, size_t size);
    ~FontHandler() {}
    
    FontHandler(const FontHandler&) = delete;
    FontHandler& operator=(const FontHandler&) = delete;
    
    auto& getFont() { return font; }
    auto& getHBWrapper() { return harfBuzzWrapper; }
};

util::generic_shared_ptr loadFontHandler(std::unique_ptr<sf::InputStream>&);
