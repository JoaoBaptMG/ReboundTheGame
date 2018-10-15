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

#include "ColorList.hpp"

#include "data/LevelData.hpp"
#include "language/convenienceConfigText.hpp"
#include "rendering/Texture.hpp"

using namespace std::literals::chrono_literals;
constexpr auto TransitionTime = 1s;

const LangID ButtonIdentifiers[] =
{
    "pause-button-collected-items",
    "pause-button-map",
    "pause-button-settings",
};

constexpr float ButtonWidth = PlayfieldWidth/3;
constexpr float ButtonHeight = 44;
constexpr size_t ButtonCaptionSize = 24;

PauseScene::PauseScene(Services& services) : services(services),
    backgroundSprite(services.resourceManager.load<Texture>("pause-background.png")), transitionFactor(0),
    pointer(services), unpausing(false), currentFrame(1), pauseFrames
    { 
        std::unique_ptr<PauseFrame>(new CollectedPauseFrame(services)),
        std::unique_ptr<PauseFrame>(new MapPauseFrame(services)),
        std::unique_ptr<PauseFrame>(new SettingsPauseFrame(services, pointer, this)),
    }
{
    for (auto& pauseFrame : pauseFrames) if (pauseFrame) pauseFrame->deactivate();
    pauseFrames[currentFrame]->activate();

    int k = 0;
    for (auto& button : frameButtons)
    {
        button.initialize(services.inputManager);
        
        auto color = k == currentFrame ? Colors::Green : Colors::White;
        createCommonTextualButton(button, services, "ui-select-field.png", "ui-select-field.png",
            util::rect(16, 0, 8, 1), util::rect(0, 0, ButtonWidth, ButtonHeight), ButtonIdentifiers[k],
            ButtonCaptionSize, color, 1, Colors::Black, glm::vec2(0, 0),
            TextDrawable::Alignment::Center);
        
        button.getPressedSprite()->setBlendColor(Colors::Yellow);
        button.getActiveSprite()->setOpacity(0.5);
        button.getActiveSprite()->setOpacity(0.5);
        button.setPosition(glm::vec2(ScreenWidth/2 + (k-1) * ButtonWidth, 30));
        button.setDepth(5000);
        
        button.setPressAction([=,&services] { playConfirm(services); switchPauseFrame(k); });
        
        k++;
    }

    auto& inputSettings = services.settings.inputSettings;
    
    quitPause.registerSource(services.inputManager, inputSettings.keyboardSettings.pauseInput, 0);
    quitPause.registerSource(services.inputManager, inputSettings.joystickSettings.pauseInput, 1);
    
    switchFrameLeft.registerSource(services.inputManager, inputSettings.keyboardSettings.switchScreenLeft, 0);
    switchFrameLeft.registerSource(services.inputManager, inputSettings.joystickSettings.switchScreenLeft, 1);
    
    switchFrameRight.registerSource(services.inputManager, inputSettings.keyboardSettings.switchScreenRight, 0);
    switchFrameRight.registerSource(services.inputManager, inputSettings.joystickSettings.switchScreenRight, 1);

    callbackEntry = services.localizationManager.registerLanguageChangeCallback(
    [=, &lm = services.localizationManager]
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

    playConfirm(services);
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
        if (switchFrameLeft.isTriggered())
        {
            playCursor(services);
            switchPauseFrame((currentFrame + 2) % 3);
        }
        if (switchFrameRight.isTriggered())
        {
            playCursor(services);
            switchPauseFrame((currentFrame + 1) % 3);
        }
    }
    else
    {
        transitionFactor = 1 - transitionFactor;
        if (transitionFactor <= 0) getSceneManager().popScene();
    }
}

void PauseScene::setMapLevelData(std::shared_ptr<LevelData> level, size_t curRoom, glm::vec2 pos,
    const std::vector<bool>& visibleMaps)
{
    static_cast<MapPauseFrame*>(pauseFrames[1].get())->setLevelData(level, curRoom, pos, visibleMaps);
}

void PauseScene::setCollectedFrameSavedGame(const SavedGame& sg)
{
    static_cast<CollectedPauseFrame*>(pauseFrames[0].get())->setSavedGame(sg);
}

void PauseScene::setCollectedFrameHealthData(size_t curHealth, size_t maxHealth)
{
    static_cast<CollectedPauseFrame*>(pauseFrames[0].get())->setHealthData(curHealth, maxHealth);
}

void PauseScene::unpause()
{
    using namespace std::chrono;
    unpausing = true;
    transitionTime = curTime - duration_cast<FrameDuration>((1 - transitionFactor) * TransitionTime);

    playConfirm(services);
}

void PauseScene::render(Renderer &renderer)
{
    renderer.pushTransform();
    
    pointer.render(renderer);
    
    if (transitionFactor < 1)
        renderer.currentTransform *= util::translate(0, ScreenHeight * (1 - transitionFactor) * (1 - transitionFactor));
        
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(ScreenWidth/2, ScreenHeight/2 - 2);
    renderer.pushDrawable(backgroundSprite, 4800);
    renderer.popTransform();
    
    for (auto& button : frameButtons)
        button.render(renderer);
        
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(0, 64);
    pauseFrames[currentFrame]->render(renderer);
    renderer.popTransform();
    
    renderer.popTransform();
    
    getSceneManager().renderBelow();
}

void PauseScene::switchPauseFrame(size_t frame)
{
    frameButtons[currentFrame].getCaption()->setDefaultColor(Colors::White);
    frameButtons[currentFrame].getCaption()->buildGeometry();
    pauseFrames[currentFrame]->deactivate();
    
    currentFrame = frame;
    
    frameButtons[currentFrame].getCaption()->setDefaultColor(Colors::Green);
    frameButtons[currentFrame].getCaption()->buildGeometry();
    pauseFrames[currentFrame]->activate();
}
