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
