#pragma once

#include <chrono>
#include <vector>
#include <SFML/Graphics.hpp>

class GameScene;

class Camera final
{
public:
    enum class Direction { Right, Down, Left, Up };

private:
    sf::Vector2f position;
    GameScene& gameScene;
    std::chrono::steady_clock::time_point curTime, transitionTime, shakeTime;
    std::vector<sf::Vector2f> shakeSamples;
    std::chrono::steady_clock::duration shakePeriod;
    Direction transitionDirection;

    sf::Vector2f getShakeDisplacement() const;

public:
    Camera(GameScene& scene) : position(), gameScene(scene) {}
    ~Camera() {}
    
    void scheduleTransition(Direction direction);
    bool transitionOccuring() const;

    bool isVisible(sf::Vector2f point) const;
    bool isVisible(sf::FloatRect rect) const;
    
    void update(std::chrono::steady_clock::time_point curTime);
    sf::Vector2f getGlobalDisplacement() const;

    void applyShake(std::chrono::steady_clock::duration duration,
        std::chrono::steady_clock::duration period, float amp);
};
