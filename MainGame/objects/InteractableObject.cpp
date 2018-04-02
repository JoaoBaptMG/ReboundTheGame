#include "InteractableObject.hpp"

#include <SFML/System.hpp>
#include "scene/GameScene.hpp"

InteractableObject::InteractableObject(GameScene& scene)
    : GameObject(scene), active(true), currentPopup(nullptr)
{

}

void InteractableObject::update(FrameTime curTime)
{
    auto player = gameScene.getObjectByName<Player>("player");

    bool popup = false;
    if (active && player)
        popup = cpvdistsq(interactionCenter, player->getPosition()) < interactionRadius*interactionRadius;

    if (popup && currentPopup == nullptr)
    {
        auto obj = std::make_unique<InteractionPopup>(gameScene);
        obj->setPosition(sf::Vector2f(round(popupPosition.x), round(popupPosition.y)));
        currentPopup = obj.get();
        gameScene.addObject(std::move(obj));
    }
    else if (!popup && currentPopup != nullptr)
    {
        currentPopup->collapse();
        currentPopup = nullptr;
    }

    if (popup && player && player->wantsToInteract()) interact();
}

bool InteractableObject::notifyScreenTransition(cpVect displacement)
{
    interactionCenter += displacement;
    popupPosition += displacement;
    return true;
}
