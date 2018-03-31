#include "KeyboardKeyName.hpp"

#include <iostream>

#if _WIN32

#elif __APPLE__

#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>
#include <utf8.h>
#include <iterator>

static const UCKeyboardLayout* curLayout;

static void initCurLayout()
{
    auto inputSource = TISCopyCurrentKeyboardLayoutInputSource();
    curLayout = (UCKeyboardLayout*)CFDataGetBytePtr((CFDataRef)TISGetInputSourceProperty(inputSource, kTISPropertyUnicodeKeyLayoutData));
    CFRelease(inputSource);
}

std::string scanCodeToLocalizedKeyName(sf::Uint32 code, LocalizationManager& lm)
{
    if (!curLayout) initCurLayout();
    
    uint32_t deadKeyState = 0;
    UniChar str[8];
    UniCharCount actualLength = 0;
    
    auto res = UCKeyTranslate(curLayout, code, kUCKeyActionDisplay, 2, LMGetKbdType(), kUCKeyTranslateNoDeadKeysBit, &deadKeyState, 8, &actualLength, str);
    if (res == noErr)
    {
        std::string resultStr;
        for (UniCharCount i = 0; i < actualLength; i++)
            utf8::unchecked::append(str[i], std::back_inserter(resultStr));
        return resultStr;
    }
    
    return lm.getFormattedString("key-name-unknown", {}, {{"n", code}}, {});
}

