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

#include "MessageBox.hpp"

#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"
#include "settings/Settings.hpp"
#include "misc/textUtils.hpp"

#include "ColorList.hpp"
#include "defaults.hpp"

#include <minmax.hpp>
#include <utf8.h>
#include <iterator>

#include <iostream>
#include <algorithm>

#include "audio/AudioManager.hpp"
#include "audio/Sound.hpp"

using namespace std::literals::chrono_literals;

constexpr auto DefaultLetterPeriod = 2_frames;
constexpr auto FadeInterval = 24_frames;
constexpr size_t VisibleLines = 4;

constexpr float FullFadePerFrame = 0.025f;
constexpr float MessageVerticalSpacing = 112;

constexpr auto IconOscillationPeriod = 48_frames;

const sf::Color DisplayColors[] =
    {
        Colors::White, Colors::CornflowerBlue, Colors::LightFuchsiaPink, Colors::YellowOrange, Colors::LimeGreen
    };
constexpr size_t DisplayColorNum = sizeof(DisplayColors)/sizeof(DisplayColors[0]);

template <typename T>
static constexpr bool isContained(T) { return false; }

template <typename T, typename... Ts>
static constexpr bool isContained(T comp, T first, Ts... next)
{
    return comp == first || isContained(comp, next...);
}

MessageBox::MessageBox(Services& services)
    : messageBackground(services.resourceManager.load<sf::Texture>("message-background.png")),
      messageIcon(services.resourceManager.load<sf::Texture>("message-next.png")),
      messageText(loadDefaultFont(services)),
      currentText(), localizationManager(services.localizationManager),
      letterPeriod(DefaultLetterPeriod), lineOffset(0), curState(Idle), spawnNewMessage(true)
{
    messageText.setFontSize(24);
    float desiredHeight = messageBackground.getTextureSize().y - 32;
    actualMessageHeight = fitTextDrawableToHeight(messageText, desiredHeight, VisibleLines);

    messageText.setDefaultColor(sf::Color::White);
    messageText.setWordAlignment(TextDrawable::Alignment::Direct);
    messageText.setWordWrappingWidth(messageBackground.getTextureSize().x - 32);
    messageText.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    messageText.setVerticalAnchor(TextDrawable::VertAnchor::Top);
    configTextDrawable(messageText, services.localizationManager);
    
    messageBackground.setOpacity(0);
    messageIcon.setOpacity(0);

    const auto& inputSettings = services.settings.inputSettings;
    messageAction.registerSource(services.inputManager, inputSettings.keyboardSettings.bombInput,   0);
    messageAction.registerSource(services.inputManager, inputSettings.keyboardSettings.dashInput,   1);
    messageAction.registerSource(services.inputManager, inputSettings.keyboardSettings.jumpInput,   2);
    messageAction.registerSource(services.inputManager, inputSettings.keyboardSettings.okInput,     3);
    messageAction.registerSource(services.inputManager, inputSettings.keyboardSettings.cancelInput, 4);

    messageAction.registerSource(services.inputManager, inputSettings.joystickSettings.bombInput,   5);
    messageAction.registerSource(services.inputManager, inputSettings.joystickSettings.dashInput,   6);
    messageAction.registerSource(services.inputManager, inputSettings.joystickSettings.jumpInput,   7);
    messageAction.registerSource(services.inputManager, inputSettings.joystickSettings.okInput,     8);
    messageAction.registerSource(services.inputManager, inputSettings.joystickSettings.cancelInput, 9);
}

void MessageBox::display(Script& script, std::string text)
{
    if (text.empty()) return;

    initTime = curTime + std::max(FadeInterval, UpdatePeriod *
        intmax_t((1 - messageBackground.getOpacity())/FullFadePerFrame));

    currentText = std::move(text);
    lineOffset = 0;
    curState = OpenBox;

    script.waitWhile([=](auto curTime) { return curState != CloseBox; });
}

void MessageBox::displayString(Script &script, const LangID &id)
{
    display(script, localizationManager.getString(id));
}

void MessageBox::displayFormattedString(Script &script, const LangID &id, const StringSpecifierMap &stringSpecifiers,
                                        const NumberSpecifierMap &numberSpecifiers, const RawSpecifierMap& rawSpecifiers)
{
    display(script, localizationManager.getFormattedString(id, stringSpecifiers, numberSpecifiers, rawSpecifiers));
}

