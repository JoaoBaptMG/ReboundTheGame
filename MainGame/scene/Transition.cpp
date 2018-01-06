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
    transitionQuad(sf::FloatRect(0, 0, ScreenWidth, ScreenHeight), sf::Color::Black)
{
    
}

void Transition::update(std::chrono::steady_clock::time_point curTime)
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
