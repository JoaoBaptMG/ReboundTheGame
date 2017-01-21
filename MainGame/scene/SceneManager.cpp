#include "SceneManager.hpp"
#include "Scene.hpp"
#include "../rendering/Renderer.hpp"

SceneManager::SceneManager() : sceneStack(), deletionScheduled(false), postDeletionScene()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::pushScene(Scene *scene)
{
    sceneStack.emplace(scene);
}

void SceneManager::popScene()
{
    deletionScheduled = true;
}

void SceneManager::replaceScene(Scene* scene)
{
    postDeletionScene.reset(scene);
    deletionScheduled = true;
}

void SceneManager::handleScreenTransition()
{
    if (deletionScheduled)
    {
        if (postDeletionScene)
        {
            using std::swap;
            swap(postDeletionScene, sceneStack.top());
            postDeletionScene.reset();
        }
        else sceneStack.pop();

        deletionScheduled = false;
    }
}

void SceneManager::update(float dt)
{
    handleScreenTransition();

    sceneStack.top()->update(dt);
}

void SceneManager::render(Renderer& renderer)
{
    sceneStack.top()->render(renderer);
}
