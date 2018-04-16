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


#include "SceneManager.hpp"
#include "Scene.hpp"
#include "rendering/Renderer.hpp"

#include "Transition.hpp"

SceneManager::SceneManager() : sceneStack(), scheduledOperation(None), operationScene(), popCount(0)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::pushScene(Scene *scene)
{
    operationScene.reset(scene);
    scene->sceneManager = this;
    scheduledOperation = Push;
}

void SceneManager::popScene(size_t count)
{
    if (count == 0) return;
    scheduledOperation = Pop;
    popCount = count;
}

void SceneManager::replaceScene(Scene* scene, size_t count)
{
    operationScene.reset(scene);
    scene->sceneManager = this;
    scheduledOperation = Pop;
    popCount = count-1;
}

void SceneManager::pushSceneTransition(Scene* scene, FrameDuration duration)
{
    if (duration == decltype(duration)()) pushScene(scene);
    else
    {
        auto prevScene = sceneStack.back().get();
        auto nextScene = scene;
        scene->sceneManager = this;
        pushScene(new Transition(prevScene, nextScene, curTime, curTime + duration, true));
    }
}

void SceneManager::popSceneTransition(size_t count, FrameDuration duration)
{
    if (count == 0) return;
    
    if (duration == decltype(duration)()) popScene(count);
    else
    {
        auto prevScene = sceneStack.back().release();
        sceneStack.pop_back();
        auto nextScene = sceneStack[sceneStack.size()-count].release();
        
        replaceScene(new Transition(prevScene, nextScene, curTime, curTime + duration, false, true), count);
    }
}

void SceneManager::replaceSceneTransition(Scene* scene, size_t count, FrameDuration duration)
{
    if (duration == decltype(duration)()) replaceScene(scene);
    else
    {
        auto prevScene = sceneStack.back().release();
        auto nextScene = scene;
        scene->sceneManager = this;
        
        replaceScene(new Transition(prevScene, nextScene, curTime, curTime + duration), count);
    }
}

void SceneManager::handleScreenTransition()
{
    if (scheduledOperation == Push)
    {
        if (!sceneStack.empty()) sceneStack.back()->pause();
        sceneStack.emplace_back(operationScene.release());
    }
    else if (scheduledOperation == Pop)
    {
        while (popCount--) sceneStack.pop_back();
        
        if (operationScene)
        {
            using std::swap;
            swap(operationScene, sceneStack.back());
            operationScene.reset();
        }
        else
        {
            if (!sceneStack.empty()) sceneStack.back()->resume();
        }
    }
    
    scheduledOperation = None;
}

void SceneManager::update(FrameTime curTime)
{
    this->curTime = curTime;
    handleScreenTransition();

    if (sceneStack.empty()) return;
    auto it = sceneStack.rbegin();
    do
    {
        if (*it)
        {
            requestBelow = false;
            (*it)->update(curTime);
        }
        it++;
    } while (requestBelow && it != sceneStack.rend());
}

void SceneManager::render(Renderer& renderer)
{
    if (sceneStack.empty()) return;
    auto it = sceneStack.rbegin();
    do
    {
        if (*it)
        {
            requestBelow = false;
            (*it)->render(renderer);
        }
        it++;
    } while (requestBelow && it != sceneStack.rend());
}
