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
#include "language/LocalizationManager.hpp"

#include "data/LevelData.hpp"

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
    
    quitPause.registerSource(im, settings.inputSettings.keyboardSettings.pauseInput);
    quitPause.registerSource(im, settings.inputSettings.joystickSettings.pauseInput);
    
    switchFrameLeft.registerSource(im, settings.inputSettings.keyboardSettings.switchScreenLeft);
    switchFrameLeft.registerSource(im, settings.inputSettings.joystickSettings.switchScreenLeft);
    
    switchFrameRight.registerSource(im, settings.inputSettings.keyboardSettings.switchScreenRight);
    switchFrameRight.registerSource(im, settings.inputSettings.joystickSettings.switchScreenRight);
}

void PauseScene::update(std::chrono::steady_clock::time_point curTime)
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
    transitionTime = curTime - duration_cast<nanoseconds>((1 - transitionFactor) * TransitionTime);
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
