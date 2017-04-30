#include "Powerup.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "utility/chronoUtils.hpp"
#include "utility/vector_math.hpp"
#include "defaults.hpp"

#include <cppmunk/PolyShape.h>
#include <chipmunk.h>
#include <chipmunk_unsafe.h>

constexpr float RotationSpeed = 240;

constexpr float PerspectiveFactor = 1.2071067811865475244008443621; // 1/(2*tan(pi/8))
constexpr float PerspectiveDistance = PerspectiveFactor * PlayfieldHeight;

using namespace collectibles;

void Powerup::makeRotatedRect()
{
    cpVect verts[4];

    int i = 0;
    for (cpVect pt : { cpVect{-24, -24}, cpVect{24, -24}, cpVect{24, 24}, cpVect{-24, 24} })
    {
        cpFloat x =  pt.x * cosf(rotationAngle);
        cpFloat y =  pt.y;
        cpFloat z = -pt.x * sinf(rotationAngle) + PerspectiveDistance;

        cpVect transformed = cpVect{x/z, y/z} * PerspectiveDistance;
        verts[i] = transformed;

        sf::Vector2f displayTransformed(std::floor(transformed.x), std::floor(transformed.y));
        vertices[i].position = displayTransformed;
        i++;
    }

    cpPolyShapeSetVerts(*collisionShape, 4, verts, cpTransformIdentity);
}

Powerup::Powerup(GameScene& scene) : Collectible(scene), vertices(sf::Quads, 4), rotationAngle(0)
{
    setupPhysics();

    vertices[0].color = sf::Color::White;
    vertices[0].texCoords = sf::Vector2f(0, 0);

    vertices[1].color = sf::Color::White;
    vertices[1].texCoords = sf::Vector2f(48, 0);

    vertices[2].color = sf::Color::White;
    vertices[2].texCoords = sf::Vector2f(48, 48);

    vertices[3].color = sf::Color::White;
    vertices[3].texCoords = sf::Vector2f(0, 48);
}

bool Powerup::configure(const Powerup::ConfigStruct& config)
{
    auto pos = cpVect{ (float)config.position.x, (float)config.position.y };
    collisionBody->setPosition(pos);

    abilityLevel = config.abilityLevel;
    if (abilityLevel <= global_AbilityLevel) return false;
    if (abilityLevel > 10) return false;

    std::string name = "powerup" + std::to_string(abilityLevel) + ".png";
    texture = gameScene.getResourceManager().load<sf::Texture>(name);
    
    return true;
}

void Powerup::setupPhysics()
{
    using namespace cp;

    collisionBody = std::make_shared<Body>(Body::Kinematic);
    collisionShape = std::make_shared<PolyShape>(collisionBody, std::vector<cpVect>{});
    makeRotatedRect();

    collisionShape->setCollisionType(Collectible::CollisionType);
    collisionShape->setSensor(true);

    gameScene.getGameSpace().add(collisionBody);
    gameScene.getGameSpace().add(collisionShape);
    
    Collectible::setupPhysics();
}

Powerup::~Powerup()
{
    gameScene.getGameSpace().remove(collisionShape);
    gameScene.getGameSpace().remove(collisionBody);
}

void Powerup::onCollect(Player& player)
{
    player.upgradeToAbilityLevel(abilityLevel);
    remove();
}

void Powerup::update(std::chrono::steady_clock::time_point curTime)
{
    rotationAngle += degreesToRadians(RotationSpeed) * toSeconds<float>(UpdateFrequency);
    makeRotatedRect();
}

void Powerup::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(vertices, sf::RenderStates(texture.get()), 25);
    renderer.popTransform();
}
