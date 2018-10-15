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


#pragma once

#include <memory>

#include <chipmunk/chipmunk.h>
#include <cppmunk/Shape.h>
#include <cppmunk/Body.h>

#include "objects/GameObject.hpp"
#include "objects/Bomb.hpp"
#include "objects/Player.hpp"
#include "scene/GameScene.hpp"
#include "drawables/Sprite.hpp"

class GameScene;
class Renderer;

namespace props
{
    class DestructibleCrate : public ::GameObject
    {
        Sprite sprite;
        std::shared_ptr<cp::Shape> shape;
        InteractionHandler interactionHandler;
        size_t belongingRoomID;

    public:
        DestructibleCrate(GameScene& scene, std::string texture, uint32_t type);
        virtual ~DestructibleCrate();

        void setupPhysics();

        virtual void update(FrameTime curTime) override;
        virtual void render(Renderer& renderer) override;

        virtual bool notifyScreenTransition(cpVect displacement) override;

        auto getPosition() const { return shape->getBody()->getPosition(); }
        void setPosition(cpVect pos) { shape->getBody()->setPosition(pos); }

        virtual bool isDestructionViable() const = 0;
        void explode(sf::FloatRect velocityRect);
        virtual void explode(void* ptr);

        auto getDisplayPosition() const
        {
            auto vec = getPosition();
            return sf::Vector2f((float)std::round(vec.x), (float)std::round(vec.y));
        }
        
        auto getDestroyedKey() const
        {
            return std::to_string(belongingRoomID) + "." + getName() + ".destroyed";
        }

#pragma pack(push, 1)
        struct ConfigStruct
        {
            sf::Vector2<int16_t> position;
        };
        static_assert(sizeof(ConfigStruct) == 4*sizeof(char), "Packing failed!");
#pragma pack(pop)

        bool configure(const ConfigStruct& config);
    };

    struct BombCrate final : public DestructibleCrate
    {
        BombCrate(GameScene& scene) : DestructibleCrate(scene, "bomb-crate.png", Bomb::InteractionType) {}
        virtual bool isDestructionViable() const override { return true; }
        virtual void explode(void* ptr) override;
    };

    struct DashCrate final : public DestructibleCrate
    {
        DashCrate(GameScene& scene) : DestructibleCrate(scene, "dash-crate.png", Player::DashInteractionType) {}
        virtual bool isDestructionViable() const override;
        virtual void explode(void* ptr) override;
    };
}
