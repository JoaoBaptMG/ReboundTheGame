//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


#include "Powerup.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Texture.hpp"
#include "resources/ResourceManager.hpp"
#include <chronoUtils.hpp>
#include <vector_math.hpp>
#include "defaults.hpp"
#include "gameplay/ScriptedPlayerController.hpp"

#include "objects/GameObjectFactory.hpp"
#include "objects/Camera.hpp"

#include <cppmunk/PolyShape.h>
#include <chipmunk/chipmunk.h>
#include <chipmunk/chipmunk_unsafe.h>

constexpr float RotationSpeed = 240;

constexpr float PerspectiveFactor = 1.2071067811865475244008443621f; // 1/(2*tan(pi/8))
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

    vertices[0].color = Colors::White;
    vertices[0].texCoords = sf::Vector2f(0, 0);

    vertices[1].color = Colors::White;
    vertices[1].texCoords = sf::Vector2f(48, 0);

    vertices[2].color = Colors::White;
    vertices[2].texCoords = sf::Vector2f(48, 48);

    vertices[3].color = Colors::White;
    vertices[3].texCoords = sf::Vector2f(0, 48);
}

bool Powerup::configure(const Powerup::ConfigStruct& config)
{
    auto pos = cpVect{ (float)config.position.x, (float)config.position.y };
    collisionBody->setPosition(pos);

    abilityLevel = config.abilityLevel;
    if (abilityLevel > 12) return false;

    std::string name = "powerup" + std::to_string(abilityLevel) + ".png";
    texture = gameScene.getResourceManager().load<Texture>(name);
    
    return abilityLevel == 11 ? !gameScene.getSavedGame().getDoubleArmor() :
        abilityLevel == 12 ? !gameScene.getSavedGame().getMoveRegen() :
        gameScene.getSavedGame().getAbilityLevel() < abilityLevel;
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
    if (abilityLevel == 11)
    {
        player.enableDoubleArmor();
        gameScene.getSavedGame().setDoubleArmor(true);
    }
    else if (abilityLevel == 12)
    {
        player.enableMoveRegen();
        gameScene.getSavedGame().setMoveRegen(true);
    }
    else
    {
        player.upgradeToAbilityLevel(abilityLevel);
        gameScene.getSavedGame().setAbilityLevel(abilityLevel);
    }

    gameScene.playSound("powerup-collect.wav");

    gameScene.runCutsceneScript([&gameScene = this->gameScene, abilityLevel = this->abilityLevel] (Script& script)
    {
        const auto& lm = gameScene.getLocalizationManager();

        LangID parm = "powerup" + std::to_string(abilityLevel) + "-name";
        LangID pmsg = "msg-powerup-description" + std::to_string(abilityLevel);

        auto &msgbox = gameScene.getMessageBox();

        LangID messagePowerup = abilityLevel > 10 ? "msg-secret-powerup-collect" : "msg-powerup-collect";
        std::string msg = lm.getFormattedString(messagePowerup, {{"pname", parm}}, {}, {}) + "\n";
        msg += lm.getFormattedString(pmsg, {}, {}, gameScene.getInputSpecifierMap());
        msgbox.display(script, msg);
    });

    remove();
}

void Powerup::update(FrameTime curTime)
{
    rotationAngle += degreesToRadians(RotationSpeed) * toSeconds<float>(UpdatePeriod);
    makeRotatedRect();
}

void Powerup::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(getDisplayPosition());
    renderer.pushDrawable(vertices, sf::RenderStates(texture.get()), 25);
    renderer.popTransform();
}

REGISTER_GAME_OBJECT(collectibles::Powerup);
