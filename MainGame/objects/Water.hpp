#pragma once

#include "GameObject.hpp"
#include "Player.hpp"

#include <SFML/Graphics.hpp>
#include <chipmunk/chipmunk.h>
#include <chrono>

class GameScene;
class Renderer;

class Water final : public GameObject
{
    cpBB rect;
    sf::RectangleShape shape;

    cpFloat oldArea;

public:
    Water(GameScene& scene);
    virtual ~Water();

    void setRect(sf::IntRect rect);

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer& renderer) override;
    
#pragma pack(push, 1)
    struct ConfigStruct
    {
        sf::Vector2<int16_t> position;
        int16_t width, height;
    };

    static_assert(sizeof(ConfigStruct) == 8*sizeof(char), "Packing failed!");
#pragma pack(pop)

    bool configure(const ConfigStruct& config);
};
