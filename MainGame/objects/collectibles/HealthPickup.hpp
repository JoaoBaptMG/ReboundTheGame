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
    class HealthPickup final : public ::Collectible
    {
        Sprite sprite;
        std::shared_ptr<cp::Shape> collisionShape;
        size_t healthAmount;

    public:
        HealthPickup(GameScene& scene, size_t amount);
        HealthPickup(GameScene& scene);
        virtual ~HealthPickup();

        void setupPhysics();

        virtual void onCollect(Player& player) override;

        virtual void update(FrameTime curTime) override;
        virtual void render(Renderer& renderer) override;
    
#pragma pack(push, 1)
        struct ConfigStruct
        {
            glm::i16vec2 position;
            uint16_t healthAmount;
        };

        static_assert(sizeof(ConfigStruct) == 6*sizeof(char), "Packing failed!");
#pragma pack(pop)

        bool configure(const ConfigStruct& config);
    };
}
