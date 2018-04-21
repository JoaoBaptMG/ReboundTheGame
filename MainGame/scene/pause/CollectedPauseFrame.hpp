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

#include "PauseFrame.hpp"

#include "drawables/TextDrawable.hpp"
#include "drawables/Sprite.hpp"

#include "Services.hpp"
#include "ui/UIScrollBar.hpp"
#include "input/CommonActions.hpp"

struct SavedGame;

class CollectedPauseFrame : public PauseFrame
{
    UIScrollBar scrollBar;
    AxisAction<1,2> travel;

    TextDrawable titles[3];
    TextDrawable powerupLabels[12];
    TextDrawable levelLabels[10], totalLabel;
    TextDrawable picketCount[10], totalPicketCount;
    
    Sprite powerupSprites[12];
    Sprite goldenTokenSprites[30];
    Sprite picketSprites[11];

    bool showSecretPowerups;

    LocalizationManager& localizationManager;
    FrameTime curTime, initTime;
    
public:
    size_t getTotalHeight() const;

    CollectedPauseFrame(Services& services);
    virtual ~CollectedPauseFrame() {}

    void setHealthData(size_t curHealth, size_t maxHealth);
    void setSavedGame(const SavedGame& savedGame);
    
    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer &renderer) override;
    
    virtual void activate() override;
    virtual void deactivate() override;
};
