#include "FileSelectScene.hpp"

#include "ui/UIButtonCommons.hpp"
#include "scene/SceneManager.hpp"
#include "scene/GameScene.hpp"
#include "language/LangID.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"
#include "resources/ResourceManager.hpp"
#include "input/InputManager.hpp"
#include "rendering/Renderer.hpp"
#include "defaults.hpp"
#include "gameplay/SavedGame.hpp"
#include "streams/FileOutputStream.hpp"
#include "execDir.hpp"

#include <cmath>

using namespace std::literals::chrono_literals;

constexpr float TextSize = 32;
constexpr float ButtonSize = 576;
constexpr float ButtonBorder = 4;
constexpr float ButtonSpace = 8;
constexpr float OffsetSpeed = 4;

bool loadSaveFromFile(SavedGame& sg, std::string file, SavedGame::Key key)
{
    sf::FileInputStream stream;
    auto fullName = getExecutableDirectory() + '/' + file;
    if (!stream.open(fullName)) return false;
    return readEncryptedSaveFile(stream, sg, key);
}

bool saveSaveToFile(const SavedGame& sg, std::string file, SavedGame::Key& key)
{
    FileOutputStream stream;
    auto fullName = getExecutableDirectory() + '/' + file;
    if (!stream.open(fullName)) return false;
    return writeEncryptedSaveFile(stream, sg, key);
}

std::string getNextFileSlot()
{
    auto files = getAllFilesInDir(getExecutableDirectory());
    
    size_t i = 0;
    while (std::find(files.begin(), files.end(), "save" + std::to_string(i) + ".sav") != files.end()) i++;
    
    return "save" + std::to_string(i) + ".sav";
}

