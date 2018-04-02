#pragma once

#include <chronoUtils.hpp>
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
    FrameTime curTime, transitionTime, shakeTime;
    std::vector<sf::Vector2f> shakeSamples;
    FrameDuration shakePeriod;
    Direction transitionDirection;

    sf::Vector2f getShakeDisplacement() const;

public:
    Camera(GameScene& scene) : position(), gameScene(scene) {}
    ~Camera() {}
    
    void scheduleTransition(Direction direction);
    bool transitionOccuring() const;

    bool isVisible(sf::Vector2f point) const;
    bool isVisible(sf::FloatRect rect) const;
    
    void update(FrameTime curTime);
    sf::Vector2f getGlobalDisplacement() const;

    void applyShake(FrameDuration duration,
        FrameDuration period, float amp);
};
