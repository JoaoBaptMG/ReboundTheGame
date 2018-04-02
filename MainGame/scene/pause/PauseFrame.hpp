#pragma once

#include <chronoUtils.hpp>

class Renderer;

class PauseFrame
{
public:
    PauseFrame() {}
    virtual ~PauseFrame() {}
    
    virtual void update(FrameTime curTime) = 0;
    virtual void render(Renderer &renderer) = 0;
    
    virtual void activate() = 0;
    virtual void deactivate() = 0;
};
