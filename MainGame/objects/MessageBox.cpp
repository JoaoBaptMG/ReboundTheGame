#include "MessageBox.hpp"

#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"
#include "settings/Settings.hpp"

#include "defaults.hpp"

#include <minmax.hpp>

using namespace std::literals::chrono_literals;

constexpr auto DefaultLetterPeriod = 5 * UpdateFrequency;
constexpr auto FadeInterval = 24 * UpdateFrequency;

constexpr float FullFadePerFrame = 0.025;

constexpr float MessageVerticalSpacing = 112;

MessageBox::MessageBox(const Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm)
    : messageBackground(rm.load<sf::Texture>("message-background.png")),
    messageText(rm.load<FontHandler>(lm.getFontName())), currentText(), letterPeriod(DefaultLetterPeriod)
{
    messageText.setFontSize(24);
    messageText.setDefaultColor(sf::Color::White);
    messageText.setWordAlignment(TextDrawable::Alignment::Direct);
    messageText.setWordWrappingWidth(messageBackground.getTextureSize().x - 32);
    messageText.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    messageText.setVerticalAnchor(TextDrawable::VertAnchor::Top);
    configTextDrawable(messageText, lm);
    
    messageBackground.setOpacity(0);
    
    messageAction.registerSource(im, settings.inputSettings.keyboardSettings.bombInput);
    messageAction.registerSource(im, settings.inputSettings.keyboardSettings.dashInput);
    messageAction.registerSource(im, settings.inputSettings.keyboardSettings.jumpInput);
    messageAction.registerSource(im, settings.inputSettings.joystickSettings.bombInput);
    messageAction.registerSource(im, settings.inputSettings.joystickSettings.dashInput);
    messageAction.registerSource(im, settings.inputSettings.joystickSettings.jumpInput);
}

void MessageBox::display(Script& script, std::string text)
{
    initTime = curTime + std::max(FadeInterval, UpdateFrequency *
        intmax_t((1 - messageBackground.getOpacity())/FullFadePerFrame));
    
    currentText = text;
    
    script.waitUntil([=] (auto curTime) { return !currentText.empty(); });
}

void MessageBox::update(std::chrono::steady_clock::time_point curTime)
{
    this->curTime = curTime;
    
    messageAction.update();
    
    if (currentText.empty())
    {
        messageBackground.setOpacity(std::max(0.0f, messageBackground.getOpacity() - FullFadePerFrame));
        
        float factor = toSeconds<float>(curTime - initTime) / toSeconds<float>(FadeInterval);
        
        for (auto& v : messageText.getAllVertices()) v.color.a = std::max<intmax_t>(0, 255 * (1 - factor));
        for (auto& v : messageText.getAllVertices(true)) v.color.a = std::max<intmax_t>(0, 255 * (1 - factor));
    }
    else
    {
        messageBackground.setOpacity(std::min(1.0f, messageBackground.getOpacity() + FullFadePerFrame));
        
        if (curTime < initTime)
        {
            float factor = toSeconds<float>(initTime - curTime) / toSeconds<float>(Fade Interval);
            
            for (auto& v : messageText.getAllVertices()) v.color.a = 255 * factor;
            for (auto& v : messageText.getAllVertices(true)) v.color.a = 255 * factor;
        }
        else
        {
            if (messageText.getString() != currentText)
            {
                messageText.setString(currentText);
                messageText.buildGeometry();
                
                for (auto& v : messageText.getAllVertices()) v.color.a = 0;
                for (auto& v : messageText.getAllVertices(true)) v.color.a = 0;
            }
            
            if (!messageText.getString().empty())
            {
                size_t id = (curTime - initTime) / letterPeriod;
                auto remain = (curTime - initTime) % letterPeriod;
                
                if (id < messageText.getNumberOfGraphemeClusters())
                {
                    float factor = toSeconds<float>(remain) / toSeconds<float>(letterPeriod);
                    
                    for (auto& v : messageText.getGraphemeCluster(id)) v.color.a = 255 * factor;
                    for (auto& v : messageText.getGraphemeCluster(id, true)) v.color.a = 255 * factor;
                    
                    if (messageAction.isTriggered())
                    {
                        initTime = curTime - letterPeriod * messageText.getNumberOfGraphemeClusters();
                        
                        for (auto& v : messageText.getAllVertices()) v.color.a = 255;
                        for (auto& v : messageText.getAllVertices(true)) v.color.a = 255;
                    }
                }
                else
                {
                    if (messageAction.isTriggered())
                    {
                        currentText.clear();
                        initTime = curTime;
                    }
                }
            }
        }
    }
}

void MessageBox::render(Renderer& renderer)
{
    if (messageBackground.getOpacity() > 0)
    {
        renderer.pushTransform();
        renderer.currentTransform.translate(ScreenWidth/2, ScreenHeight - MessageVerticalSpacing);
        renderer.pushDrawable(messageBackground, {}, 4500);
        renderer.currentTransform.translate(0, -(float)messageBackground.getTextureSize().y/2 + 16);
        renderer.pushDrawable(messageText, {}, 4502);
        renderer.popTransform();
    }
}
