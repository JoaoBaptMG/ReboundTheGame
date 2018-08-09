//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


#pragma once

#include "UIButton.hpp"

#include <string>
#include "language/LangID.hpp"

#include "Services.hpp"

void createCommonTextualButton(UIButton& button, Services& services,
    std::string activeResourceName, std::string pressedResourceName, util::rect centerRect,
    util::rect destRect, LangID captionString, size_t captionSize, glm::u8vec4 textColor,
    float outlineThickness, glm::u8vec4 outlineColor, glm::vec2 captionDisplacement,
    TextDrawable::Alignment wordAlignment = TextDrawable::Alignment::Direct);

void createCommonGraphicsButton(UIButton& button, ResourceManager& rm, std::string normalResourceName,
    std::string activeResourceName, std::string pressedResourceName);

void playConfirm(Services& services);
void playCursor(Services& services);
