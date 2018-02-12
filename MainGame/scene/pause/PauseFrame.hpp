#pragma once

#include <chrono>

class Renderer;

class PauseFrame
{
public:
    PauseFrame() {}
    virtual ~PauseFrame() {}
    
    virtual void update(std::chrono::steady_clock::time_point curTime) = 0;
    virtual void render(Renderer &renderer) = 0;
    
    virtual void activate() = 0;
    virtual void deactivate() = 0;
};