FileSelectScene::FileSelectScene(Settings& settings, const SavedGame& savedGame, InputManager& im, ResourceManager& rm,
    LocalizationManager& lm, FileAction action)
    : sceneFrame(rm.load<sf::Texture>("mid-level-scene-frame.png"), sf::Vector2f(0, 0)),
    pointer(im, rm), buttonGroup(im, settings.inputSettings), cancelButton(im, 8),
    headerBackground(rm.load<sf::Texture>("ui-file-button-frame.png")),
    headerLabel(rm.load<FontHandler>(lm.getFontName()))
{
    sceneFrame.setBlendColor(sf::Color(128, 128, 128, 255));
    
    size_t k = 0;
    for (auto& pair : settings.savedKeys)
    {
        SavedGame curSg;
        if (!loadSaveFromFile(curSg, pair.name, pair.key)) continue;
        
        fileButtons.emplace_back(std::make_unique<UIFileSelectButton>(curSg, im, rm, lm, k));
        fileButtons[k]->setPosition(sf::Vector2f(ScreenWidth/2, k*128 + 128));
        fileButtons[k]->setDepth(60);
        
        if (action == FileAction::Save)
        {
            fileButtons[k]->setPressAction([&, savedGame, k, file = pair.name, this]
            {
                if (this != getSceneManager().currentScene()) return;
                SavedGame::Key key;
                if (saveSaveToFile(savedGame, file, key))
                {
                    settings.savedKeys[k].key = key;
                    substituteButton = std::move(fileButtons[k]);
                    fileButtons[k] = std::make_unique<UIFileSelectButton>(savedGame, im, rm, lm, k);
                    fileButtons[k]->setPosition(sf::Vector2f(ScreenWidth/2, k*128 + 128));
                    fileButtons[k]->setDepth(60);
                    getSceneManager().popSceneTransition(1s);
                }
            });
        }
        else
        {
            fileButtons[k]->setPressAction([&, curSg, k, pair, this]
            {
                if (this != getSceneManager().currentScene()) return;
                
                auto scene = new GameScene(settings, curSg, im, rm, lm);
                scene->loadLevel("level" + std::to_string(curSg.getCurLevel()) + ".lvl");
                getSceneManager().replaceSceneTransition(scene, 2, 1s);
            });
        }
        
        fileButtons[k]->setOverAction([=] { positionButton(k); });
        k++;
    }
    
    if (action == FileAction::Save)
    {
        dummyButton = std::make_unique<UIButton>(im);
        
        createCommonTextualButton(*dummyButton, rm, lm, "ui-file-button-frame-active.png",
            "ui-file-button-frame-pressed.png", sf::FloatRect(4, 4, 4, 4),
            sf::FloatRect(0, 0, ButtonSize, 128), "file-select-new-file",
            TextSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(0, 0),
            TextDrawable::Alignment::Center);
        
        auto normalSprite = std::make_unique<SegmentedSprite>(rm.load<sf::Texture>("ui-file-button-frame.png"));
        normalSprite->setCenterRect(sf::FloatRect(4, 4, 4, 4));
        normalSprite->setDestinationRect(sf::FloatRect(0, 0, ButtonSize, 128));
        normalSprite->setAnchorPoint(sf::Vector2f(ButtonSize/2, 64));
        
        dummyButton->setNormalSprite(std::move(normalSprite));
        dummyButton->autoComputeBounds();
        
        dummyButton->setPosition(sf::Vector2f(ScreenWidth/2, k*128 + 128));
        dummyButton->setDepth(60);
        
        dummyButton->setPressAction([&, savedGame, k, this]
        {
            if (this != getSceneManager().currentScene()) return;
            auto file = getNextFileSlot();
            
            SavedGame::Key key;
            if (saveSaveToFile(savedGame, file, key))
            {
                settings.savedKeys.emplace_back(file, key);
                substituteButton = std::move(dummyButton);
                dummyButton = std::make_unique<UIFileSelectButton>(savedGame, im, rm, lm, k);
                dummyButton->setPosition(sf::Vector2f(ScreenWidth/2, k*128 + 128));
                dummyButton->setDepth(60);
                getSceneManager().popSceneTransition(1s);
            }
        });
        
        dummyButton->setOverAction([=] { positionButton(k); });
    }
    else if (fileButtons.empty())
    {
        dummyButton = std::make_unique<UIButton>();
        
        auto normalSprite = std::make_unique<SegmentedSprite>(rm.load<sf::Texture>("ui-file-button-frame.png"));
        normalSprite->setCenterRect(sf::FloatRect(4, 4, 4, 4));
        normalSprite->setDestinationRect(sf::FloatRect(0, 0, ButtonSize, 128));
        normalSprite->setAnchorPoint(sf::Vector2f(ButtonSize/2, 64));
        
        auto caption = std::make_unique<TextDrawable>(rm.load<FontHandler>(lm.getFontName()));
        caption->setString(lm.getString("file-select-no-files"));
        caption->setFontSize(TextSize);
        caption->setDefaultColor(sf::Color::White);
        caption->setOutlineThickness(1);
        caption->setDefaultOutlineColor(sf::Color::Black);
        caption->setHorizontalAnchor(TextDrawable::HorAnchor::Center);
        caption->setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(*caption, lm);
        caption->buildGeometry();
        
        dummyButton->setCaption(std::move(caption));
        dummyButton->setNormalSprite(std::move(normalSprite));
        dummyButton->autoComputeBounds();
        
        dummyButton->setPosition(sf::Vector2f(ScreenWidth/2, k*128 + 128));
        dummyButton->setDepth(60);
        
        dummyButton->deactivate();
    }
    
    {
        createCommonTextualButton(cancelButton, rm, lm, "ui-file-button-frame-active.png",
            "ui-file-button-frame-pressed.png", sf::FloatRect(4, 4, 4, 4),
            sf::FloatRect(0, 0, ButtonSize, 64), "file-select-cancel",
            TextSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(0, 0),
            TextDrawable::Alignment::Center);
        
        auto normalSprite = std::make_unique<SegmentedSprite>(rm.load<sf::Texture>("ui-file-button-frame.png"));
        normalSprite->setCenterRect(sf::FloatRect(4, 4, 4, 4));
        normalSprite->setDestinationRect(sf::FloatRect(0, 0, ButtonSize, 64));
        normalSprite->setAnchorPoint(sf::Vector2f(ButtonSize/2, 32));
        
        cancelButton.setNormalSprite(std::move(normalSprite));
        cancelButton.autoComputeBounds();
        
        cancelButton.setPosition(sf::Vector2f(ScreenWidth/2, ScreenHeight - 32));
        cancelButton.setDepth(324);
        
        cancelButton.setPressAction([&,this]
        {
            if (this != getSceneManager().currentScene()) return;
            getSceneManager().popSceneTransition(1s);
        });
    }
        
    headerBackground.setCenterRect(sf::FloatRect(4, 4, 4, 4));
    headerBackground.setDestinationRect(sf::FloatRect(0, 0, ButtonSize, 64));
    headerBackground.setAnchorPoint(sf::Vector2f(ButtonSize/2, 0));
    
    auto title = action == FileAction::Load ? LangID("file-select-load") : LangID("file-select-save");
    headerLabel.setString(lm.getString(title));
    headerLabel.setFontSize(TextSize);
    headerLabel.setDefaultColor(sf::Color::Yellow);
    headerLabel.setOutlineThickness(1);
    headerLabel.setDefaultOutlineColor(sf::Color::Black);
    headerLabel.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    headerLabel.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(headerLabel, lm);
    headerLabel.buildGeometry();
    
    std::vector<UIButton*> buttons;
    for (auto& button : fileButtons)
        buttons.push_back(button.get());
    if (action == FileAction::Save) buttons.push_back(dummyButton.get());
    buttons.push_back(&cancelButton);
    
    buttonGroup.setButtons(buttons);
    buttonGroup.setPointer(pointer);
    
    if (getScrollSize() > ScreenHeight - 128)
    {
        scrollBar = std::make_unique<UIScrollBar>(im, rm, getScrollSize(), ScreenHeight - 128);
        scrollBar->setDepth(180);
        scrollBar->setPosition(sf::Vector2f((ScreenWidth + ButtonSize)/2, 64));
    }
}

