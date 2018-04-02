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
