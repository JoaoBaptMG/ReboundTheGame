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

#include "objects/Collectible.hpp"
#include "drawables/Sprite.hpp"

#include <SFML/Graphics.hpp>
#include <chronoUtils.hpp>
#include <memory>
#include <cppmunk/Body.h>
#include <cppmunk/Shape.h>

namespace collectibles
{
    class Powerup final : public ::Collectible
    {
        sf::VertexArray vertices;
        std::shared_ptr<sf::Texture> texture;
        std::shared_ptr<cp::Shape> collisionShape;
        size_t abilityLevel;

        float rotationAngle;
        void makeRotatedRect();

    public:
        Powerup(GameScene& scene);
        virtual ~Powerup();

        void setupPhysics();

        virtual void onCollect(Player& player) override;

        virtual void update(FrameTime curTime) override;
        virtual void render(Renderer& renderer) override;
    
#pragma pack(push, 1)
        struct ConfigStruct
        {
            sf::Vector2<int16_t> position;
            uint8_t abilityLevel;
        };

        static_assert(sizeof(ConfigStruct) == 5*sizeof(char), "Packing failed!");
#pragma pack(pop)

        bool configure(const ConfigStruct& config);
    };
}
