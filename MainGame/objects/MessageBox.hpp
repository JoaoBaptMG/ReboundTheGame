#pragma once

#include <cstddef>
#include <string>
#include <chronoUtils.hpp>
#include "input/InputManager.hpp"
#include "input/CommonActions.hpp"
#include "drawables/Sprite.hpp"
#include "drawables/TextDrawable.hpp"
#include "gameplay/Script.hpp"

class Renderer;
class ResourceManager;
class LocalizationManager;
struct Settings;

class MessageBox final
{
    Sprite messageBackground;
    TextDrawable messageText;
    
    ButtonAction messageAction;
    
    std::string currentText;
    std::chrono::steady_clock::duration letterPeriod;
    std::chrono::steady_clock::time_point curTime, initTime;
    
public:
    MessageBox(const Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm);
    ~MessageBox() {}
    
    void display(Script& script, std::string);
    
    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer);
};
