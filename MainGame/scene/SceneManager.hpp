#pragma once

#include <non_copyable_movable.hpp>

#include <vector>
#include <memory>
#include <chrono>

class Scene;
class Renderer;

class SceneManager final : util::non_copyable_movable
{
    std::vector<std::unique_ptr<Scene>> sceneStack;
    enum { None, Push, Pop } scheduledOperation;
    std::unique_ptr<Scene> operationScene;
    bool requestBelow;
    std::chrono::steady_clock::time_point curTime;

public:
    SceneManager();
    ~SceneManager();

    void pushScene(Scene* scene);
    void popScene();
    void replaceScene(Scene* scene);
    
    void pushSceneTransition(Scene* scene, std::chrono::steady_clock::duration duration);
    void popSceneTransition(std::chrono::steady_clock::duration duration);
    void replaceSceneTransition(Scene* scene, std::chrono::steady_clock::duration duration);

    void handleScreenTransition();

    inline bool hasScenes() const { return !sceneStack.empty(); }
    auto currentScene() const { return sceneStack.back().get(); }

    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer);
    
    void updateBelow() { requestBelow = true; }
    void renderBelow() { requestBelow = true; }
};