size_t FileSelectScene::getScrollSize() const
{
    float maxOffset = 128 * fileButtons.size();
    if (dummyButton) maxOffset += 128;
    return maxOffset;
}

void FileSelectScene::update(FrameTime curTime)
{
    if (!scrollBar) return;
    
    size_t k = 0;
    for (auto& button : fileButtons)
    {
        auto pos = button->getPosition();
        pos.y = 128 + 128*k - scrollBar->getCurrentOffset();
        button->setPosition(pos);
        k++;
    }
    
    if (dummyButton)
    {
        auto pos = dummyButton->getPosition();
        pos.y = 128 + 128*k - scrollBar->getCurrentOffset();
        dummyButton->setPosition(pos);
    }
}

void FileSelectScene::positionButton(size_t k)
{
    if (!scrollBar) return;
    if (!std::isnan(pointer.getPosition().y)) return;
    
    if (scrollBar->getCurrentOffset() > k*128)
        scrollBar->setCurrentOffset(k*128);
    else if (scrollBar->getCurrentOffset() < (k+1)*128 - float(ScreenHeight - 128))
        scrollBar->setCurrentOffset((k+1)*128 - float(ScreenHeight - 128));
}

void FileSelectScene::render(Renderer& renderer)
{
    renderer.pushDrawable(sceneFrame, {}, 0);
    
    for (auto& button : fileButtons) button->render(renderer);
    if (dummyButton) dummyButton->render(renderer);
    cancelButton.render(renderer);
    
    pointer.render(renderer);
    if (scrollBar) scrollBar->render(renderer);
    
    renderer.pushTransform();
    renderer.currentTransform.translate(ScreenWidth/2, 0);
    renderer.pushDrawable(headerBackground, {}, 288);
    renderer.currentTransform.translate(0, 32);
    renderer.pushDrawable(headerLabel, {}, 289);
    renderer.popTransform();
}
