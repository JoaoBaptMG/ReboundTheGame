#pragma once

#include <cstdint>

namespace unicode
{
    bool isGraphemeExtender(uint32_t c);
    bool isJapaneseWrappable(uint32_t pc, uint32_t c);
    
    inline static constexpr bool isCharacterWhitespace(uint32_t c)
    {
        if (c >= 0x9 && c <= 0xD) return true;
        if (c >= 0x2000 && c <= 0x200A) return true;
        
        if (c <= 0x2028)
        {
            if (c <= 0xA0) return c == 0x20 || c == 0x85 || c == 0xA0;
            else return c == 0x1680 || c == 0x2028;
        }
        else
        {
            if (c <= 0x202F) return c == 0x2029 || c == 0x202F;
            else return c == 0x205F || c == 0x3000;
        }
    }
    
    inline static constexpr bool isCharacterWrappable(uint32_t c)
    {
        if (c >= 0x2000 && c <= 0x200D) return true;

        if (c <= 0x1680) return c == 0x9 || c == 0x20 || c == 0x1680;
        else return c == 0x180E || c == 0x205F || c == 0x3000;
    }

    inline static constexpr bool isCharacterNewline(uint32_t c)
    {
        if (c >= 0xA && c <= 0xD) return true;
        if (c == 0x85) return true;
        return c == 0x2028 || c == 0x2029;
    }
    
    inline static constexpr bool isCharacterZeroWidth(uint32_t c)
    {
        return (c >= 0x200B && c <= 0x200D) || c == 0x180E;
    }
}
