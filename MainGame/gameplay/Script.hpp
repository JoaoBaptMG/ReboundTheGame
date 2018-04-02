#pragma once

#include <chronoUtils.hpp>
#include <vector>
#include <functional>
#include <utility>
#include <list>
#include <type_traits>
#include <boost/context/all.hpp>

class Script
{
public:
    using SemaphoreFunc = std::function<bool(FrameTime)>;
    using ScriptFunction = std::function<void(Script&)>;
    
private:
    boost::context::continuation hostContinuation, scriptContinuation;
    SemaphoreFunc currentSemaphore;
    FrameTime curTime;
    //std::vector<Script> childrenScripts;
    
public:
    Script();
    ~Script();
    
    void runScript(ScriptFunction function);
    
    void waitWhile(SemaphoreFunc func);
    void waitFor(FrameDuration dur);
    
    void executeMain(std::function<void()> func);
    
    void cancelScript();
    
    void update(FrameTime curTime);
    
    auto getCurTime() const { return curTime; }
};
