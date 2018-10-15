//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


#include "KeyNames.hpp"

LangID getStringIDForKey(sf::Keyboard::Key key)
{
    switch (key)
    {
        case sf::Keyboard::Key::Unknown:   return "keyboard-key-name-unknown";
        case sf::Keyboard::Key::A:         return "keyboard-key-name-a";
        case sf::Keyboard::Key::B:         return "keyboard-key-name-b";
        case sf::Keyboard::Key::C:         return "keyboard-key-name-c";
        case sf::Keyboard::Key::D:         return "keyboard-key-name-d";
        case sf::Keyboard::Key::E:         return "keyboard-key-name-e";
        case sf::Keyboard::Key::F:         return "keyboard-key-name-f";
        case sf::Keyboard::Key::G:         return "keyboard-key-name-g";
        case sf::Keyboard::Key::H:         return "keyboard-key-name-h";
        case sf::Keyboard::Key::I:         return "keyboard-key-name-i";
        case sf::Keyboard::Key::J:         return "keyboard-key-name-j";
        case sf::Keyboard::Key::K:         return "keyboard-key-name-k";
        case sf::Keyboard::Key::L:         return "keyboard-key-name-l";
        case sf::Keyboard::Key::M:         return "keyboard-key-name-m";
        case sf::Keyboard::Key::N:         return "keyboard-key-name-n";
        case sf::Keyboard::Key::O:         return "keyboard-key-name-o";
        case sf::Keyboard::Key::P:         return "keyboard-key-name-p";
        case sf::Keyboard::Key::Q:         return "keyboard-key-name-q";
        case sf::Keyboard::Key::R:         return "keyboard-key-name-r";
        case sf::Keyboard::Key::S:         return "keyboard-key-name-s";
        case sf::Keyboard::Key::T:         return "keyboard-key-name-t";
        case sf::Keyboard::Key::U:         return "keyboard-key-name-u";
        case sf::Keyboard::Key::V:         return "keyboard-key-name-v";
        case sf::Keyboard::Key::W:         return "keyboard-key-name-w";
        case sf::Keyboard::Key::X:         return "keyboard-key-name-x";
        case sf::Keyboard::Key::Y:         return "keyboard-key-name-y";
        case sf::Keyboard::Key::Z:         return "keyboard-key-name-z";
        case sf::Keyboard::Key::Num0:      return "keyboard-key-name-num0";
        case sf::Keyboard::Key::Num1:      return "keyboard-key-name-num1";
        case sf::Keyboard::Key::Num2:      return "keyboard-key-name-num2";
        case sf::Keyboard::Key::Num3:      return "keyboard-key-name-num3";
        case sf::Keyboard::Key::Num4:      return "keyboard-key-name-num4";
        case sf::Keyboard::Key::Num5:      return "keyboard-key-name-num5";
        case sf::Keyboard::Key::Num6:      return "keyboard-key-name-num6";
        case sf::Keyboard::Key::Num7:      return "keyboard-key-name-num7";
        case sf::Keyboard::Key::Num8:      return "keyboard-key-name-num8";
        case sf::Keyboard::Key::Num9:      return "keyboard-key-name-num9";
        case sf::Keyboard::Key::Escape:    return "keyboard-key-name-escape";
        case sf::Keyboard::Key::LControl:  return "keyboard-key-name-lcontrol";
        case sf::Keyboard::Key::LShift:    return "keyboard-key-name-lshift";
        case sf::Keyboard::Key::LAlt:      return "keyboard-key-name-lalt";
        case sf::Keyboard::Key::LSystem:   return "keyboard-key-name-lsystem";
        case sf::Keyboard::Key::RControl:  return "keyboard-key-name-rcontrol";
        case sf::Keyboard::Key::RShift:    return "keyboard-key-name-rshift";
        case sf::Keyboard::Key::RAlt:      return "keyboard-key-name-ralt";
        case sf::Keyboard::Key::RSystem:   return "keyboard-key-name-rsystem";
        case sf::Keyboard::Key::Menu:      return "keyboard-key-name-menu";
        case sf::Keyboard::Key::LBracket:  return "keyboard-key-name-lbracket";
        case sf::Keyboard::Key::RBracket:  return "keyboard-key-name-rbracket";
        case sf::Keyboard::Key::SemiColon: return "keyboard-key-name-semicolon";
        case sf::Keyboard::Key::Comma:     return "keyboard-key-name-comma";
        case sf::Keyboard::Key::Period:    return "keyboard-key-name-period";
        case sf::Keyboard::Key::Quote:     return "keyboard-key-name-quote";
        case sf::Keyboard::Key::Slash:     return "keyboard-key-name-slash";
        case sf::Keyboard::Key::BackSlash: return "keyboard-key-name-backslash";
        case sf::Keyboard::Key::Tilde:     return "keyboard-key-name-tilde";
        case sf::Keyboard::Key::Equal:     return "keyboard-key-name-equal";
        case sf::Keyboard::Key::Dash:      return "keyboard-key-name-dash";
        case sf::Keyboard::Key::Space:     return "keyboard-key-name-space";
        case sf::Keyboard::Key::Return:    return "keyboard-key-name-return";
        case sf::Keyboard::Key::BackSpace: return "keyboard-key-name-backspace";
        case sf::Keyboard::Key::Tab:       return "keyboard-key-name-tab";
        case sf::Keyboard::Key::PageUp:    return "keyboard-key-name-pageup";
        case sf::Keyboard::Key::PageDown:  return "keyboard-key-name-pagedown";
        case sf::Keyboard::Key::End:       return "keyboard-key-name-end";
        case sf::Keyboard::Key::Home:      return "keyboard-key-name-home";
        case sf::Keyboard::Key::Insert:    return "keyboard-key-name-insert";
        case sf::Keyboard::Key::Delete:    return "keyboard-key-name-delete";
        case sf::Keyboard::Key::Add:       return "keyboard-key-name-add";
        case sf::Keyboard::Key::Subtract:  return "keyboard-key-name-subtract";
        case sf::Keyboard::Key::Multiply:  return "keyboard-key-name-multiply";
        case sf::Keyboard::Key::Divide:    return "keyboard-key-name-divide";
        case sf::Keyboard::Key::Left:      return "keyboard-key-name-left";
        case sf::Keyboard::Key::Right:     return "keyboard-key-name-right";
        case sf::Keyboard::Key::Up:        return "keyboard-key-name-up";
        case sf::Keyboard::Key::Down:      return "keyboard-key-name-down";
        case sf::Keyboard::Key::Numpad0:   return "keyboard-key-name-numpad0";
        case sf::Keyboard::Key::Numpad1:   return "keyboard-key-name-numpad1";
        case sf::Keyboard::Key::Numpad2:   return "keyboard-key-name-numpad2";
        case sf::Keyboard::Key::Numpad3:   return "keyboard-key-name-numpad3";
        case sf::Keyboard::Key::Numpad4:   return "keyboard-key-name-numpad4";
        case sf::Keyboard::Key::Numpad5:   return "keyboard-key-name-numpad5";
        case sf::Keyboard::Key::Numpad6:   return "keyboard-key-name-numpad6";
        case sf::Keyboard::Key::Numpad7:   return "keyboard-key-name-numpad7";
        case sf::Keyboard::Key::Numpad8:   return "keyboard-key-name-numpad8";
        case sf::Keyboard::Key::Numpad9:   return "keyboard-key-name-numpad9";
        case sf::Keyboard::Key::F1:        return "keyboard-key-name-f1";
        case sf::Keyboard::Key::F2:        return "keyboard-key-name-f2";
        case sf::Keyboard::Key::F3:        return "keyboard-key-name-f3";
        case sf::Keyboard::Key::F4:        return "keyboard-key-name-f4";
        case sf::Keyboard::Key::F5:        return "keyboard-key-name-f5";
        case sf::Keyboard::Key::F6:        return "keyboard-key-name-f6";
        case sf::Keyboard::Key::F7:        return "keyboard-key-name-f7";
        case sf::Keyboard::Key::F8:        return "keyboard-key-name-f8";
        case sf::Keyboard::Key::F9:        return "keyboard-key-name-f9";
        case sf::Keyboard::Key::F10:       return "keyboard-key-name-f10";
        case sf::Keyboard::Key::F11:       return "keyboard-key-name-f11";
        case sf::Keyboard::Key::F12:       return "keyboard-key-name-f12";
        case sf::Keyboard::Key::F13:       return "keyboard-key-name-f13";
        case sf::Keyboard::Key::F14:       return "keyboard-key-name-f14";
        case sf::Keyboard::Key::F15:       return "keyboard-key-name-f15";
        case sf::Keyboard::Key::Pause:     return "keyboard-key-name-pause";
        default:                           return "";
    }
}
