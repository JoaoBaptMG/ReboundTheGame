#pragma once

#include "PauseFrame.hpp"

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
    
    sf::Vector2i initialPosition;
    bool active;
    enum { Outside, Inside, Clicked } state;
    
public:
    MapPauseFrame(const Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm,
        const std::shared_ptr<LevelData>& levelData = nullptr, size_t curRoom = 0, sf::Vector2f pos = {},
        const std::vector<bool>& visibleMaps = {});
    virtual ~MapPauseFrame() {}
    
    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer &renderer) override;
    
    virtual void activate() override;
    virtual void deactivate() override;
    
    void setLevelData(const std::shared_ptr<LevelData>& levelData, size_t curRoom, sf::Vector2f pos,
        const std::vector<bool>& visibleMaps);
};
