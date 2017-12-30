#include "TitleScene.hpp"

#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIButtonCommons.hpp"
#include "defaults.hpp"
#include "scene/SceneManager.hpp"

#include "GameScene.hpp"

constexpr float ButtonHeight = 40;
constexpr float ButtonSpace = 10;
constexpr float SetBottomSpace = 24;
constexpr size_t ButtonCaptionSize = 32;

const LangID ButtonIdentifiers[] =
{
    "title-button-new-game",
    "title-button-load-game",
    "title-button-settings",
    "title-button-exit",
};

TitleScene::TitleScene(const Settings& settings, InputManager& inputManager, ResourceManager& rm, LocalizationManager& lm)
    : background(rm.load<sf::Texture>("title-background.png")), foreground(rm.load<sf::Texture>("title-foreground.png")),
    pointer(inputManager, rm)
{
    background.setAnchorPoint(sf::Vector2f(0, 0));
    foreground.setAnchorPoint(sf::Vector2f(0, 0));
    
    int k = 0;
    for (auto& button : buttons)
    {
        button.initialize(inputManager);
        
        createCommonTextualButton(button, rm, lm, "ui-select-field.png", "ui-select-field.png",
            sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, ScreenWidth - 2 * ButtonSpace, ButtonHeight),
            ButtonIdentifiers[k], ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black,
            TextDrawable::HorAnchor::Left, TextDrawable::VertAnchor::Center);
        
        button.getPressedSprite()->setBlendColor(sf::Color::Yellow);
        button.getActiveSprite()->setOpacity(0.5);
        button.getActiveSprite()->setOpacity(0.5);
        button.setPosition(sf::Vector2f(ButtonSpace,
            ScreenHeight - SetBottomSpace + ButtonSpace - (4-k) * (ButtonHeight + ButtonSpace)));
        button.setCaptionDisplacement(sf::Vector2f(24, 0));
        button.setDepth(10);
        
        k++;
    }
    
    buttons[0].setPressAction([&,this]
    {
        auto scene = new GameScene(settings, inputManager, rm, lm);
        scene->loadLevel("level1.lvl");
        getSceneManager().pushScene(scene);
    });
    
    buttons[3].setPressAction([&,this]
    {
        getSceneManager().popScene();
    });
}

void TitleScene::update(std::chrono::steady_clock::time_point curTime)
{
}

void TitleScene::render(Renderer &renderer)
{
    renderer.pushDrawable(background, {}, 0);
    for (auto& button : buttons) button.render(renderer);
    renderer.pushDrawable(foreground, {}, 20);
    pointer.render(renderer);
}
