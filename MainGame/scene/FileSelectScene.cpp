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
#include <streams/FileInputStream.hpp>
#include <streams/FileOutputStream.hpp>
#include "execDir.hpp"
#include "ColorList.hpp"

#include "audio/AudioManager.hpp"
#include "audio/Sound.hpp"

#include <cmath>

using namespace std::literals::chrono_literals;

constexpr float TextSize = 32;
constexpr float ButtonSize = 720;
constexpr float ButtonBorder = 4;
constexpr float ButtonSpace = 8;
constexpr float OffsetSpeed = 4;

bool loadSaveFromFile(SavedGame& sg, std::string file, SavedGame::Key key)
{
    FileInputStream stream;
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

FileSelectScene::FileSelectScene(Services& services, const SavedGame& savedGame, FileAction action)
    : sceneFrame(services.resourceManager.load<Texture>("mid-level-scene-frame.png"), glm::vec2(0, 0)),
    pointer(services), buttonGroup(services, TravelingMode::Vertical), cancelButton(services.inputManager, 8),
    headerBackground(services.resourceManager.load<Texture>("ui-file-button-frame.png")),
    headerLabel(loadDefaultFont(services))
{
    sceneFrame.setBlendColor(glm::u8vec4(128, 128, 128, 255));

    auto globalBounds = util::rect((ScreenWidth - ButtonSize)/2, 64, ButtonSize, ScreenHeight - 128);

    size_t k = 0;
    for (auto& pair : services.settings.savedKeys)
    {
        SavedGame curSg;
        if (!loadSaveFromFile(curSg, pair.name, pair.key)) continue;
        
        fileButtons.emplace_back(std::make_unique<UIFileSelectButton>(curSg, services, k));
        fileButtons[k]->setPosition(glm::vec2(ScreenWidth/2, k*128 + 128));
        fileButtons[k]->setDepth(60);
        fileButtons[k]->setGlobalBounds(globalBounds);

        if (action == FileAction::Save)
        {
            fileButtons[k]->setPressAction([&, savedGame, k, file = pair.name, this]
            {
                if (this != getSceneManager().currentScene()) return;

                services.audioManager.playSound(services.resourceManager.load<Sound>("ui-file-select.wav"));
                SavedGame::Key key;
                if (saveSaveToFile(savedGame, file, key))
                {
                    services.settings.savedKeys[k].key = key;
                    substituteButton = std::move(fileButtons[k]);
                    fileButtons[k] = std::make_unique<UIFileSelectButton>(savedGame, services, k);
                    fileButtons[k]->setPosition(glm::vec2(ScreenWidth/2, k*128 + 128));
                    fileButtons[k]->setDepth(60);
                    fileButtons[k]->setGlobalBounds(globalBounds);
                    getSceneManager().popSceneTransition(1s);
                }
            });
        }
        else
        {
            fileButtons[k]->setPressAction([&, curSg, k, pair, this]
            {
                if (this != getSceneManager().currentScene()) return;

                services.audioManager.playSound(services.resourceManager.load<Sound>("ui-file-select.wav"));
                auto scene = new GameScene(services, curSg);
                scene->loadLevel("level" + std::to_string(curSg.getCurLevel()) + ".lvl");
                getSceneManager().replaceSceneTransition(scene, 2, 1s);
            });
        }
        
        fileButtons[k]->setOverAction([=,&services]
        {
            playCursor(services);
            positionButton(k);
        });
        k++;
    }
    
    if (action == FileAction::Save)
    {
        dummyButton = std::make_unique<UIButton>(services.inputManager);
        
        createCommonTextualButton(*dummyButton, services, "ui-file-button-frame-active.png",
            "ui-file-button-frame-pressed.png", util::rect(4, 4, 4, 4),
            util::rect(0, 0, ButtonSize, 128), "file-select-new-file",
            TextSize, Colors::White, 1, Colors::Black, glm::vec2(0, 0),
            TextDrawable::Alignment::Center);
        
        auto normalSprite = std::make_unique<SegmentedSprite>(services.resourceManager.load<Texture>("ui-file-button-frame.png"));
        normalSprite->setCenterRect(util::rect(4, 4, 4, 4));
        normalSprite->setDestinationRect(util::rect(0, 0, ButtonSize, 128));
        normalSprite->setAnchorPoint(glm::vec2(ButtonSize/2, 64));
        
        dummyButton->setNormalSprite(std::move(normalSprite));
        dummyButton->autoComputeBounds();
        
        dummyButton->setPosition(glm::vec2(ScreenWidth/2, k*128 + 128));
        dummyButton->setDepth(60);
        dummyButton->setGlobalBounds(globalBounds);
        
        dummyButton->setPressAction([&, savedGame, k, this]
        {
            if (this != getSceneManager().currentScene()) return;

            services.audioManager.playSound(services.resourceManager.load<Sound>("ui-file-select.wav"));
            auto file = getNextFileSlot();
            
            SavedGame::Key key;
            if (saveSaveToFile(savedGame, file, key))
            {
                services.settings.savedKeys.emplace_back(file, key);
                substituteButton = std::move(dummyButton);
                dummyButton = std::make_unique<UIFileSelectButton>(savedGame, services, k);
                dummyButton->setPosition(glm::vec2(ScreenWidth/2, k*128 + 128));
                dummyButton->setDepth(60);
                dummyButton->setGlobalBounds(globalBounds);
                getSceneManager().popSceneTransition(1s);
            }
        });
        
        dummyButton->setOverAction([=,&services]
        {
            playCursor(services);
            positionButton(k);
        });
    }
    else if (fileButtons.empty())
    {
        dummyButton = std::make_unique<UIButton>();
        
        auto normalSprite = std::make_unique<SegmentedSprite>(services.resourceManager.load<Texture>("ui-file-button-frame.png"));
        normalSprite->setCenterRect(util::rect(4, 4, 4, 4));
        normalSprite->setDestinationRect(util::rect(0, 0, ButtonSize, 128));
        normalSprite->setAnchorPoint(glm::vec2(ButtonSize/2, 64));
        
        auto caption = std::make_unique<TextDrawable>(loadDefaultFont(services));
        caption->setString(services.localizationManager.getString("file-select-no-files"));
        caption->setFontSize(TextSize);
        caption->setDefaultColor(Colors::White);
        caption->setOutlineThickness(1);
        caption->setDefaultOutlineColor(Colors::Black);
        caption->setHorizontalAnchor(TextDrawable::HorAnchor::Center);
        caption->setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(*caption, services.localizationManager);
        caption->buildGeometry();
        
        dummyButton->setCaption(std::move(caption));
        dummyButton->setNormalSprite(std::move(normalSprite));
        dummyButton->autoComputeBounds();
        
        dummyButton->setPosition(glm::vec2(ScreenWidth/2, k*128 + 128));
        dummyButton->setDepth(60);
        dummyButton->setGlobalBounds(globalBounds);
        
        dummyButton->deactivate();
    }
    
    {
        createCommonTextualButton(cancelButton, services, "ui-file-button-frame-active.png",
            "ui-file-button-frame-pressed.png", util::rect(4, 4, 4, 4),
            util::rect(0, 0, ButtonSize, 64), "file-select-cancel",
            TextSize, Colors::White, 1, Colors::Black, glm::vec2(0, 0),
            TextDrawable::Alignment::Center);
        
        auto normalSprite = std::make_unique<SegmentedSprite>(services.resourceManager.load<Texture>("ui-file-button-frame.png"));
        normalSprite->setCenterRect(util::rect(4, 4, 4, 4));
        normalSprite->setDestinationRect(util::rect(0, 0, ButtonSize, 64));
        normalSprite->setAnchorPoint(glm::vec2(ButtonSize/2, 32));
        
        cancelButton.setNormalSprite(std::move(normalSprite));
        cancelButton.autoComputeBounds();
        
        cancelButton.setPosition(glm::vec2(ScreenWidth/2, ScreenHeight - 32));
        cancelButton.setDepth(324);
        
        cancelButton.setPressAction([&,this]
        {
            if (this != getSceneManager().currentScene()) return;

            playConfirm(services);
            getSceneManager().popSceneTransition(1s);
        });
    }
        
    headerBackground.setCenterRect(util::rect(4, 4, 4, 4));
    headerBackground.setDestinationRect(util::rect(0, 0, ButtonSize, 64));
    headerBackground.setAnchorPoint(glm::vec2(ButtonSize/2, 0));
    
    auto title = action == FileAction::Load ? LangID("file-select-load") : LangID("file-select-save");
    headerLabel.setString(services.localizationManager.getString(title));
    headerLabel.setFontSize(TextSize);
    headerLabel.setDefaultColor(Colors::Yellow);
    headerLabel.setOutlineThickness(1);
    headerLabel.setDefaultOutlineColor(Colors::Black);
    headerLabel.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    headerLabel.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(headerLabel, services.localizationManager);
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
        scrollBar = std::make_unique<UIScrollBar>(services, getScrollSize(), ScreenHeight - 128);
        scrollBar->setDepth(180);
        scrollBar->setPosition(glm::vec2((ScreenWidth + ButtonSize)/2, 64));
    }
}

size_t FileSelectScene::getScrollSize() const
{
    float maleftOffset = 128 * fileButtons.size();
    if (dummyButton) maleftOffset += 128;
    return maleftOffset;
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
    renderer.pushDrawable(sceneFrame, 0);
    
    for (auto& button : fileButtons) button->render(renderer);
    if (dummyButton) dummyButton->render(renderer);
    cancelButton.render(renderer);
    
    pointer.render(renderer);
    if (scrollBar) scrollBar->render(renderer);
    
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(ScreenWidth/2, 0);
    renderer.pushDrawable(headerBackground, 288);
    renderer.currentTransform *= util::translate(0, 32);
    renderer.pushDrawable(headerLabel, 289);
    renderer.popTransform();
}
