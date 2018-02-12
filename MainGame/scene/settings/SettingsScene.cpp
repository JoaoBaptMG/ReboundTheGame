#include "SettingsScene.hpp"

#include "scene/SceneManager.hpp"
#include "scene/TitleScene.hpp"

#include "resources/ResourceManager.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"

#include "scene/pause/PauseScene.hpp"
#include "rendering/Renderer.hpp"

#include "ui/UIButtonCommons.hpp"

#include <defaults.hpp>

constexpr float FrameWidth = PlayfieldWidth;
constexpr float ButtonHeight = 36;
constexpr float ButtonSpace = 8;
constexpr float ButtonCaptionSize = 28;
constexpr float TotalHeight = 11 * ButtonHeight + 10 * ButtonSpace;

const LangID Titles[] =
{
    "settings-video-settings",
    "settings-audio-settings",
    "settings-input-settings",
};

const LangID VideoIdentifiers[] =
{
    "settings-fullscreen",
    "settings-vsync",
    "settings-change-language",
};

const LangID SoundIdentifiers[] =
{
    "settings-music-volume",
    "settings-sound-volume",
};


const LangID InputIdentifiers[] =
{
    "settings-keyboard-controls",
    "settings-joystick-controls",
};

extern bool GlobalUpdateWindowHandler;

SettingsBase::SettingsBase(Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager &lm,
    sf::Vector2f centerPos, UIPointer& pointer, LangID backId) : centerPosition(centerPos), 
    fullscreen(settings.videoSettings.fullscreen), vsync(settings.videoSettings.vsyncEnabled),
    musicVolume(settings.audioSettings.musicVolume), soundVolume(settings.audioSettings.soundVolume),
    fullscreenSwitch(im, settings.inputSettings, &fullscreen), vsyncSwitch(im, settings.inputSettings, &vsync),
    languageButton(im), musicSlider(im, rm, settings.inputSettings, lm.isRTL(), &musicVolume, 100),
    soundSlider(im, rm, settings.inputSettings, lm.isRTL(), &soundVolume, 100),
    keyboardButton(im), joystickButton(im), backButton(im), buttonGroup(im, settings.inputSettings)
{
    auto pos = centerPos + sf::Vector2f(0, -TotalHeight/2 + ButtonHeight/2);
    
    size_t k = 0;
    for (auto& title : titles)
    {
        title.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
        title.setString(lm.getString(Titles[k]));
        title.setFontSize(ButtonCaptionSize);
        title.setDefaultColor(sf::Color::Yellow);
        title.setOutlineThickness(1);
        title.setDefaultOutlineColor(sf::Color::Black);
        title.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
        title.setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(title, lm);
        title.buildGeometry();
        k++;
    }
    
    k = 0;
    for (auto button : { &fullscreenSwitch, &vsyncSwitch })
    {
        createCommonTextSwitch(*button, rm, lm, "ui-select-field.png", "ui-select-field.png",
            sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
            VideoIdentifiers[k], ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0));
        button->setPosition(pos + sf::Vector2f(0, (k+1) * (ButtonHeight + ButtonSpace)));
        k++;
    }
    
    createCommonTextualButton(languageButton, rm, lm, "ui-select-field.png", "ui-select-field.png",
        sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
        VideoIdentifiers[2], ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0));
    languageButton.setPosition(pos + sf::Vector2f(0, 3 * (ButtonHeight + ButtonSpace)));
    
    k = 0;
    for (auto button : { &musicSlider, &soundSlider })
    {
        createCommonTextualButton(*button, rm, lm, "ui-select-field.png", "ui-select-field.png",
            sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
            SoundIdentifiers[k], ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0));
        button->setPosition(pos + sf::Vector2f(0, (k+5) * (ButtonHeight + ButtonSpace)));
        k++;
    }
    
    k = 0;
    for (auto button : { &keyboardButton, &joystickButton })
    {
        createCommonTextualButton(*button, rm, lm, "ui-select-field.png", "ui-select-field.png",
            sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
            InputIdentifiers[k], ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0));
        button->setPosition(pos + sf::Vector2f(0, (k+8) * (ButtonHeight + ButtonSpace)));
        k++;
    }
    
    createCommonTextualButton(backButton, rm, lm, "ui-select-field.png", "ui-select-field.png",
        sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
        backId, ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0),
        TextDrawable::Alignment::Center);
    backButton.setPosition(pos + sf::Vector2f(0, 10 * (ButtonHeight + ButtonSpace)));
    
    for (auto button : std::initializer_list<UIButton*>{ &fullscreenSwitch, &vsyncSwitch, &languageButton, &musicSlider,
        &soundSlider, &keyboardButton, &joystickButton, &backButton })
    {
        button->getPressedSprite()->setBlendColor(sf::Color::Yellow);
        button->getActiveSprite()->setOpacity(0.5);
        button->getActiveSprite()->setOpacity(0.5);
        button->setDepth(3200);
    }
    
    fullscreenSwitch.setSwitchAction([&] (bool val)
    {
        settings.videoSettings.fullscreen = val;
        GlobalUpdateWindowHandler = true;
    });
    
    vsyncSwitch.setSwitchAction([&] (bool val)
    {
        settings.videoSettings.vsyncEnabled = val;
        GlobalUpdateWindowHandler = true;
    });
    
    musicSlider.setSlideAction([&] (size_t val)
    {
        settings.audioSettings.musicVolume = val;
    });
    
    soundSlider.setSlideAction([&] (size_t val)
    {
        settings.audioSettings.soundVolume = val;
    });
    
    backButton.setPressAction([&,this] { if (backAction) backAction(); });
    
    buttonGroup.setButtons({ fullscreenSwitch, vsyncSwitch, languageButton, musicSlider, soundSlider, keyboardButton,
        joystickButton, backButton });
    buttonGroup.setPointer(pointer);
}

