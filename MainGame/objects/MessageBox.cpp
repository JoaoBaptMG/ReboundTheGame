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

using namespace std::literals::chrono_literals;

constexpr auto DefaultLetterPeriod = 2 * UpdateFrequency;
constexpr auto FadeInterval = 24 * UpdateFrequency;
constexpr size_t VisibleLines = 4;

constexpr float FullFadePerFrame = 0.025;
constexpr float MessageVerticalSpacing = 112;

constexpr auto IconOscillationPeriod = 48 * UpdateFrequency;

const sf::Color DisplayColors[] =
    {
        Colors::White, Colors::CornflowerBlue, Colors::LightFuchsiaPink, Colors::YellowOrange, Colors::LimeGreen
    };
constexpr size_t DisplayColorNum = sizeof(DisplayColors)/sizeof(DisplayColors[0]);

MessageBox::MessageBox(const Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm)
    : messageBackground(rm.load<sf::Texture>("message-background.png")),
      messageIcon(rm.load<sf::Texture>("message-next.png")), localizationManager(lm),
      messageText(rm.load<FontHandler>(lm.getFontName())), currentText(), letterPeriod(DefaultLetterPeriod),
      lineOffset(0), updateLines(false)
{
    messageText.setFontSize(24);
    float desiredHeight = messageBackground.getTextureSize().y - 32;
    actualMessageHeight = fitTextDrawableToHeight(messageText, desiredHeight, VisibleLines);

    messageText.setDefaultColor(sf::Color::White);
    messageText.setWordAlignment(TextDrawable::Alignment::Direct);
    messageText.setWordWrappingWidth(messageBackground.getTextureSize().x - 32);
    messageText.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    messageText.setVerticalAnchor(TextDrawable::VertAnchor::Top);
    configTextDrawable(messageText, lm);
    
    messageBackground.setOpacity(0);
    messageIcon.setOpacity(0);

    messageAction.registerSource(im, settings.inputSettings.keyboardSettings.bombInput, 0);
    messageAction.registerSource(im, settings.inputSettings.keyboardSettings.dashInput, 1);
    messageAction.registerSource(im, settings.inputSettings.keyboardSettings.jumpInput, 2);
    messageAction.registerSource(im, settings.inputSettings.joystickSettings.bombInput, 3);
    messageAction.registerSource(im, settings.inputSettings.joystickSettings.dashInput, 4);
    messageAction.registerSource(im, settings.inputSettings.joystickSettings.jumpInput, 5);
}

