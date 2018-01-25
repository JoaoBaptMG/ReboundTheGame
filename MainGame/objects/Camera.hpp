#pragma once

#include <chrono>
#include <SFML/System.hpp>

class GameScene;

class Camera final
{
public:
    enum class Direction { Right, Down, Left, Up };

private:
    sf::Vector2f position;
    GameScene& gameScene;
    std::chrono::steady_clock::time_point curTime, transitionTime;
    Direction transitionDirection;
    
public:
    Camera(GameScene& scene) : position(), gameScene(scene) {}
    ~Camera() {}
    
    void scheduleTransition(Direction direction);
    bool transitionOccuring() const;
    
    void update(std::chrono::steady_clock::time_point curTime);
    sf::Vector2f getGlobalDisplacement() const;
};
