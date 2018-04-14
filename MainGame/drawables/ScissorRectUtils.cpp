#include "ScissorRectUtils.hpp"
#include "defaults.hpp"

#include <cmath>
#include <vector>
#include <unordered_set>
#include <memory>
struct ScissorStatus
{
    GLint prevScissor[4];
    GLboolean prevScissorStatus;
};

std::vector<ScissorStatus> scissorStack;

ScissorRectGuard::ScissorRectGuard()
{
    if ((prevScissorStatus = glIsEnabled(GL_SCISSOR_TEST)))
        glGetIntegerv(GL_SCISSOR_BOX, prevScissor);
    glDisable(GL_SCISSOR_TEST);
}

ScissorRectGuard::ScissorRectGuard(const sf::FloatRect& rect)
{
    prevScissorStatus = glIsEnabled(GL_SCISSOR_TEST);
    if (prevScissorStatus) glGetIntegerv(GL_SCISSOR_BOX, prevScissor);
    
    glEnable(GL_SCISSOR_TEST);
    glScissor(rect.left, ScreenHeight - rect.top - rect.height, rect.width, rect.height);
}

ScissorRectGuard::~ScissorRectGuard()
{
    if (prevScissorStatus)
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(prevScissor[0], prevScissor[1], prevScissor[2], prevScissor[3]);
    }
    else glDisable(GL_SCISSOR_TEST);
}

void ScissorRectPush::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    scissorStack.push_back({});
    if ((scissorStack.back().prevScissorStatus = glIsEnabled(GL_SCISSOR_TEST)))
        glGetIntegerv(GL_SCISSOR_BOX, scissorStack.back().prevScissor);

    if (isnanf(scissorRect.left) && isnanf(scissorRect.top))
        glDisable(GL_SCISSOR_TEST);
    else
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(scissorRect.left, ScreenHeight - scissorRect.top - scissorRect.height,
            scissorRect.width, scissorRect.height);
    }
}

void ScissorRectPop::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!scissorStack.empty())
    {
        if (scissorStack.back().prevScissorStatus)
        {
            glEnable(GL_SCISSOR_TEST);
            glScissor(scissorStack.back().prevScissor[0],
                scissorStack.back().prevScissor[1],
                scissorStack.back().prevScissor[2],
                scissorStack.back().prevScissor[3]);
        }
        else glDisable(GL_SCISSOR_TEST);

        scissorStack.pop_back();
    }
    else glDisable(GL_SCISSOR_TEST);
}

const ScissorRectPop& ScissorRect::pop()
{
    static ScissorRectPop pop{};
    return pop;
}

const ScissorRectPush& ScissorRect::push()
{
    static ScissorRectPush push{};
    return push;
}