std::string scanCodeToKeyName(sf::Uint32 code, LocalizationManager& lm)
{
    std::cout << code << std::endl;
    
    switch (code)
    {
        case kVK_Return: return lm.getString("key-name-macos-return");
        case kVK_Tab: return lm.getString("key-name-tab");
        case kVK_Space: return lm.getString("key-name-space");
        case kVK_Delete: return lm.getString("key-name-macos-delete");
        case kVK_Escape: return lm.getString("key-name-escape");
        case kVK_Command: return lm.getString("key-name-macos-command");
        case kVK_Shift: return lm.getString("key-name-macos-shift");
        case kVK_CapsLock: return lm.getString("key-name-caps-lock");
        case kVK_Option: return lm.getString("key-name-macos-option");
        case kVK_Control: return lm.getString("key-name-macos-control");
        case kVK_RightCommand: return lm.getString("key-name-macos-right-command");
        case kVK_RightShift: return lm.getString("key-name-macos-right-shift");
        case kVK_RightOption: return lm.getString("key-name-macos-right-option");
        case kVK_RightControl: return lm.getString("key-name-macos-right-control");
        case kVK_Function: return lm.getString("key-name-macos-function");
        case kVK_F17: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 17}}, {});
        case kVK_VolumeUp: return lm.getString("key-name-macos-volup");
        case kVK_VolumeDown: return lm.getString("key-name-macos-voldown");
        case kVK_Mute: return lm.getString("key-name-macos-mute");
        case kVK_F18: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 18}}, {});
        case kVK_F19: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 19}}, {});
        case kVK_F20: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 20}}, {});
        case kVK_F5: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 5}}, {});
        case kVK_F6: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 6}}, {});
        case kVK_F7: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 7}}, {});
        case kVK_F3: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 3}}, {});
        case kVK_F8: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 8}}, {});
        case kVK_F9: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 9}}, {});
        case kVK_F11: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 11}}, {});
        case kVK_F13: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 13}}, {});
        case kVK_F16: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 16}}, {});
        case kVK_F14: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 14}}, {});
        case kVK_F10: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 10}}, {});
        case kVK_F12: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 12}}, {});
        case kVK_F15: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 15}}, {});
        case kVK_Help: return lm.getString("key-name-help");
        case kVK_Home: return lm.getString("key-name-home");
        case kVK_PageUp: return lm.getString("key-name-page-up");
        case kVK_ForwardDelete: return lm.getString("key-name-macos-fwd-delete");
        case kVK_F4: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 4}}, {});
        case kVK_End: return lm.getString("key-name-end");
        case kVK_F2: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 2}}, {});
        case kVK_PageDown: return lm.getString("key-name-page-down");
        case kVK_F1: return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", 1}}, {});
        case kVK_LeftArrow: return lm.getString("key-name-left");
        case kVK_RightArrow: return lm.getString("key-name-right");
        case kVK_DownArrow: return lm.getString("key-name-down");
        case kVK_UpArrow: return lm.getString("key-name-up");
        case kVK_ANSI_Keypad0: return lm.getFormattedString("keyboard-key-name-numkey", {}, {{"n", 0}}, {});
        case kVK_ANSI_Keypad1: return lm.getFormattedString("keyboard-key-name-numkey", {}, {{"n", 1}}, {});
        case kVK_ANSI_Keypad2: return lm.getFormattedString("keyboard-key-name-numkey", {}, {{"n", 2}}, {});
        case kVK_ANSI_Keypad3: return lm.getFormattedString("keyboard-key-name-numkey", {}, {{"n", 3}}, {});
        case kVK_ANSI_Keypad4: return lm.getFormattedString("keyboard-key-name-numkey", {}, {{"n", 4}}, {});
        case kVK_ANSI_Keypad5: return lm.getFormattedString("keyboard-key-name-numkey", {}, {{"n", 5}}, {});
        case kVK_ANSI_Keypad6: return lm.getFormattedString("keyboard-key-name-numkey", {}, {{"n", 6}}, {});
        case kVK_ANSI_Keypad7: return lm.getFormattedString("keyboard-key-name-numkey", {}, {{"n", 7}}, {});
        case kVK_ANSI_Keypad8: return lm.getFormattedString("keyboard-key-name-numkey", {}, {{"n", 8}}, {});
        case kVK_ANSI_Keypad9: return lm.getFormattedString("keyboard-key-name-numkey", {}, {{"n", 9}}, {});
        case kVK_ANSI_KeypadDecimal: return lm.getString("key-name-num-decimal");
        case kVK_ANSI_KeypadMultiply: return lm.getString("key-name-num-multply");
        case kVK_ANSI_KeypadPlus: return lm.getString("key-name-num-add");
        case kVK_ANSI_KeypadClear: return lm.getString("key-name-num-clear");
        case kVK_ANSI_KeypadDivide: return lm.getString("key-name-num-divide");
        case kVK_ANSI_KeypadEnter: return lm.getString("key-name-num-return");
        case kVK_ANSI_KeypadMinus: return lm.getString("key-name-num-subtract");
        case kVK_ANSI_KeypadEquals: return lm.getString("key-name-num-equal");
    }
    
    return scanCodeToLocalizedKeyName(code, lm);
}

#elif __linux__

#include <X11/Xlib.h>
#include <cstdlib>
#include <cctype>
#include <utf8.h>
#include <algorithm>
#include "UnixKeyboardMaps.hpp"

namespace sf
{
    namespace priv
    {
        Display* OpenDisplay();
        void CloseDisplay(Display* display);
    }
}

static KeySym* keysyms = nullptr;
static int minKeys, maxKeys, keysymsPerKeycode;

static void initializeKeyMapper()
{
    auto display = sf::priv::OpenDisplay();
    XDisplayKeycodes(display, &minKeys, &maxKeys);
    keysyms = XGetKeyboardMapping(display, minKeys, maxKeys - minKeys + 1, &keysymsPerKeycode);
    sf::priv::CloseDisplay(display);

    std::atexit([]{ XFree(keysyms); });
}