void MessageBox::update(FrameTime curTime)
{
    this->curTime = curTime;
    
    messageAction.update();

    auto setAlphaAll = [](float a, TextDrawable::GraphemeRange range)
    {
        if (a < 0) a = 0;
        else if (a > 1) a = 1;

        sf::Uint8 ab = 255*a;
        for (auto& v : range) v.color.a = ab;
    };

    switch (curState)
    {
        case Idle: break;

        case OpenBox:
            if (messageBackground.getOpacity() == 1) curState = FadingPage;
            break;

        case NextCharacter:
        {
            float factor = toSeconds<float>(curTime - initTime) / toSeconds<float>(letterPeriod);
            setAlphaAll(factor, messageText.getGraphemeCluster(curCharacter));
            setAlphaAll(factor, messageText.getGraphemeCluster(curCharacter, true));

            bool moveOn = false;

            if (curTime - initTime >= letterPeriod)
            {
                initTime += letterPeriod;
                curCharacter++;
                moveOn = true;
            }

            if (messageAction.isTriggered())
            {
                size_t lastCur = curCharacter;

                if (curStop != stopPoints.end())
                    curCharacter = std::min(*curStop, *curBreak);
                else curCharacter = *curBreak;

                setAlphaAll(1, messageText.getGraphemeClusterInterval(lastCur, curCharacter));
                setAlphaAll(1, messageText.getGraphemeClusterInterval(lastCur, curCharacter, true));

                moveOn = true;
            }

            if (moveOn)
            {
                if (curStop != stopPoints.end() && *curStop == curCharacter)
                {
                    curState = FullStop;
                    curStop++;
                }
                else if (*curBreak == curCharacter)
                {
                    curState = FullStopPageBreak;
                    curBreak++;
                }
            }
        } break;

        case FullStop:
        case FullStopPageBreak:
        {
            if (messageAction.isTriggered())
            {
                if (curState == FullStopPageBreak)
                {
                    initTime = curTime + FadeInterval;
                    if (curBreak == breakPoints.end())
                    {
                        spawnNewMessage = true;
                        curState = CloseBox;
                    }
                    else curState = FadingPage;
                }
                else
                {
                    initTime = curTime;
                    curState = NextCharacter;
                }
            }
        } break;

        case CloseBox:
            if (messageBackground.getOpacity() == 0) curState = Idle;
			// It is intended to fall through
        case FadingPage:
        {
            float factor = toSeconds<float>(initTime - curTime) / toSeconds<float>(FadeInterval);
            setAlphaAll(factor, messageText.getGraphemeClusterInterval(firstVisibleCharacter, curCharacter));
            setAlphaAll(factor, messageText.getGraphemeClusterInterval(firstVisibleCharacter, curCharacter, true));

            if (curState == FadingPage && curTime > initTime)
            {
                curState = NextCharacter;

                if (spawnNewMessage)
                {
                    buildMessageText();
                    firstVisibleCharacter = curCharacter = 0;
                    curBreak = breakPoints.begin();
                    curStop = stopPoints.begin();

                    setAlphaAll(0, messageText.getAllVertices());
                    setAlphaAll(0, messageText.getAllVertices(true));

                    spawnNewMessage = false;
                }
                else
                {
                    firstVisibleCharacter = curCharacter;
                    lineOffset = messageText.getGraphemeClusterLine(firstVisibleCharacter);
                }
            }
        } break;
    }

    if (!isContained(curState, Idle, CloseBox))
        messageBackground.setOpacity(std::min(1.0f, messageBackground.getOpacity() + FullFadePerFrame));
    else messageBackground.setOpacity(std::max(0.0f, messageBackground.getOpacity() - FullFadePerFrame));

    if (isContained(curState, FullStop, FullStopPageBreak))
        messageIcon.setOpacity(std::min(1.0f, messageIcon.getOpacity() + FullFadePerFrame));
    else messageIcon.setOpacity(std::max(0.0f, messageIcon.getOpacity() - FullFadePerFrame));
}

