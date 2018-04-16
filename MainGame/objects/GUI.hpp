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

#include "drawables/Sprite.hpp"
#include "drawables/GUIMeter.hpp"
#include "drawables/TextDrawable.hpp"
#include "objects/Player.hpp"
#include <chronoUtils.hpp>
#include "drawables/GUIMap.hpp"

#include "language/LocalizationManager.hpp"

class GameScene;
class Renderer;

class GUIBossUpdater
{
public:
    virtual size_t getCurrentHealth() const = 0;
    virtual size_t getMaxHealth() const = 0;
};

GUIBossUpdater* nullBossMeter();

class GUI final
{
    GameScene& gameScene;
    
    Sprite guiLeft, guiRight;
    GUIMeter playerMeter, dashMeter, bossMeter;
    Sprite bombSprites[MaxBombs];
    TextDrawable levelLabel, levelID;
    size_t levelNumber;
    bool drawDash;

    const GUIBossUpdater* currentBoss;
    GUIMap guiMap;
    std::string lastIconName;
    
    void configureText();
    
    FrameTime lastTime;
    float healthBlinkPhase;

    LocalizationManager::CallbackEntry callbackEntry;
    
public:
    GUI(GameScene& scene);
    ~GUI() {}

    void update(FrameTime curTime);
    void render(Renderer& renderer);

    void setLevelNumber(size_t number);
    void setVisibleMaps(const std::vector<bool>& visibleMaps);
    void resetBlinker();
    
    auto getCurrentBoss() const { return currentBoss; }
    void setCurrentBoss(const GUIBossUpdater* boss) { currentBoss = boss; }
};
