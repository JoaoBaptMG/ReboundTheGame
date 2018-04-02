#include "MidLevelScene.hpp"

#include "ui/UIButtonCommons.hpp"
#include "scene/SceneManager.hpp"
#include "scene/GameScene.hpp"
#include "scene/FileSelectScene.hpp"
#include "scene/TitleScene.hpp"
#include "language/LangID.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"
#include "resources/ResourceManager.hpp"
#include "input/InputManager.hpp"
#include "rendering/Renderer.hpp"
#include "defaults.hpp"

using namespace std::literals::chrono_literals;

constexpr float ButtonHeight = 40;
constexpr float ButtonSpace = 10;
constexpr float SetBottomSpace = 24;
constexpr size_t ButtonCaptionSize = 32;
constexpr size_t TitleCaptionSize = 80;
constexpr float ButtonTop = ScreenHeight - SetBottomSpace + ButtonSpace;
constexpr float TitleSpace = 32;

const LangID ButtonIdentifiers[] =
{
    "mid-level-continue",
    "mid-level-save-game",
    "mid-level-exit",
};

MidLevelScene::MidLevelScene(Settings& settings, const SavedGame& sg, InputManager& im, ResourceManager& rm,
    LocalizationManager &lm, std::string nextLevel, bool gameover) : nextLevel(nextLevel),
    title(rm.load<FontHandler>(lm.getFontName())), pointer(im, rm), buttonGroup(im, settings.inputSettings),
    sceneFrame(rm.load<sf::Texture>("mid-level-scene-frame.png"), sf::Vector2f(0, 0))
{
    title.setString(lm.getString(gameover ? LangID("mid-level-gameover") : LangID("mid-level-advance")));
    title.setFontSize(TitleCaptionSize);
    title.setDefaultColor(sf::Color::Yellow);
    title.setOutlineThickness(2);
    title.setDefaultOutlineColor(sf::Color::Black);
    title.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    title.setVerticalAnchor(TextDrawable::VertAnchor::Top);
    title.setWordWrappingWidth(ScreenWidth - 2 * TitleSpace);
    title.setWordAlignment(TextDrawable::Alignment::Center);
    configTextDrawable(title, lm);
    title.buildGeometry();
    
    size_t k = 0;
    for (auto& button : buttons)
    {
        button.initialize(im);
        
        createCommonTextualButton(button, rm, lm, "ui-select-field.png", "ui-select-field.png",
            sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, ScreenWidth - 2 * ButtonSpace, ButtonHeight),
            ButtonIdentifiers[k], ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0));
        
        button.getPressedSprite()->setBlendColor(sf::Color::Yellow);
        button.getActiveSprite()->setOpacity(0.5);
        button.getActiveSprite()->setOpacity(0.5);
        button.setPosition(sf::Vector2f(ScreenWidth/2, ButtonTop + ButtonHeight/2 - (3-k) * (ButtonHeight + ButtonSpace)));
        button.setDepth(20);
        
        k++;
    }
    
    buttons[0].setPressAction([&,sg,nextLevel,this]
    {
        if (this != getSceneManager().currentScene()) return;
        
        auto scene = new GameScene(settings, sg, im, rm, lm);
        scene->loadLevel(nextLevel);
        getSceneManager().replaceSceneTransition(scene, 1s);
    });
    
    buttons[1].setPressAction([&,sg,this]
    {
        if (this != getSceneManager().currentScene()) return;
        
        auto scene = new FileSelectScene(settings, sg, im, rm, lm, FileSelectScene::FileAction::Save);
        getSceneManager().pushSceneTransition(scene, 1s);
    });
    
    buttons[2].setPressAction([&,this]
    {
        if (this != getSceneManager().currentScene()) return;
        
        auto scene = new TitleScene(settings, im, rm, lm);
        getSceneManager().replaceSceneTransition(scene, 1s);
    });
    
    buttonGroup.setButtons(buttons);
    buttonGroup.setPointer(pointer);
}

void MidLevelScene::update(FrameTime curTime)
{
    
}

void MidLevelScene::render(Renderer &renderer)
{
    renderer.pushDrawable(sceneFrame, {}, 0);
    
    renderer.pushTransform();
    renderer.currentTransform.translate(ScreenWidth/2, TitleSpace);
    renderer.pushDrawable(title, {}, 10);
    renderer.popTransform();
    
    for (auto& button : buttons) button.render(renderer);
    pointer.render(renderer);
}
