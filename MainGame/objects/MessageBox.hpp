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

#include <cstddef>
#include <string>
#include <vector>
#include <list>
#include <chronoUtils.hpp>
#include "input/CommonActions.hpp"
#include "drawables/Sprite.hpp"
#include "drawables/TextDrawable.hpp"
#include "gameplay/Script.hpp"
#include "language/LocalizationManager.hpp"
#include "language/LangID.hpp"
#include "Services.hpp"

class Renderer;

class MessageBox final
{
    Sprite messageBackground, messageIcon;
    TextDrawable messageText;

    LocalizationManager& localizationManager;
    float actualMessageHeight;

    ButtonAction<10> messageAction;

    std::string currentText;
    FrameDuration letterPeriod;
    FrameTime curTime, initTime;

    std::vector<size_t> breakPoints, stopPoints;

    enum State { Idle, OpenBox, NextCharacter, FullStop, FullStopPageBreak, FadingPage, CloseBox } curState;
    std::vector<size_t>::iterator curBreak, curStop;
    size_t firstVisibleCharacter, curCharacter, lineOffset;
    bool spawnNewMessage;
    
public:
    MessageBox(Services& services);
    ~MessageBox() = default;
    
    void display(Script& script, std::string);
    void displayString(Script& script, const LangID& id);
    void displayFormattedString(Script& script, const LangID& id, const StringSpecifierMap &stringSpecifiers,
                                const NumberSpecifierMap& numberSpecifiers, const RawSpecifierMap& rawSpecifiers);

    void buildMessageText();
    void searchForSpecialMarkers(std::map<size_t,size_t>& colorChanges, std::list<size_t>& tempBreakPoints);
    void autoPlaceBreakPoints(std::list<size_t> tempBreakPoints);

    void update(FrameTime curTime);
    void render(Renderer& renderer);
};
