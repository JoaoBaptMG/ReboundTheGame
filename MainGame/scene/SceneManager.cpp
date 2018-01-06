#include "SceneManager.hpp"
#include "Scene.hpp"
#include "rendering/Renderer.hpp"

#include "Transition.hpp"

SceneManager::SceneManager() : sceneStack(), scheduledOperation(None), operationScene()
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

void SceneManager::popScene()
{
    scheduledOperation = Pop;
}

void SceneManager::replaceScene(Scene* scene)
{
    operationScene.reset(scene);
    scene->sceneManager = this;
    scheduledOperation = Pop;
}

void SceneManager::pushSceneTransition(Scene* scene, std::chrono::steady_clock::duration duration)
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

void SceneManager::popSceneTransition(std::chrono::steady_clock::duration duration)
{
    if (duration == decltype(duration)()) popScene();
    else
    {
        sceneStack.back()->sceneManager = nullptr;
        auto prevScene = sceneStack.back().release();
        sceneStack.pop_back();
        auto nextScene = sceneStack.back().release();
        replaceScene(new Transition(prevScene, nextScene, curTime, curTime + duration));
    }
}

void SceneManager::replaceSceneTransition(Scene* scene, std::chrono::steady_clock::duration duration)
{
    if (duration == decltype(duration)()) replaceScene(scene);
    else
    {
        auto prevScene = sceneStack.back().release();
        auto nextScene = scene;
        scene->sceneManager = this;
        replaceScene(new Transition(prevScene, nextScene, curTime, curTime + duration));
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
        if (operationScene)
        {
            using std::swap;
            swap(operationScene, sceneStack.back());
            operationScene.reset();
        }
        else
        {
            sceneStack.pop_back();
            if (!sceneStack.empty()) sceneStack.back()->resume();
        }
    }
    
    scheduledOperation = None;
}

void SceneManager::update(std::chrono::steady_clock::time_point curTime)
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
