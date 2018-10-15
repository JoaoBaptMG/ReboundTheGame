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

#pragma once


#include <type_traits>
#include <functional>
#include <streamReaders.hpp>
#include <streamWriters.hpp>

class LocalizationManager;

class InputSource final
{
private:
    enum class Type : uint8_t { None, Keyboard, MouseButton, MouseAxis, JoystickButton, JoystickAxis };

    Type type;
    size_t attribute;

    // Constructor private
    InputSource() = default;
    InputSource(Type type, size_t attr) : type(type), attribute(attr) {}
public:
    std::string getInputName(LocalizationManager& lm);

    static InputSource none;

    static InputSource keyboardKey(size_t scanCode);
    
    static InputSource mouseButton(sf::Mouse::Button button);
    static InputSource mouseX, mouseY, invertedMouseX, invertedMouseY;
    static InputSource mouseWheel(sf::Mouse::Wheel wheel);
    static InputSource invertedMouseWheel(sf::Mouse::Wheel wheel);

    static InputSource joystickButton(unsigned int button);
    static InputSource joystickAxis(sf::Joystick::Axis axis);
    static InputSource invertedJoystickAxis(sf::Joystick::Axis axis);

    friend bool operator==(InputSource in1, InputSource in2);
    friend bool operator<(InputSource in1, InputSource in2);

    friend bool readFromStream(InputStream& stream, InputSource& in);
    friend bool writeToStream(OutputStream& stream, const InputSource& in);

    friend struct std::hash<InputSource>;
};

inline bool operator!=(InputSource in1, InputSource in2) { return !operator==(in1, in2); }
inline bool operator>(InputSource in1, InputSource in2)  { return  operator<(in2, in1); }
inline bool operator<=(InputSource in1, InputSource in2) { return !operator<(in2, in1); }
inline bool operator>=(InputSource in1, InputSource in2) { return !operator<(in1, in2); }

namespace std
{
    template <> struct hash<InputSource>
    {
        size_t operator()(const InputSource& in) const noexcept
        {
            return std::hash<uint8_t>()((uint8_t)in.type) << (21 - (uint8_t)in.type)
                 ^ std::hash<uint32_t>()((uint32_t)in.attribute);
        }
    };
}
