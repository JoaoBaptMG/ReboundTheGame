#pragma once

#include <chronoUtils.hpp>
#include "ui/UIPointer.hpp"
#include "language/LangID.hpp"

class SettingsBase;
class Renderer;

class SettingsPanel
{
protected:
    SettingsBase* curSettings;
    UIPointer& pointer;

    SettingsPanel(SettingsBase* curSettings, UIPointer& pointer) : curSettings(curSettings), pointer(pointer) {}

    sf::Vector2f getCenterPosition() const;
    const LangID& getBackId() const;
    void executeBackAction();

public:
    virtual ~SettingsPanel() = default;

    virtual void update(FrameTime curTime) = 0;
    virtual void render(Renderer& renderer) = 0;

    virtual void activate() = 0;
    virtual void deactivate() = 0;
};