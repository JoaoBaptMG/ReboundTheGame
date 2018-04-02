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
