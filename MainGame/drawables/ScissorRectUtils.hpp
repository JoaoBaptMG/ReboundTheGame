#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>

class ScissorRectGuard
{
    GLint prevScissor[4];
    GLboolean prevScissorStatus;

public:
    ScissorRectGuard();
    ScissorRectGuard(const sf::FloatRect& rect);
    ~ScissorRectGuard();
};

class ScissorRectPush : public sf::Drawable
{
    sf::FloatRect scissorRect;

public:
    ScissorRectPush() : scissorRect(NAN, NAN, NAN, NAN) {}
    ScissorRectPush(sf::FloatRect rect) : scissorRect(rect) {}

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    friend struct ScissorRect;
};

class ScissorRectPop : public sf::Drawable
{
    ScissorRectPop() {}
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    friend struct ScissorRect;
};

struct ScissorRect
{
    static const ScissorRectPop& pop();
    static const ScissorRectPush& push();
};
