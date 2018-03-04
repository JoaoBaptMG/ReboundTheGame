#pragma once

#include "GameObject.hpp"

#include "drawables/SegmentedSprite.hpp"
#include "drawables/TextDrawable.hpp"

#include <memory>
#include <chrono>
#include <SFML/System.hpp>

class GameScene;
class Renderer;

class InteractionPopup final : public GameObject
{
    SegmentedSprite popupBox;
    TextDrawable popupLabel;

    std::chrono::steady_clock::time_point curTime, destTime;

    sf::Vector2f position;
    bool collapsing;

public:
    explicit InteractionPopup(GameScene& scene);
    virtual ~InteractionPopup() = default;

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer& renderer) override;

    virtual bool notifyScreenTransition(cpVect displacement) override;

    auto getPosition() const { return position; }
    void setPosition(sf::Vector2f pos) { position = pos; }

    void collapse();
};