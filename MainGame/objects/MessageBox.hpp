#pragma once

#include <cstddef>
#include <string>
#include <map>
#include <chronoUtils.hpp>
#include "input/InputManager.hpp"
#include "input/CommonActions.hpp"
#include "drawables/Sprite.hpp"
#include "drawables/TextDrawable.hpp"
#include "gameplay/Script.hpp"
#include "language/LocalizationManager.hpp"
#include "language/LangID.hpp"

class Renderer;
class ResourceManager;
class LocalizationManager;
struct Settings;

class MessageBox final
{
    Sprite messageBackground;
    TextDrawable messageText;

    ButtonAction<6> messageAction;

    std::string currentText;
    std::chrono::steady_clock::duration letterPeriod;
    std::chrono::steady_clock::time_point curTime, initTime;

    LocalizationManager& localizationManager;
    float actualMessageHeight;
    size_t lineOffset;
    bool updateLines;

    std::map<size_t,size_t> colorChanges;
    
public:
    MessageBox(const Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm);
    ~MessageBox() {}
    
    void display(Script& script, std::string);
    void displayString(Script& script, const LangID& id);
    void displayFormattedString(Script& script, const LangID& id, const StringSpecifierMap &stringSpecifiers,
                                const NumberSpecifierMap& numberSpecifiers, const RawSpecifierMap& rawSpecifiers);

    void buildMessageText();
    void searchForColorMarkers();

    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer);
};
