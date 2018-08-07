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

#include <non_copyable_movable.hpp>
#include <generic_ptrs.hpp>
#include <chipmunk/chipmunk.h>

#include <memory>
#include <chronoUtils.hpp>
#include <functional>
#include <InputStream.hpp>

class GameScene;
class Renderer;

class GameObject : util::non_copyable
{
protected:
    GameScene& gameScene;
    std::string name;

    bool shouldRemove:1;
    bool isPersistent:1;
    bool transitionState:1;

public:
    GameObject(GameScene& scene) : gameScene(scene), shouldRemove(false), isPersistent(false), transitionState(false),
        name() {}
    inline void remove() { shouldRemove = true; }

    virtual void update(FrameTime curTime) = 0;
    virtual void render(Renderer& renderer) = 0;

    virtual bool notifyScreenTransition(cpVect displacement) { return false; }

    std::string getName() const { return name; }
    void setName(std::string name) { this->name = name; }

    virtual ~GameObject() {}

    friend class GameScene;

    static constexpr cpCollisionType Interactable = 'itbl';
    using InteractionHandler = std::function<void(uint32_t,void*)>;
};

struct GameObjectDescriptor;

util::generic_shared_ptr readParametersFromStream(InputStream& stream, std::string klass);
std::unique_ptr<GameObject> createObjectFromDescriptor(GameScene& gameScene, const GameObjectDescriptor& descriptor);
