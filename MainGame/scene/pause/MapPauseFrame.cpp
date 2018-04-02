#include "MapPauseFrame.hpp"

#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"
#include "settings/Settings.hpp"

#include <defaults.hpp>

const sf::Vector2f GlobalPosition(ScreenWidth/2, ScreenHeight/2);
const float ControllerMovementSpeed = 2;

MapPauseFrame::MapPauseFrame(const Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm,
    const std::shared_ptr<LevelData>& levelData, size_t curRoom, sf::Vector2f pos, const std::vector<bool>& visibleMaps)
    : mapFrame(rm.load<sf::Texture>("pause-map-frame.png")), map(true), active(true), state(Outside),
    mapController(im, settings.inputSettings)
{
    setLevelData(levelData, curRoom, pos, visibleMaps);
    
    mouseMoveEntry = im.registerMouseMoveCallback([=] (sf::Vector2i position)
    {
        if (!active) return;
        
        if (state == Outside && map.getBounds().contains(sf::Vector2f(position) - GlobalPosition)) state = Inside;
        else if (state != Outside && !map.getBounds().contains(sf::Vector2f(position) - GlobalPosition))
            state = Outside;
        
        if (state == Inside) initialPosition = position;
        else if (state == Clicked)
        {
            map.setDisplayPosition(map.getDisplayPosition() - sf::Vector2f(position - initialPosition));
            initialPosition = position;
        }
    });
    
    mouseClickEntry = im.registerCallback(InputSource::mouseButton(sf::Mouse::Button::Left),
    [=] (InputSource, float val)
    {
        if (!active) return;
        if (val > 0.5 && state == Inside) state = Clicked;
        else if (state == Clicked) state = Inside;
    });
}

void MapPauseFrame::setLevelData(const std::shared_ptr<LevelData>& levelData, size_t curRoom, sf::Vector2f pos,
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
    renderer.currentTransform.translate(ScreenWidth/2, ScreenHeight/2 - 64);
    renderer.pushDrawable(mapFrame, {}, 2800);
    renderer.pushDrawable(map, {}, 2802);
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
