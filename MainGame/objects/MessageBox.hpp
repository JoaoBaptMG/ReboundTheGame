#pragma once

#include <cstddef>
#include <string>
#include <set>
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
    Sprite messageBackground, messageIcon;
    TextDrawable messageText;

    LocalizationManager& localizationManager;
    float actualMessageHeight;

    ButtonAction<6> messageAction;

    std::string currentText;
    std::chrono::steady_clock::duration letterPeriod;
    std::chrono::steady_clock::time_point curTime, initTime;

    std::vector<size_t> breakPoints, stopPoints;

    enum State { Idle, OpenBox, NextCharacter, FullStop, FullStopPageBreak, FadingPage, CloseBox } curState;


    size_t lineOffset;
    struct VisibleBounds { size_t begin, end, lineCount; } curBounds;
    bool updateLines;
    
public:
    MessageBox(const Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm);
    ~MessageBox() {}
    
    void display(Script& script, std::string);
    void displayString(Script& script, const LangID& id);
    void displayFormattedString(Script& script, const LangID& id, const StringSpecifierMap &stringSpecifiers,
                                const NumberSpecifierMap& numberSpecifiers, const RawSpecifierMap& rawSpecifiers);

    void buildMessageText();
    void searchForSpecialMarkers(std::map<size_t,size_t>& colorChanges);

    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer);
};
