#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <list>
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
    FrameDuration letterPeriod;
    FrameTime curTime, initTime;

    std::vector<size_t> breakPoints, stopPoints;

    enum State { Idle, OpenBox, NextCharacter, FullStop, FullStopPageBreak, FadingPage, CloseBox } curState;
    std::vector<size_t>::iterator curBreak, curStop;
    size_t firstVisibleCharacter, curCharacter, lineOffset;
    bool spawnNewMessage;
    
public:
    MessageBox(const Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm);
    ~MessageBox() = default;
    
    void display(Script& script, std::string);
    void displayString(Script& script, const LangID& id);
    void displayFormattedString(Script& script, const LangID& id, const StringSpecifierMap &stringSpecifiers,
                                const NumberSpecifierMap& numberSpecifiers, const RawSpecifierMap& rawSpecifiers);

    void buildMessageText();
    void searchForSpecialMarkers(std::map<size_t,size_t>& colorChanges, std::list<size_t>& tempBreakPoints);
    void autoPlaceBreakPoints(std::list<size_t> tempBreakPoints);

    void update(FrameTime curTime);
    void render(Renderer& renderer);
};
