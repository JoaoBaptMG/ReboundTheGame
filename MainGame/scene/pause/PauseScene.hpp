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


#pragma once

#include "scene/Scene.hpp"
#include "PauseFrame.hpp"

#include "Services.hpp"
#include "drawables/Sprite.hpp"
#include "input/CommonActions.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIButtonGroup.hpp"
#include "ui/UIPointer.hpp"

#include "language/LocalizationManager.hpp"

struct Settings;
class ResourceManager;
class InputManager;
class LocalizationManager;

class LevelData;

class PauseScene final : public Scene
{
    Services& services;
    
    Sprite backgroundSprite;
    FrameTime curTime, transitionTime;
    float transitionFactor;
    bool unpausing;
    
    ButtonAction<2> quitPause, switchFrameLeft, switchFrameRight;
    UIButton frameButtons[3];
    //UIButtonGroup frameButtonGroup;
    UIPointer pointer;
    
    size_t currentFrame;
    std::unique_ptr<PauseFrame> pauseFrames[3];

    LocalizationManager::CallbackEntry callbackEntry;

public:
    PauseScene(Services& services);
    virtual ~PauseScene() {}
    
    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer &renderer) override;
    
    void setMapLevelData(std::shared_ptr<LevelData> level, size_t curRoom, glm::vec2 pos,
        const std::vector<bool>& visibleMaps);
    void setCollectedFrameSavedGame(const SavedGame& sg);
    void setCollectedFrameHealthData(size_t curHealth, size_t maxHealth);
    void unpause();
    
    void switchPauseFrame(size_t frame);
};

