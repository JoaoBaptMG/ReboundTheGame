#include "UnicodeUtils.hpp"
#include <algorithm>
#include <string>

const uint32_t JpChNumerals[] =
{
    0x4E00, 0x4E03, 0x4E07, 0x4E09, 0x4E5D, 0x4E8C, 0x4E94,
    0x4EBF, 0x5104, 0x5146, 0x516B, 0x516D, 0x5341, 0x5343,
    0x56DB, 0x767E, 0x842C
};
constexpr auto JpChNumeralsEnd = JpChNumerals + sizeof(JpChNumerals)/sizeof(JpChNumerals[0]);

constexpr static bool isJapaneseChineseNumber(uint32_t c)
{
    if (c >= '0' && c <= '9') return true;
    return std::binary_search(JpChNumerals, JpChNumeralsEnd, c);
}

bool unicode::isJapaneseWrappable(uint32_t pc, uint32_t c)
{
    const std::string nextForbidden =
        u8")]｝〕〉》」』】〙〗〟’\"｠»"
        u8"ヽヾーァィゥェォッャュョヮヵヶぁぃぅぇぉっゃゅょゎゕゖㇰㇱㇲㇳㇴㇵㇶㇷㇸㇹㇺㇻㇼㇽㇾㇿ々〻"
        u8"‐゠–〜?!‼⁇⁈⁉・、:;,。.";
        
    const std::string prevForbidden = u8"([｛〔〈《「『【〘〖〝‘\"｟«";

    if (nextForbidden.find(c) != std::string::npos) return false;
    if (prevForbidden.find(pc) != std::string::npos) return false;
    if (isJapaneseChineseNumber(pc) && isJapaneseChineseNumber(c)) return false;
        
    return true;
}
