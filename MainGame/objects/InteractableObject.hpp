#pragma once

#include "GameObject.hpp"
#include "Player.hpp"
#include "InteractionPopup.hpp"

#include <cppmunk.h>
#include <memory>
#include <chrono>

class GameScene;
class Renderer;

class InteractableObject : public GameObject
{
    InteractionPopup* currentPopup;

protected:
    cpVect interactionCenter, popupPosition;
    cpFloat interactionRadius;
    bool active;

public:
    explicit InteractableObject(GameScene& scene);
    virtual ~InteractableObject() = default;

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual bool notifyScreenTransition(cpVect displacement) override;

    virtual void interact() = 0;
};