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

#include "GameObject.hpp"
#include "scene/GameScene.hpp"

#include <memory>
#include <chipmunk/chipmunk.h>
#include <cppmunk.h>

class Player;

class Collectible : public GameObject
{
    void setupCollisionHandlers(cp::Space* space);

protected:
    std::shared_ptr<cp::Body> collisionBody;

public:
    Collectible(GameScene& scene);
    virtual ~Collectible() {}

    void setupPhysics();

    virtual void onCollect(Player& player) = 0;

    virtual bool notifyScreenTransition(cpVect displacement);

    auto getPosition() const { return collisionBody->getPosition(); }
    auto getDisplayPosition() const
    {
        auto vec = getPosition();
        return glm::vec2((float)std::round(vec.x), (float)std::round(vec.y));
    }

    static constexpr cpCollisionType CollisionType = 'item';
};
