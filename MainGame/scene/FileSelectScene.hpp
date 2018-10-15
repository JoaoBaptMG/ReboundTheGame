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

#include "Scene.hpp"

#include "Services.hpp"
#include "drawables/Sprite.hpp"
#include "drawables/TextDrawable.hpp"
#include "drawables/SegmentedSprite.hpp"
#include "ui/UIFileSelectButton.hpp"
#include "ui/UIButtonGroup.hpp"
#include "ui/UIPointer.hpp"
#include "ui/UIScrollBar.hpp"
#include <vector>
#include <memory>

struct Settings;
struct SavedGame;

class Renderer;

class FileSelectScene : public Scene
{
public:
    enum class FileAction { Load, Save };
    
private:
    Sprite sceneFrame;
    std::vector<std::unique_ptr<UIFileSelectButton>> fileButtons;
    std::unique_ptr<UIButton> dummyButton, substituteButton;
    std::unique_ptr<UIScrollBar> scrollBar;
    UIButton cancelButton;
    UIPointer pointer;
    UIButtonGroup buttonGroup;
    SegmentedSprite headerBackground;
    TextDrawable headerLabel;

public:
    FileSelectScene(Services& services, const SavedGame& savedGame, FileAction action);
    virtual ~FileSelectScene() {}
    
    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer& renderer) override;
    
    size_t getScrollSize() const;
    void positionButton(size_t k);
};
