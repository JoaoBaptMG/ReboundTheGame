#include "textUtils.hpp"

float fitTextDrawableToHeight(TextDrawable& text, float height, size_t lines)
{
    float curHeight = text.getHeightForLineNumber(lines);
    size_t newSize = text.getFontSize() * height / curHeight;
    text.setFontSize(newSize);
    return text.getHeightForLineNumber(lines);
}