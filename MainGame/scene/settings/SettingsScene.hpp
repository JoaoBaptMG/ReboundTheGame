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
#include "scene/pause/PauseFrame.hpp"
#include "SettingsPanel.hpp"

#include "drawables/Sprite.hpp"
#include "drawables/TextDrawable.hpp"

#include "input/CommonActions.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIButtonGroup.hpp"
#include "ui/UIPointer.hpp"
#include "ui/UITextSwitch.hpp"
#include "ui/UISlider.hpp"

#include "language/LangID.hpp"

#include "Services.hpp"

class PauseScene;

class SettingsBase
{
    glm::vec2 centerPosition;
    std::unique_ptr<SettingsPanel> curSettingsPanel;
    SettingsPanel* nextSettingsPanel;
    LangID backId;
    
protected:
    SettingsBase(Services& services, glm::vec2 centerPos, UIPointer& pointer, LangID backId);
    ~SettingsBase() = default;
    
    void update(FrameTime curTime);
    void render(Renderer& renderer);
    
    void activate();
    void deactivate();

    std::function<void()> backAction;

public:
    void changeSettingsPanel(SettingsPanel* panel);

    friend class SettingsPanel;
};

class SettingsScene : public Scene, public SettingsBase
{
    Sprite sceneFrame;
    UIPointer pointer;
    
public:
    SettingsScene(Services& services);
    virtual ~SettingsScene() {}
    
    virtual void update(FrameTime curTime) override { SettingsBase::update(curTime); }
    virtual void render(Renderer& renderer) override;
};

class SettingsPauseFrame : public SettingsBase, public PauseFrame
{
public:
    SettingsPauseFrame(Services& services, UIPointer& pointer, PauseScene* scene);
    virtual ~SettingsPauseFrame() {}
    
    virtual void update(FrameTime curTime) override { SettingsBase::update(curTime); }
    virtual void render(Renderer &renderer) override;
    
    virtual void activate() override { SettingsBase::activate(); }
    virtual void deactivate() override { SettingsBase::deactivate(); }
};
