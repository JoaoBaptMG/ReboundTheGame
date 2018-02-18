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

using namespace std::literals::chrono_literals;

constexpr auto DefaultLetterPeriod = 2 * UpdateFrequency;
constexpr auto FadeInterval = 24 * UpdateFrequency;
constexpr size_t VisibleLines = 4;

constexpr float FullFadePerFrame = 0.025;

constexpr float MessageVerticalSpacing = 112;

const sf::Color DisplayColors[] =
    {
        Colors::White, Colors::CornflowerBlue, Colors::LightFuchsiaPink, Colors::YellowOrange, Colors::LimeGreen
    };
constexpr size_t DisplayColorNum = sizeof(DisplayColors)/sizeof(DisplayColors[0]);

MessageBox::MessageBox(const Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm)
    : messageBackground(rm.load<sf::Texture>("message-background.png")), localizationManager(lm),
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

    currentText = text;
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

    if (currentText.empty())
    {
        messageBackground.setOpacity(std::max(0.0f, messageBackground.getOpacity() - FullFadePerFrame));

        size_t begin = lineOffset == 0 ? 0 : messageText.getLineBoundary(lineOffset-1);
        size_t end = messageText.getLineBoundary(lineOffset + VisibleLines-1);
        float factor = toSeconds<float>(curTime - initTime) / toSeconds<float>(FadeInterval);

        for (auto& v : messageText.getGraphemeClusterInterval(begin, end))
            v.color.a = std::max<intmax_t>(0, 255 * (1 - factor));
        for (auto& v : messageText.getGraphemeClusterInterval(begin, end, true))
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
            size_t begin = lineOffset == 0 ? 0 : messageText.getLineBoundary(lineOffset-1);
            size_t end = messageText.getLineBoundary(lineOffset + VisibleLines-1);

            float factor = toSeconds<float>(initTime - curTime) / toSeconds<float>(FadeInterval);
            for (auto& v : messageText.getGraphemeClusterInterval(begin, end)) v.color.a = 255 * factor;
            for (auto& v : messageText.getGraphemeClusterInterval(begin, end)) v.color.a = 255 * factor;
        }
        else
        {
            if (messageText.getString() != currentText)
            {
                buildMessageText();

                for (auto& v : messageText.getAllVertices()) v.color.a = 0;
                for (auto& v : messageText.getAllVertices(true)) v.color.a = 0;
            }

            if (updateLines)
            {
                lineOffset += VisibleLines;
                updateLines = false;
            }

            size_t begin = lineOffset == 0 ? 0 : messageText.getLineBoundary(lineOffset-1);
            size_t end = messageText.getLineBoundary(lineOffset + VisibleLines-1);

            if (!messageText.getString().empty())
            {
                size_t id = (curTime - initTime) / letterPeriod;
                auto remain = (curTime - initTime) % letterPeriod;
                id += begin;

                if (id < end)
                {
                    float factor = toSeconds<float>(remain) / toSeconds<float>(letterPeriod);

                    if (id > begin)
                    {
                        for (auto& v : messageText.getGraphemeCluster(id - 1)) v.color.a = 255;
                        for (auto& v : messageText.getGraphemeCluster(id - 1, true)) v.color.a = 255;
                    }

                    for (auto& v : messageText.getGraphemeCluster(id)) v.color.a = 255 * factor;
                    for (auto& v : messageText.getGraphemeCluster(id, true)) v.color.a = 255 * factor;
                    
                    if (messageAction.isTriggered())
                    {
                        initTime = curTime - letterPeriod * (end - begin);

                        for (auto& v : messageText.getGraphemeClusterInterval(begin, end)) v.color.a = 255;
                        for (auto& v : messageText.getGraphemeClusterInterval(begin, end, true)) v.color.a = 255;
                    }
                }
                else
                {
                    if (end > 0)
                    {
                        for (auto& v : messageText.getGraphemeCluster(end - 1)) v.color.a = 255;
                        for (auto& v : messageText.getGraphemeCluster(end - 1, true)) v.color.a = 255;
                    }

                    if (messageAction.isTriggered())
                    {
                        if (lineOffset + VisibleLines < messageText.getNumberOfLines())
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
    searchForColorMarkers();

    messageText.setString(currentText);
    messageText.buildGeometry();

    auto it = colorChanges.begin();
    sf::Color curColor = DisplayColors[0];

    for (size_t i = 0; i < messageText.getNumberOfGraphemeClusters(); i++)
    {
        auto loc = messageText.getGraphemeClusterByteLocation(i);
        if (it != colorChanges.end() && loc >= it->first)
        {
            curColor = DisplayColors[it->second];
            it++;
        }

        for (auto& v : messageText.getGraphemeCluster(i)) v.color = curColor;
    }
}

void MessageBox::searchForColorMarkers()
{
    colorChanges.clear();

    using Iterator = decltype(currentText.begin());

    std::vector<std::pair<Iterator, Iterator>> slices;
    std::vector<size_t> changes;
    for (auto it = currentText.begin(); it != currentText.end();)
    {
        auto oldIt = it;
        uint32_t c = utf8::unchecked::next(it);
        if (c == 0xFFFF)
        {
            if (it == currentText.end()) continue;

            uint32_t p = utf8::unchecked::next(it);
            if (p - '0' < DisplayColorNum) changes.push_back(p - '0');

            slices.emplace_back(oldIt, it);
        }
    }

    auto sliceIt = currentText.begin();
    auto destIt = sliceIt;
    size_t i = 0;
    for (const auto &p : slices)
    {
        if (sliceIt != destIt)
            std::copy(sliceIt, p.first, destIt);

        destIt = utf8::unchecked::append(0x2060, destIt + (p.first - sliceIt));
        sliceIt = p.second;

        colorChanges.emplace(destIt - currentText.begin(), changes[i++]);
    }

    std::copy(sliceIt, currentText.end(), destIt);
    destIt += currentText.end() - sliceIt;
    currentText.erase(destIt, currentText.end());
}

void MessageBox::render(Renderer& renderer)
{
    if (messageBackground.getOpacity() > 0)
    {
        renderer.pushTransform();
        renderer.currentTransform.translate(ScreenWidth/2, ScreenHeight - MessageVerticalSpacing);
        renderer.pushDrawable(messageBackground, {}, 4500);
        renderer.currentTransform.translate(0, -actualMessageHeight/2 - lineOffset * messageText.getLineSpacing());
        renderer.pushDrawable(messageText, {}, 4502);
        renderer.popTransform();
    }
}