void MessageBox::display(Script& script, std::string text)
{
    initTime = curTime + std::max(FadeInterval, UpdateFrequency *
        intmax_t((1 - messageBackground.getOpacity())/FullFadePerFrame));

    currentText = std::move(text);
    lineOffset = 0;

    script.waitUntil([=] (auto curTime) { return !currentText.empty(); });
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

void MessageBox::update(std::chrono::steady_clock::time_point curTime)
{
    this->curTime = curTime;
    
    messageAction.update();

    auto calculateBounds = [=]
    {
        VisibleBounds bounds{};
        bounds.begin = lineOffset == 0 ? 0 : messageText.getLineBoundary(lineOffset-1);
        bounds.end = messageText.getLineBoundary(lineOffset + VisibleLines-1);
        bounds.lineCount = VisibleLines;

        auto it1 = std::upper_bound(breakPoints.begin(), breakPoints.end(), bounds.begin);
        auto it2 = std::upper_bound(breakPoints.begin(), breakPoints.end(), bounds.end);

        if (it1 != it2)
        {
            bounds.end = *it1;
            bounds.lineCount = messageText.getGraphemeClusterLine(bounds.end) - lineOffset;
        }
        return bounds;
    };

    if (currentText.empty())
    {
        messageBackground.setOpacity(std::max(0.0f, messageBackground.getOpacity() - FullFadePerFrame));
        messageIcon.setOpacity(std::max(0.0f, messageIcon.getOpacity() - FullFadePerFrame));

        float factor = toSeconds<float>(curTime - initTime) / toSeconds<float>(FadeInterval);

        for (auto& v : messageText.getGraphemeClusterInterval(curBounds.begin, curBounds.end))
            v.color.a = std::max<intmax_t>(0, 255 * (1 - factor));
        for (auto& v : messageText.getGraphemeClusterInterval(curBounds.begin, curBounds.end, true))
            v.color.a = std::max<intmax_t>(0, 255 * (1 - factor));

        if (messageBackground.getOpacity() == 0 && !messageText.getString().empty())
        {
            messageText.setString("");
            messageText.buildGeometry();
        }
    }
    else
    {
        messageBackground.setOpacity(std::min(1.0f, messageBackground.getOpacity() + FullFadePerFrame));
        
        if (curTime <= initTime)
        {
            messageIcon.setOpacity(std::max(0.0f, messageIcon.getOpacity() - FullFadePerFrame));

            float factor = toSeconds<float>(initTime - curTime) / toSeconds<float>(FadeInterval);
            for (auto& v : messageText.getGraphemeClusterInterval(curBounds.begin, curBounds.end))
                v.color.a = 255 * factor;
            for (auto& v : messageText.getGraphemeClusterInterval(curBounds.begin, curBounds.end, true))
                v.color.a = 255 * factor;
        }
        else
        {
            if (messageText.getString() != currentText)
            {
                buildMessageText();
                curBounds = calculateBounds();

                for (auto& v : messageText.getAllVertices()) v.color.a = 0;
                for (auto& v : messageText.getAllVertices(true)) v.color.a = 0;
            }

            if (updateLines)
            {
                lineOffset += curBounds.lineCount;
                updateLines = false;

                curBounds = calculateBounds();
            }

            if (!messageText.getString().empty())
            {
                size_t id = (curTime - initTime) / letterPeriod;
                auto remain = (curTime - initTime) % letterPeriod;
                id += curBounds.begin;

                if (id < curBounds.end)
                {
                    messageIcon.setOpacity(std::max(0.0f, messageIcon.getOpacity() - FullFadePerFrame));
                    float factor = toSeconds<float>(remain) / toSeconds<float>(letterPeriod);

                    if (id > curBounds.begin)
                    {
                        for (auto& v : messageText.getGraphemeCluster(id - 1)) v.color.a = 255;
                        for (auto& v : messageText.getGraphemeCluster(id - 1, true)) v.color.a = 255;
                    }

                    for (auto& v : messageText.getGraphemeCluster(id)) v.color.a = 255 * factor;
                    for (auto& v : messageText.getGraphemeCluster(id, true)) v.color.a = 255 * factor;
                    
                    if (messageAction.isTriggered())
                    {
                        initTime = curTime - letterPeriod * (curBounds.end - curBounds.begin);

                        for (auto& v : messageText.getGraphemeClusterInterval(curBounds.begin, curBounds.end))
                            v.color.a = 255;
                        for (auto& v : messageText.getGraphemeClusterInterval(curBounds.begin, curBounds.end, true))
                            v.color.a = 255;
                    }
                }
                else
                {
                    messageIcon.setOpacity(std::min(1.0f, messageIcon.getOpacity() + FullFadePerFrame));

                    if (curBounds.end > 0)
                    {
                        for (auto& v : messageText.getGraphemeCluster(curBounds.end - 1)) v.color.a = 255;
                        for (auto& v : messageText.getGraphemeCluster(curBounds.end - 1, true)) v.color.a = 255;
                    }

                    if (messageAction.isTriggered())
                    {
                        if (lineOffset + curBounds.lineCount < messageText.getNumberOfLines())
                        {
                            initTime = curTime + FadeInterval;
                            updateLines = true;
                        }
                        else
                        {
                            currentText.clear();
                            initTime = curTime;
                        }
                    }
                }
            }
        }
    }
}

void MessageBox::buildMessageText()
{
    std::map<size_t,size_t> colorChanges;
    searchForSpecialMarkers(colorChanges);

    messageText.setString(currentText);
    messageText.buildGeometry();

    auto cit = colorChanges.begin();
    auto bit = breakPoints.begin();
    sf::Color curColor = DisplayColors[0];

    for (size_t i = 0; i < messageText.getNumberOfGraphemeClusters(); i++)
    {
        auto loc = messageText.getGraphemeClusterByteLocation(i);
        if (cit != colorChanges.end() && loc >= cit->first)
        {
            curColor = DisplayColors[cit->second];
            cit++;
        }

        if (bit != breakPoints.end() && loc >= *bit)
        {
            *bit = i;
            bit++;
        }

        for (auto& v : messageText.getGraphemeCluster(i)) v.color = curColor;
    }
}

void MessageBox::searchForSpecialMarkers(std::map<size_t,size_t>& colorChanges)
{
    colorChanges.clear();
    breakPoints.clear();

    struct Slice
    {
        std::string::iterator begin, end;
        enum { ColorChange, PageBreak } type;
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
                breakPoints.emplace_back(destIt - currentText.begin());
                break;
        }
    }

    std::copy(sliceIt, currentText.end(), destIt);
    destIt += currentText.end() - sliceIt;
    currentText.erase(destIt, currentText.end());
}

void MessageBox::render(Renderer& renderer)
{
    if (messageBackground.getOpacity() > 0)
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
