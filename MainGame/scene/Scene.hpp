#pragma once

#include <non_copyable_movable.hpp>
#include <chrono>

class Renderer;
class SceneManager;

class Scene : util::non_copyable_movable
{
    SceneManager* sceneManager;
    
protected:
    auto& getSceneManager() const { return *sceneManager; }
    
public:
    Scene() : sceneManager(nullptr) {}
    virtual ~Scene() {}

    virtual void update(std::chrono::steady_clock::time_point curTime) = 0;
    virtual void render(Renderer &renderer) = 0;
    
    virtual void pause() {}
    virtual void resume() {}
    
    friend class SceneManager;
};

