#include "Player.hpp"
#include <functional>
#include "../utility/vector_math.hpp"
#include "../input/PlayerController.hpp"
#include "../scene/GameScene.hpp"
#include "../rendering/Renderer.hpp"
#include "../resources/ResourceManager.hpp"
#include <chipmunk.h>

Player::Player(const PlayerController& controller, ResourceManager& resMan, GameScene& scene)
    : controller(controller)
{
    sprite.setTexture(*resMan.load<sf::Texture>("player.png"));
    sprite.setOrigin(32, 32);

    handle = scene.getCollisionHandler().newDynamicBody();
    auto shp = handle.addShape(cp::CircleShapeMake(handle.getBodyPtr(), 32, cpvzero));
    cpShapeSetDensity(shp, 1);
    cpShapeSetFriction(shp, 1);
    cpShapeSetElasticity(shp, 1);

    cpBodySetPosition(handle.getBodyPtr(), { 160, 160 });
}

Player::~Player()
{
}

void Player::update(float dt)
{
    auto vec = controller.getMovementVector();
    //printf("%g\n", vec.x);

    auto vel = cpBodyGetVelocity(handle.getBodyPtr());
    auto dest = 320 * vec.x;

    if (std::abs(vel.x - dest) < 6.0)
    {
        vel.x = dest;
        cpBodySetVelocity(handle.getBodyPtr(), vel);
    }

    auto base = cpVect{vel.x < dest ? 1.0 : vel.x > dest ? -1.0 : 0.0, 0.0};

    cpBodyApplyForceAtWorldPoint(handle.getBodyPtr(), base * 2400000.0,
        cpBodyGetPosition(handle.getBodyPtr()));

    if (controller.hasJumpAction())
        cpBodyApplyImpulseAtWorldPoint(handle.getBodyPtr(),
            cpVect{0, -600.0} * cpBodyGetMass(handle.getBodyPtr()),
            cpBodyGetPosition(handle.getBodyPtr()));
}

void Player::render(Renderer& renderer)
{
    renderer.pushTransform();

    renderer.currentTransform.translate(getDisplayPosition());
    renderer.currentTransform.rotate(radiansToDegrees(cpBodyGetAngle(handle.getBodyPtr())));

    renderer.pushDrawable(sprite, {}, 2);
    renderer.popTransform();
}