std::string scanCodeToKeyName(sf::Uint32 code, LocalizationManager& lm)
{
    if (!keysyms) initializeKeyMapper();

    size_t i = (code - minKeys) * keysymsPerKeycode;
    for (size_t k = 0; k < keysymsPerKeycode; k++)
    {
        auto key = keysyms[i];

        sf::Uint32 charCode = 0;

        // ASCII character;
        if (key == 32) return lm.getString("key-name-space");
        else if (key > 32 && key <= 126) charCode = islower(key) ? toupper(key) : key;
        else if (key >= 0xA0 && key <= 0xDF) charCode = key;
        else if (key == 0xF7) charCode = 0xF7;
        else if (key >= 0xE0 && key <= 0xFE) charCode = key - 0x20;
        else if (key >= 0x1A0 && key <= 0x1FF) charCode = codeMaps0x100[key - 0x1A0];
        else if (key >= 0x2A0 && key <= 0x2FF) charCode = codeMaps0x200[key - 0x2A0];
        else if (key >= 0x3A0 && key <= 0x3FF) charCode = codeMaps0x300[key - 0x3A0];
        else if (key == 0x47E) charCode = 0x203E;
        else if (key >= 0x4A0 && key <= 0x4DF) charCode = codeMaps0x400[key - 0x4A0];
        else if (key >= 0x5A0 && key <= 0x5FF) charCode = key + 0x60;
        else if (key >= 0x6A0 && key <= 0x6FF) charCode = codeMaps0x600[key - 0x6A0];
        else if (key >= 0x7A0 && key <= 0x7BF) charCode = codeMaps0x700[key - 0x7A0];
        else if (key >= 0x7C0 && key <= 0x7FF) charCode = key + 0x40;
        else if (key >= 0x8A0 && key <= 0x8FF) charCode = codeMaps0x800[key - 0x8A0];
        else if (key >= 0x9E0 && key <= 0x9FF) charCode = codeMaps0x900[key - 0x9E0];
        else if (key >= 0xAA0 && key <= 0xAFF) charCode = codeMaps0xA00[key - 0xAA0];
        else if (key >= 0xBA0 && key <= 0xBFF) charCode = codeMaps0xB00[key - 0xBA0];
        else if (key == 0xCDF) charCode = 0x2017;
        else if (key >= 0xCE0 && key <= 0xCFF) charCode = codeMaps0xC00[key - 0xCE0];
        else if (key >= 0xDA0 && key <= 0xDFF) charCode = codeMaps0xD00[key - 0xDA0];
        else if (key == 0x20AC) charCode = 0x20AC;
        else if (key >= 0x1000000) charCode = key & 0xFFFF;
        else if (key >= 0xFFB0 && key <= 0xFFB9)
            return lm.getFormattedString("keyboard-key-name-numkey", {}, {{"n", key-0xFFB0}}, {});
        else if (key >= 0xFFBE && key <= 0xFFE0)
            return lm.getFormattedString("keyboard-key-name-fkey", {}, {{"n", key-0xFFBD}}, {});
        else
        {
            LangID id = "";

            if (key >= 0xFF50 && key <= 0xFF58) id = miscKeys1[key-0xFF50];
            else if (key >= 0xFFAA && key <= 0xFFAF) id = miscKeys2[key-0xFFAA];
            else if (key >= 0xFF60 && key <= 0xFF6B) id = miscKeys3[key-0xFF60];
            else if (key >= 0xFFE1 && key <= 0xFFEC) id = miscKeys4[key-0xFFE1];
            else
            {
                struct Comparator
                {
                    bool operator()(const MiscPair& first, sf::Uint32 second) const { return first.code < second; }
                    bool operator()(sf::Uint32 first, const MiscPair& second) const { return first < second.code; }
                };

                auto pair = std::equal_range(scrambledMiscKeys, scrambledMiscKeys+scrambledMiscKeysSize, key, Comparator());
                if (pair.first != pair.second) id = pair.first->id;
            }

            if (id != "") return lm.getString(id);
        }

        if (charCode > 0)
        {
            char val[8];
            *(utf8::unchecked::append(charCode, val)) = 0;
            return val;
        }
    }

    return lm.getFormattedString("key-name-unknown", {}, {{"n", code}}, {});
}

#endif
