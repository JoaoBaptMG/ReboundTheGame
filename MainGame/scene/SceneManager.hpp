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

#include <vector>
#include <memory>
#include <chronoUtils.hpp>

class Scene;
class Renderer;

class SceneManager final : util::non_copyable_movable
{
    std::vector<std::unique_ptr<Scene>> sceneStack;
    enum { None, Push, Pop } scheduledOperation;
    std::unique_ptr<Scene> operationScene;
    size_t popCount;
    bool requestBelow;
    FrameTime curTime;

public:
    SceneManager();
    ~SceneManager();

    void pushScene(Scene* scene);
    void popScene(size_t count = 1);
    void replaceScene(Scene* scene, size_t count = 1);
    
    void pushSceneTransition(Scene* scene, FrameDuration duration);
    void popSceneTransition(size_t count, FrameDuration duration);
    void popSceneTransition(FrameDuration duration) { popSceneTransition(1, duration); }
    void replaceSceneTransition(Scene* scene, size_t count, FrameDuration duration);
    void replaceSceneTransition(Scene* scene, FrameDuration duration)
    { replaceSceneTransition(scene, 1, duration); }

    void handleScreenTransition();

    inline bool hasScenes() const { return !sceneStack.empty(); }
    auto currentScene() const { return sceneStack.back().get(); }

    void update(FrameTime curTime);
    void render(Renderer& renderer);
    
    void updateBelow() { requestBelow = true; }
    void renderBelow() { requestBelow = true; }
};
