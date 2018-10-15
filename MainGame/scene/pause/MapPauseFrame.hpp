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

#include "Services.hpp"
#include "drawables/Sprite.hpp"
#include "drawables/GUIMap.hpp"

#include "input/InputManager.hpp"
#include "input/InputPlayerController.hpp"
#include "data/LevelData.hpp"

struct Settings;
class ResourceManager;
class InputManager;
class LocalizationManager;

class MapPauseFrame : public PauseFrame
{
    Sprite mapFrame;
    GUIMap map;
    
    InputManager::CallbackEntry mouseClickEntry;
    InputManager::MouseMoveEntry mouseMoveEntry;
    InputPlayerController mapController;
    
    glm::ivec2 initialPosition;
    bool active;
    enum { Outside, Inside, Clicked } state;
    
public:
    MapPauseFrame(Services& services,
        const std::shared_ptr<LevelData>& levelData = nullptr, size_t curRoom = 0, glm::vec2 pos = {},
        const std::vector<bool>& visibleMaps = {});
    virtual ~MapPauseFrame() {}
    
    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer &renderer) override;
    
    virtual void activate() override;
    virtual void deactivate() override;
    
    void setLevelData(const std::shared_ptr<LevelData>& levelData, size_t curRoom, glm::vec2 pos,
        const std::vector<bool>& visibleMaps);
};
