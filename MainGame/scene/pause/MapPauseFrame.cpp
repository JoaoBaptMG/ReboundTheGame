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


#include "MapPauseFrame.hpp"

#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Texture.hpp"
#include "settings/Settings.hpp"

#include <defaults.hpp>

const glm::vec2 GlobalPosition(ScreenWidth/2, ScreenHeight/2);
const float ControllerMovementSpeed = 2;

MapPauseFrame::MapPauseFrame(Services& services, const std::shared_ptr<LevelData>& levelData,
    size_t curRoom, glm::vec2 pos, const std::vector<bool>& visibleMaps)
    : mapFrame(services.resourceManager.load<Texture>("pause-map-frame.png")),
    mapController(services.inputManager, services.settings.inputSettings),
    map(true), active(true), state(Outside)
{
    setLevelData(levelData, curRoom, pos, visibleMaps);
    
    mouseMoveEntry = services.inputManager.registerMouseMoveCallback([=] (glm::ivec2 position)
    {
        if (!active) return;
        
        if (state == Outside && map.getBounds().contains(glm::vec2(position) - GlobalPosition)) state = Inside;
        else if (state != Outside && !map.getBounds().contains(glm::vec2(position) - GlobalPosition))
            state = Outside;
        
        if (state == Inside) initialPosition = position;
        else if (state == Clicked)
        {
            map.setDisplayPosition(map.getDisplayPosition() - glm::vec2(position - initialPosition));
            initialPosition = position;
        }
    });
    
    mouseClickEntry = services.inputManager.registerCallback(InputSource::mouseButton(sf::Mouse::Button::Left),
    [=] (InputSource, float val)
    {
        if (!active) return;
        if (val > 0.5 && state == Inside) state = Clicked;
        else if (state == Clicked) state = Inside;
    });
}

void MapPauseFrame::setLevelData(const std::shared_ptr<LevelData>& levelData, size_t curRoom, glm::vec2 pos,
    const std::vector<bool>& visibleMaps)
{
    map.setCurLevel(levelData);
    map.setCurRoom(curRoom);
    map.setDisplayPosition(pos/(float)DefaultTileSize);
    
    for (size_t i = 0; i < visibleMaps.size(); i++)
        if (visibleMaps.at(i)) map.presentRoomFull(i);
        else map.hideRoom(i);
}

void MapPauseFrame::update(FrameTime curTime)
{
    map.update(curTime);
    mapController.update();
    
    if (state != Clicked)
        map.setDisplayPosition(map.getDisplayPosition() + ControllerMovementSpeed * mapController.movement().getValue());
}

void MapPauseFrame::render(Renderer &renderer)
{
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(ScreenWidth/2, ScreenHeight/2 - 64);
    renderer.pushDrawable(mapFrame, 5000);
    renderer.pushDrawable(map, 5002);
    renderer.popTransform();
}

void MapPauseFrame::activate()
{
    active = true;
}

void MapPauseFrame::deactivate()
{
    active = false;
    state = Outside;
}
