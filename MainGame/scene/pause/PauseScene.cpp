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


#include "PauseScene.hpp"

#include "PauseFrame.hpp"
#include "CollectedPauseFrame.hpp"
#include "MapPauseFrame.hpp"

#include <chronoUtils.hpp>
#include <defaults.hpp>
#include <cstdlib>

#include "scene/settings/SettingsScene.hpp"

#include "scene/SceneManager.hpp"
#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"

#include "ui/UIButtonCommons.hpp"

#include "data/LevelData.hpp"
#include "language/convenienceConfigText.hpp"

using namespace std::literals::chrono_literals;
constexpr auto TransitionTime = 1s;

const LangID ButtonIdentifiers[] =
{
    "pause-button-collected-items",
    "pause-button-map",
    "pause-button-settings",
};

constexpr float ButtonWidth = 192;
constexpr float ButtonHeight = 44;
constexpr size_t ButtonCaptionSize = 24;

PauseScene::PauseScene(Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm)
    : settings(settings), inputManager(im), resourceManager(rm), localizationManager(lm),
    backgroundSprite(rm.load<sf::Texture>("pause-background.png")), transitionFactor(0),
    pointer(im, rm), unpausing(false), currentFrame(1), pauseFrames
    { 
        std::unique_ptr<PauseFrame>(new CollectedPauseFrame(settings, im, rm, lm)),
        std::unique_ptr<PauseFrame>(new MapPauseFrame(settings, im, rm, lm)),
        std::unique_ptr<PauseFrame>(new SettingsPauseFrame(settings, im, rm, lm, pointer, this)),
    }
{
    for (auto& pauseFrame : pauseFrames) if (pauseFrame) pauseFrame->deactivate();
    pauseFrames[currentFrame]->activate();
    
    int k = 0;
    for (auto& button : frameButtons)
    {
        button.initialize(im);
        
        auto color = k == currentFrame ? sf::Color::Green : sf::Color::White;
        createCommonTextualButton(button, rm, lm, "ui-select-field.png", "ui-select-field.png",
            sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, ButtonWidth, ButtonHeight), ButtonIdentifiers[k],
            ButtonCaptionSize, color, 1, sf::Color::Black, sf::Vector2f(0, 0),
            TextDrawable::Alignment::Center);
        
        button.getPressedSprite()->setBlendColor(sf::Color::Yellow);
        button.getActiveSprite()->setOpacity(0.5);
        button.getActiveSprite()->setOpacity(0.5);
        button.setPosition(sf::Vector2f(ScreenWidth/2 + (k-1) * ButtonWidth, 30));
        button.setDepth(2800);
        
        button.setPressAction([=] { switchPauseFrame(k); });
        
        k++;
    }
    
    quitPause.registerSource(im, settings.inputSettings.keyboardSettings.pauseInput, 0);
    quitPause.registerSource(im, settings.inputSettings.joystickSettings.pauseInput, 1);
    
    switchFrameLeft.registerSource(im, settings.inputSettings.keyboardSettings.switchScreenLeft, 0);
    switchFrameLeft.registerSource(im, settings.inputSettings.joystickSettings.switchScreenLeft, 1);
    
    switchFrameRight.registerSource(im, settings.inputSettings.keyboardSettings.switchScreenRight, 0);
    switchFrameRight.registerSource(im, settings.inputSettings.joystickSettings.switchScreenRight, 1);

    callbackEntry = lm.registerLanguageChangeCallback([=,&lm]
    {
        int k = 0;

        for (auto& button : frameButtons)
        {
            button.getCaption()->setString(lm.getString(ButtonIdentifiers[k]));
            configTextDrawable(*button.getCaption(), lm);
            button.getCaption()->buildGeometry();
            k++;
        }
    });
}

void PauseScene::update(FrameTime curTime)
{
    this->curTime = curTime;
    
    if (isNull(transitionTime)) transitionTime = curTime;
    if (curTime < transitionTime + TransitionTime)
        transitionFactor = toSeconds<float>(curTime - transitionTime) / toSeconds<float>(TransitionTime);
    else transitionFactor = 1;
    
    pauseFrames[currentFrame]->update(curTime);
    
    if (!unpausing)
    {
        quitPause.update();
        switchFrameLeft.update();
        switchFrameRight.update();
        if (quitPause.isTriggered()) unpause();
        if (switchFrameLeft.isTriggered()) switchPauseFrame((currentFrame + 2) % 3);
        if (switchFrameRight.isTriggered()) switchPauseFrame((currentFrame + 1) % 3);
    }
    else
    {
        transitionFactor = 1 - transitionFactor;
        if (transitionFactor <= 0) getSceneManager().popScene();
    }
}

void PauseScene::setMapLevelData(std::shared_ptr<LevelData> level, size_t curRoom, sf::Vector2f pos,
    const std::vector<bool>& visibleMaps)
{
    static_cast<MapPauseFrame*>(pauseFrames[1].get())->setLevelData(level, curRoom, pos, visibleMaps);
}

void PauseScene::setCollectedFrameSavedGame(const SavedGame& sg)
{
    static_cast<CollectedPauseFrame*>(pauseFrames[0].get())->setSavedGame(sg);
}

void PauseScene::unpause()
{
    using namespace std::chrono;
    unpausing = true;
    transitionTime = curTime - duration_cast<FrameDuration>((1 - transitionFactor) * TransitionTime);
}

void PauseScene::render(Renderer &renderer)
{
    renderer.pushTransform();
    
    pointer.render(renderer);
    
    if (transitionFactor < 1)
        renderer.currentTransform.translate(0, ScreenHeight * (1 - transitionFactor) * (1 - transitionFactor));
        
    renderer.pushTransform();
    renderer.currentTransform.translate(ScreenWidth/2, ScreenHeight/2 - 2);
    renderer.pushDrawable(backgroundSprite, {}, 2500);
    renderer.popTransform();
    
    for (auto& button : frameButtons)
        button.render(renderer);
        
    renderer.pushTransform();
    renderer.currentTransform.translate(0, 64);
    pauseFrames[currentFrame]->render(renderer);
    renderer.popTransform();
    
    renderer.popTransform();
    
    getSceneManager().renderBelow();
}

void PauseScene::switchPauseFrame(size_t frame)
{
    frameButtons[currentFrame].getCaption()->setDefaultColor(sf::Color::White);
    frameButtons[currentFrame].getCaption()->buildGeometry();
    pauseFrames[currentFrame]->deactivate();
    
    currentFrame = frame;
    
    frameButtons[currentFrame].getCaption()->setDefaultColor(sf::Color::Green);
    frameButtons[currentFrame].getCaption()->buildGeometry();
    pauseFrames[currentFrame]->activate();
}