void SettingsBase::update(std::chrono::steady_clock::time_point curTime)
{
    musicSlider.update();
    soundSlider.update();
}

void SettingsBase::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(centerPosition);
    renderer.currentTransform.translate(0, -TotalHeight/2 + ButtonHeight/2);
    int k = 0;
    for (auto& title : titles)
    {
        renderer.pushDrawable(title, {}, 3200);
        renderer.currentTransform.translate(0, (4-(k++)) * (ButtonHeight + ButtonSpace));
    }
    renderer.popTransform();
    
    fullscreenSwitch.render(renderer);
    vsyncSwitch.render(renderer);
    languageButton.render(renderer);
    
    musicSlider.render(renderer);
    soundSlider.render(renderer);
    
    keyboardButton.render(renderer);
    joystickButton.render(renderer);
    backButton.render(renderer);
}

void SettingsBase::activate()
{
    fullscreenSwitch.activate();
    vsyncSwitch.activate();
    
    musicSlider.activate();
    soundSlider.activate();
    
    keyboardButton.activate();
    joystickButton.activate();
    backButton.activate();
    
    buttonGroup.activate();
}

void SettingsBase::deactivate()
{
    fullscreenSwitch.deactivate();
    vsyncSwitch.deactivate();
    
    musicSlider.deactivate();
    soundSlider.deactivate();
    
    keyboardButton.deactivate();
    joystickButton.deactivate();
    backButton.deactivate();
    
    buttonGroup.deactivate();
}

SettingsScene::SettingsScene(Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager &lm)
    : SettingsBase(settings, im, rm, lm, sf::Vector2f(ScreenWidth, ScreenHeight)/2.0f, pointer, "settings-scene-back"),
    sceneFrame(rm.load<sf::Texture>("settings-scene-frame.png")), pointer(im, rm)
{
    using namespace std::literals::chrono_literals;
    backAction = [&,this]
    {
        auto scene = new TitleScene(settings, im, rm, lm);
        getSceneManager().replaceSceneTransition(scene, 1s);
    };
}

void SettingsScene::render(Renderer& renderer)
{
    SettingsBase::render(renderer);
    
    renderer.pushTransform();
    renderer.currentTransform.translate(ScreenWidth/2, ScreenHeight/2);
    renderer.pushDrawable(sceneFrame, {}, 3000);
    renderer.popTransform();
    
    pointer.render(renderer);
}

SettingsPauseFrame::SettingsPauseFrame(Settings& settings, InputManager& im, ResourceManager& rm,
    LocalizationManager& lm, UIPointer& pointer, PauseScene* scene) : SettingsBase(settings, im, rm, lm,
    sf::Vector2f(ScreenWidth/2, ScreenHeight/2 + 32), pointer, "settings-pause-resume")
{
    backAction = [=] { scene->unpause(); };
}

void SettingsPauseFrame::render(Renderer& renderer)
{
    renderer.currentTransform.translate(0, -64);
    SettingsBase::render(renderer);
}