void MessageBox::buildMessageText()
{
    std::map<size_t,size_t> colorChanges;
    std::list<size_t> tempBreakPoints;
    searchForSpecialMarkers(colorChanges, tempBreakPoints);

    messageText.setString(currentText);
    messageText.buildGeometry();

    auto cit = colorChanges.begin();
    auto bit = tempBreakPoints.begin();
    auto sit = stopPoints.begin();
    sf::Color curColor = DisplayColors[0];

    for (size_t i = 0; i < messageText.getNumberOfGraphemeClusters(); i++)
    {
        auto loc = messageText.getGraphemeClusterByteLocation(i);
        if (cit != colorChanges.end() && loc >= cit->first)
        {
            curColor = DisplayColors[cit->second];
            cit++;
        }

        if (bit != tempBreakPoints.end() && loc >= *bit) *(bit++) = i;
        if (sit != stopPoints.end() && loc >= *sit) *(sit++) = i;

        for (auto& v : messageText.getGraphemeCluster(i)) v.color = curColor;
    }

    autoPlaceBreakPoints(std::move(tempBreakPoints));
}

void MessageBox::autoPlaceBreakPoints(std::list<size_t> tempBreakPoints)
{
    tempBreakPoints.push_back(messageText.getNumberOfGraphemeClusters());

    size_t lastBreak = 0;
    for (auto it = tempBreakPoints.begin(); it != tempBreakPoints.end(); it++)
    {
        auto curLine = messageText.getGraphemeClusterLine(*it);
        auto lastLine = messageText.getGraphemeClusterLine(lastBreak);

        if (curLine - lastLine > VisibleLines)
            it = tempBreakPoints.insert(it, messageText.getLineBoundary(lastLine + VisibleLines - 1));

        lastBreak = *it;
    }

    breakPoints.assign(tempBreakPoints.begin(), tempBreakPoints.end());
}

void MessageBox::searchForSpecialMarkers(std::map<size_t,size_t>& colorChanges, std::list<size_t>& tempBreakPoints)
{
    colorChanges.clear();
    breakPoints.clear();
    stopPoints.clear();

    struct Slice
    {
        std::string::iterator begin, end;
        enum { ColorChange, PageBreak, FullStop } type;
    };

    std::vector<Slice> slices;
    std::vector<size_t> changes;
    for (auto it = currentText.begin(); it != currentText.end();)
    {
        auto oldIt = it;
        uint32_t c = utf8::unchecked::next(it);
        if (c == 0xFFFF)
        {
            if (it == currentText.end()) continue;

            uint32_t p = utf8::unchecked::next(it);
            if (p-1 < DisplayColorNum) changes.push_back(p-1);
            slices.push_back({ oldIt, it, Slice::ColorChange });
        }
        else if (c == '\f') slices.push_back({ oldIt, it, Slice::PageBreak });
        else if (c == '\r') slices.push_back({ oldIt, it, Slice::FullStop });
    }

    auto sliceIt = currentText.begin();
    auto destIt = sliceIt;
    size_t i = 0;
    for (const auto &p : slices)
    {
        if (sliceIt != destIt)
            std::copy(sliceIt, p.begin, destIt);

        destIt += p.begin - sliceIt;
        sliceIt = p.end;

        switch (p.type)
        {
            case Slice::ColorChange:
                destIt = utf8::unchecked::append(0x2060, destIt);
                colorChanges.emplace(destIt - currentText.begin(), changes[i++]);
                break;
            case Slice::PageBreak:
                destIt = utf8::unchecked::append('\n', destIt);
                tempBreakPoints.emplace_back(destIt - currentText.begin());
                break;
            case Slice::FullStop:
                stopPoints.emplace_back(destIt - currentText.begin());
                break;
        }
    }

    std::copy(sliceIt, currentText.end(), destIt);
    destIt += currentText.end() - sliceIt;
    currentText.erase(destIt, currentText.end());
}

void MessageBox::render(Renderer& renderer)
{
    if (curState != Idle)
    {
        float factor = toSeconds<float>(curTime.time_since_epoch() % IconOscillationPeriod) /
            toSeconds<float>(IconOscillationPeriod);
        float pos = roundf(factor < 0.5 ? -4 - 8*factor : -12 + 8*factor);

        renderer.pushTransform();
        renderer.currentTransform.translate(ScreenWidth/2, ScreenHeight - MessageVerticalSpacing);
        renderer.pushDrawable(messageBackground, {}, 4500);
        renderer.currentTransform.translate(0, (float)messageBackground.getTextureSize().y/2 + pos);
        renderer.pushDrawable(messageIcon, {}, 4504);
        renderer.currentTransform.translate(0, -(float)messageBackground.getTextureSize().y/2 - pos);
        renderer.currentTransform.translate(0, -actualMessageHeight/2 - lineOffset * messageText.getLineSpacing());
        renderer.pushDrawable(messageText, {}, 4502);
        renderer.popTransform();
    }
}
