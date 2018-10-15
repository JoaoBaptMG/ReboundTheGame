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

#include "Parallax.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Texture.hpp"
#include "defaults.hpp"
#include <iostream>

#include "objects/GameObjectFactory.hpp"

using namespace background;

Parallax::Parallax(GameScene& scene) : GameObject(scene), parallaxFactor(1.0f), internalDisplacement(0, 0),
    plane(util::rect((float)(ScreenWidth-PlayfieldWidth)/2, (float)(ScreenHeight-PlayfieldHeight)/2,
                        (float)PlayfieldWidth, (float)PlayfieldHeight))
{
}

Parallax::Parallax(GameScene& scene, std::string textureName) : Parallax(scene)
{
    plane.setTexture(scene.getResourceManager().load<Texture>(textureName));
}

bool readFromStream(InputStream& stream, Parallax::ConfigStruct& config)
{
    using namespace util;

    return readFromStream(stream, config.dummy, config.textureName, config.parallaxFactor);
}

bool Parallax::configure(const ConfigStruct& config)
{
    auto texture = gameScene.getResourceManager().load<Texture>(config.textureName);
    auto other = gameScene.getObjectByName<Parallax>(getName());
    if (other && other->plane.getTexture() == texture)
    {
        other->transitionState = false;
        return false;
    }

    parallaxFactor = config.parallaxFactor;
    plane.setTexture(texture);

    return true;
}

void Parallax::update(FrameTime curTime) {}

void Parallax::render(Renderer& renderer)
{
    auto oldTransform = renderer.currentTransform;

    auto trans = glm::vec2(oldTransform[2]) * parallaxFactor + internalDisplacement;
    trans = glm::vec2(std::floor(trans.x), std::floor(trans.y));

    renderer.popTransform();

    renderer.pushTransform();
    renderer.currentTransform *= util::translate(trans);
    renderer.pushDrawable(plane, 0);
    renderer.popTransform();

    renderer.pushTransform();
    renderer.currentTransform = oldTransform;
}

bool Parallax::notifyScreenTransition(cpVect displacement)
{
    internalDisplacement += glm::vec2(displacement.x, displacement.y) * parallaxFactor;
    return true;
}

REGISTER_GAME_OBJECT(background::Parallax);
