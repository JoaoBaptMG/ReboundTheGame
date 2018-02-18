#include "KeyboardKeyName.hpp"

#include <iostream>

#if _WIN32

#elif __APPLE__

#elif __linux__

#include <X11/Xlib.h>
#include <cstdlib>
#include <cctype>
#include <utf8.h>

namespace sf
{
    namespace priv
    {
        Display* OpenDisplay();
        void CloseDisplay(Display* display);
    }
}

static KeySym* keysyms = nullptr;
static int minKeys, maxKeys, keysymsPerKeycode;

static void initializeKeyMapper()
{
    auto display = sf::priv::OpenDisplay();
    XDisplayKeycodes(display, &minKeys, &maxKeys);
    keysyms = XGetKeyboardMapping(display, minKeys, maxKeys - minKeys + 1, &keysymsPerKeycode);
    sf::priv::CloseDisplay(display);

    std::atexit([]{ XFree(keysyms); });
}

extern const sf::Uint32 codeMaps0x100[96], codeMaps0x200[96], codeMaps0x300[96], codeMaps0x400[64];
extern const sf::Uint32 codeMaps0x600[96], codeMaps0x700[32], codeMaps0x800[96], codeMaps0x900[32];
extern const sf::Uint32 codeMaps0xA00[96], codeMaps0xB00[96], codeMaps0xC00[32], codeMaps0xD00[96];

std::string scanCodeToKeyName(sf::Uint32 code)
{
    if (!keysyms) initializeKeyMapper();

    size_t i = (code - minKeys) * keysymsPerKeycode;
    for (size_t k = 0; k < keysymsPerKeycode; k++)
    {
        auto key = keysyms[i];

        sf::Uint32 charCode = 0;

        // ASCII character;
        if (key >= 32 && key <= 126) charCode = islower(key) ? toupper(key) : key;
        else if (key >= 0xA0 && key <= 0xDF) charCode = key;
        else if (key == 0xF7) charCode = 0xF7;
        else if (key >= 0xE0 && key <= 0xFE) charCode = key - 0x20;
        else if (key >= 0x1A0 && key <= 0x1FF) charCode = codeMaps0x100[key - 0x1A0];
        else if (key >= 0x2A0 && key <= 0x2FF) charCode = codeMaps0x200[key - 0x2A0];
        else if (key >= 0x3A0 && key <= 0x3FF) charCode = codeMaps0x300[key - 0x3A0];
        else if (key == 0x47E) charCode = 0x203E;
        else if (key >= 0x4A0 && key <= 0x4DF) charCode = codeMaps0x400[key - 0x4A0];
        else if (key >= 0x5A0 && key <= 0x5FF) charCode = key + 0x60;
        else if (key >= 0x6A0 && key <= 0x6FF) charCode = codeMaps0x600[key - 0x6A0];
        else if (key >= 0x7A0 && key <= 0x7BF) charCode = codeMaps0x700[key - 0x7A0];
        else if (key >= 0x7C0 && key <= 0x7FF) charCode = key + 0x40;
        else if (key >= 0x8A0 && key <= 0x8FF) charCode = codeMaps0x800[key - 0x8A0];
        else if (key >= 0x9E0 && key <= 0x9FF) charCode = codeMaps0x900[key - 0x9E0];
        else if (key >= 0xAA0 && key <= 0xAFF) charCode = codeMaps0xA00[key - 0xAA0];
        else if (key >= 0xBA0 && key <= 0xBFF) charCode = codeMaps0xB00[key - 0xBA0];
        else if (key == 0xCDF) charCode = 0x2017;
        else if (key >= 0xCE0 && key <= 0xCFF) charCode = codeMaps0xC00[key - 0xCE0];
        else if (key >= 0xDA0 && key <= 0xDFF) charCode = codeMaps0xD00[key - 0xDA0];
        else if (key >= 0x20AC) charCode = 0x20AC;
        else if (key >= 0x1000000) charCode = key & 0xFFFF;
        else if (key >= 0xFFB0 && key <= 0xFFB9) return "Num " + std::to_string(key - 0xFFB0);
        else if (key >= 0xFFBE && key <= 0xFFE0) return 'F' + std::to_string(key - 0xFFBD);

        if (charCode > 0)
        {
            char val[8];
            *(utf8::unchecked::append(charCode, val)) = 0;
            return val;
        }
    }

    return "";
}

#endif