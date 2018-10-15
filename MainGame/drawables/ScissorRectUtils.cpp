//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

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

ScissorRectGuard::ScissorRectGuard(const util::rect& rect)
{
    prevScissorStatus = glIsEnabled(GL_SCISSOR_TEST);
    if (prevScissorStatus) glGetIntegerv(GL_SCISSOR_BOX, prevScissor);
    
    glEnable(GL_SCISSOR_TEST);
    glScissor(rect.x, ScreenHeight - rect.y - rect.height, rect.width, rect.height);
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

    if (std::isnan(scissorRect.x) && std::isnan(scissorRect.y))
        glDisable(GL_SCISSOR_TEST);
    else
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(scissorRect.x, ScreenHeight - scissorRect.y - scissorRect.height,
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
