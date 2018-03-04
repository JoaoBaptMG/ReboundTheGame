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
    using SemaphoreFunc = std::function<bool(std::chrono::steady_clock::time_point)>;
    using ScriptFunction = std::function<void(Script&)>;
    
private:
    boost::context::continuation hostContinuation, scriptContinuation;
    SemaphoreFunc currentSemaphore;
    std::chrono::steady_clock::time_point curTime;
    //std::vector<Script> childrenScripts;
    
public:
    Script();
    ~Script();
    
    void runScript(ScriptFunction function);
    
    void waitWhile(SemaphoreFunc func);
    void waitFor(std::chrono::steady_clock::duration dur);
    
    void executeMain(std::function<void()> func);
    
    void cancelScript();
    
    void update(std::chrono::steady_clock::time_point curTime);
    
    auto getCurTime() const { return curTime; }
};
