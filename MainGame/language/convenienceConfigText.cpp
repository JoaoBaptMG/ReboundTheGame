#include "convenienceConfigText.hpp"

void configTextDrawable(TextDrawable& text, LocalizationManager& manager)
{
    text.setFontSize(text.getFontSize() * manager.getFontSizeFactor());
    text.setRTL(manager.isRTL());
}

