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


#include "Script.hpp"
#include <assert.hpp>

Script::Script() {}

Script::~Script() {}

void Script::runScript(Script::ScriptFunction function)
{
    using namespace boost::context;
    
    if (scriptContinuation) cancelScript();
    
    scriptContinuation = callcc([=] (continuation&& c)
    {
        hostContinuation = c.resume();
        function(*this);
        return std::move(hostContinuation);
    });
    
    currentSemaphore = [=] (FrameTime) { return false; };
}

void Script::cancelScript()
{
    using namespace boost::context;
    scriptContinuation = continuation();
    hostContinuation = continuation();
    currentSemaphore = SemaphoreFunc();
}

void Script::waitWhile(Script::SemaphoreFunc func)
{
    ASSERT(hostContinuation && !currentSemaphore);
    currentSemaphore = func;
    hostContinuation = hostContinuation.resume();
}

void Script::waitFor(FrameDuration dur)
{
    waitWhile([=, initTime = curTime](auto curTime) { return curTime - initTime <= dur; });
}

void Script::executeMain(std::function<void()> func)
{
    waitWhile([=](auto curTime)
    {
        func();
        return false;
    });
}

void Script::update(FrameTime curTime)
{
    this->curTime = curTime;
    
    if (scriptContinuation)
    {
        if (!currentSemaphore) cancelScript();
        else if (!currentSemaphore(curTime))
        {
            currentSemaphore = SemaphoreFunc();
            scriptContinuation = scriptContinuation.resume();
        }
    }
}
