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


#include "Transition.hpp"

#include "scene/SceneManager.hpp"
#include "rendering/Renderer.hpp"

#include <chronoUtils.hpp>
#include <defaults.hpp>
#include <cmath>
#include <algorithm>

Transition::Transition(Scene* prevScene, Scene* nextScene, decltype(transitionBegin) transitionBegin,
        decltype(transitionBegin) transitionEnd, bool releasePrev, bool isPop)
    : prevScene(prevScene), nextScene(nextScene), releasePrev(releasePrev), isPop(isPop),
    transitionBegin(transitionBegin), transitionEnd(transitionEnd), transitionFactor(0),
    transitionQuad(util::rect(0, 0, ScreenWidth, ScreenHeight), glm::u8vec4::Black)
{
    
}

void Transition::update(FrameTime curTime)
{
    transitionFactor = 2 * toSeconds<float>(curTime - transitionBegin) /
        toSeconds<float>(transitionEnd - transitionBegin);
        
    if (curTime < transitionBegin + (transitionEnd - transitionBegin) / 2)
        prevScene->update(curTime);
    else if (curTime < transitionEnd)
        nextScene->update(curTime);
    else
    {
        if (isPop) nextScene->resume();
        getSceneManager().replaceScene(nextScene);
    }
}

void Transition::render(Renderer &renderer)
{
    if (transitionFactor < 1) prevScene->render(renderer);
    else nextScene->render(renderer);
    
    transitionQuad.setAlpha(255 * std::max(1.0f - fabsf(transitionFactor - 1), 0.0f));
    renderer.pushDrawable(transitionQuad, {}, 1000000000);
}
