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

#include "objects/Enemy.hpp"
#include "drawables/Sprite.hpp"

#include <SFML/Graphics.hpp>
#include <chronoUtils.hpp>
#include <memory>
#include <cppmunk/Body.h>
#include <cppmunk/Shape.h>

class GameScene;
class Renderer;
class Player;

namespace enemies
{
    class Floater final : public ::Enemy
    {
        cpVect originalPos;
        Sprite sprite;

        FrameTime initialTime;
        std::shared_ptr<cp::Shape> collisionShapes[4];

    public:
        Floater(GameScene& gameScene);
        virtual ~Floater();

        void setupPhysics();

        virtual bool onCollisionAttack(Player& player, std::shared_ptr<cp::Shape> shape) override;
        virtual bool onCollisionHit(Player& player, std::shared_ptr<cp::Shape> shape) override;

        virtual void update(FrameTime curTime) override;
        virtual void render(Renderer& renderer) override;
        
        virtual bool notifyScreenTransition(cpVect displacement) override;

#pragma pack(push, 1)
        struct ConfigStruct
        {
            sf::Vector2<int16_t> position;
        };

        static_assert(sizeof(ConfigStruct) == 4*sizeof(char), "Packing failed!");
#pragma pack(pop)

        bool configure(const ConfigStruct& config);
    };
}
