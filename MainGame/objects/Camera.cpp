#include "Camera.hpp"

#include "scene/GameScene.hpp"
#include <defaults.hpp>
#include <chronoUtils.hpp>
#include <cmath>

constexpr auto TransitionDuration = 20 * UpdateFrequency;

void Camera::update(std::chrono::steady_clock::time_point curTime)
{
    this->curTime = curTime;
    
    auto player = gameScene.getObjectByName<Player>("player");
    if (player)
    {
        position = player->getDisplayPosition();
        
        if (position.x < PlayfieldWidth/2) position.x = PlayfieldWidth/2;
        else if (position.x > (float)DefaultTileSize * gameScene.getRoomWidth() - PlayfieldWidth/2)
            position.x = (float)DefaultTileSize * gameScene.getRoomWidth() - PlayfieldWidth/2;
            
        if (position.y < PlayfieldHeight/2) position.y = PlayfieldHeight/2;
        else if (position.y > (float)DefaultTileSize * gameScene.getRoomHeight() - PlayfieldHeight/2)
            position.y = (float)DefaultTileSize * gameScene.getRoomHeight() - PlayfieldHeight/2;
    }
    
    if (!isNull(transitionTime) && curTime > transitionTime + TransitionDuration)
    {
        reset(transitionTime);
        gameScene.notifyTransitionEnded();
    }
}

void Camera::scheduleTransition(Camera::Direction direction)
{
    transitionDirection = direction;
    if (isNull(transitionTime)) transitionTime = curTime;
    else transitionTime = curTime + (curTime - transitionTime) - TransitionDuration;
}

bool Camera::transitionOccuring() const
{
    return !isNull(transitionTime);
}

sf::Vector2f Camera::getGlobalDisplacement() const
{
    auto disp = position;
    
    if (!isNull(transitionTime))
    {
        float factor = toSeconds<float>(curTime - transitionTime) / toSeconds<float>(TransitionDuration) - 0.5;
        
        switch (transitionDirection)
        {
            case Direction::Right: disp.x = factor * PlayfieldWidth; break;
            case Direction::Down: disp.y = factor * PlayfieldHeight; break;
            case Direction::Left:
                disp.x = floorf((float)DefaultTileSize * gameScene.getRoomWidth() - factor * PlayfieldWidth);
                break;
            case Direction::Up:
                disp.y = floorf((float)DefaultTileSize * gameScene.getRoomHeight() - factor * PlayfieldHeight);
                break;
        }
    }
    
    return sf::Vector2f((float)ScreenWidth, (float)ScreenHeight)/2.0f - disp;
}
