#include "TitleScene.hpp"

#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIButtonCommons.hpp"
#include "defaults.hpp"
#include "scene/SceneManager.hpp"
#include "language/LocalizationManager.hpp"

#include "GameScene.hpp"
#include "SettingsScene.hpp"

using namespace std::literals::chrono_literals;

constexpr float ButtonHeight = 40;
constexpr float ButtonSpace = 10;
constexpr float SetBottomSpace = 24;
constexpr size_t ButtonCaptionSize = 32;
constexpr float ButtonTop = ScreenHeight - SetBottomSpace + ButtonSpace;

const LangID ButtonIdentifiers[] =
{
    "title-button-new-game",
    "title-button-load-game",
    "title-button-settings",
    "title-button-exit",
};

TitleScene::TitleScene(Settings& settings, InputManager& inputManager, ResourceManager& rm, LocalizationManager& lm)
    : background(rm.load<sf::Texture>("title-background.png")), foreground(rm.load<sf::Texture>("title-foreground.png")),
    pointer(inputManager, rm), buttonGroup(inputManager, settings.inputSettings)
{
    rtl = lm.isRTL();
    
    background.setAnchorPoint(sf::Vector2f(0, 0));
    foreground.setAnchorPoint(sf::Vector2f(0, 0));
    
    int k = 0;
    for (auto& button : buttons)
    {
        button.initialize(inputManager);
        
        createCommonTextualButton(button, rm, lm, "ui-select-field.png", "ui-select-field.png",
            sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, ScreenWidth - 2 * ButtonSpace, ButtonHeight),
            ButtonIdentifiers[k], ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0));
        
        button.getPressedSprite()->setBlendColor(sf::Color::Yellow);
        button.getActiveSprite()->setOpacity(0.5);
        button.getActiveSprite()->setOpacity(0.5);
        button.setPosition(sf::Vector2f(ScreenWidth/2, ButtonTop + ButtonHeight/2 - (4-k) * (ButtonHeight + ButtonSpace)));
        button.setDepth(10);
        
        k++;
    }
    
    buttons[0].setPressAction([&,this]
    {
        if (this != getSceneManager().currentScene()) return;
        
        auto scene = new GameScene(settings, inputManager, rm, lm);
        scene->loadLevel("level1.lvl");
        getSceneManager().replaceSceneTransition(scene, 1s);
    });
    
    buttons[2].setPressAction([&,this]
    {
        if (this != getSceneManager().currentScene()) return;
        
        auto scene = new SettingsScene(settings, inputManager, rm, lm);
        getSceneManager().replaceSceneTransition(scene, 1s);
    });
    
    buttons[3].setPressAction([&,this]
    {
        if (this != getSceneManager().currentScene()) return;
        getSceneManager().popScene();
    });
    
    buttonGroup.setButtons(buttons);
    buttonGroup.setPointer(pointer);
}

void TitleScene::update(std::chrono::steady_clock::time_point curTime)
{
}

void TitleScene::pause()
{
    for (auto& button : buttons) button.deactivate();
    buttonGroup.deactivate();
}

void TitleScene::resume()
{
    for (auto& button : buttons) button.activate();
    buttonGroup.activate();
}

void TitleScene::render(Renderer &renderer)
{
    renderer.pushDrawable(background, {}, 0);
    for (auto& button : buttons) button.render(renderer);
    
    renderer.pushTransform();
    if (rtl)
    {
        renderer.currentTransform.translate(ScreenWidth, 0);
        renderer.currentTransform.scale(-1, 1);
    }
    renderer.pushDrawable(foreground, {}, 20);
    renderer.popTransform();
    
    pointer.render(renderer);
}
