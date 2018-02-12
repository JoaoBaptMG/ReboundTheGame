#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <map>
#include <stack>
#include <non_copyable_movable.hpp>

using RenderData = std::pair<std::reference_wrapper<const sf::Drawable>,sf::RenderStates>;

class Renderer final : util::non_copyable
{
    std::multimap<long,RenderData> drawableList;
    std::stack<sf::Transform> transformStack;

public:
    Renderer() noexcept : currentTransform(sf::Transform::Identity) {}
    Renderer(Renderer&& other) noexcept;
    Renderer& operator=(Renderer other);

    void pushDrawable(const sf::Drawable &drawable, sf::RenderStates states, long depth = 0);

    void pushTransform();
    void popTransform();

    void render(sf::RenderTarget& target);
    void clearState();

    sf::Transform currentTransform;
    
    friend void swap(Renderer& r1, Renderer& r2);
};

