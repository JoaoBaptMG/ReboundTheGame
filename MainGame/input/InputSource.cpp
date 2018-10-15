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

#include "InputSource.hpp"

#include <algorithm>
#include "language/KeyboardKeyName.hpp"

constexpr auto MaxWheels = 1 + std::max(sf::Mouse::HorizontalWheel, sf::Mouse::VerticalWheel);

InputSource InputSource::none{InputSource::Type::None, 0};

InputSource InputSource::mouseX{InputSource::Type::MouseAxis, 2 * MaxWheels };
InputSource InputSource::mouseY{InputSource::Type::MouseAxis, 2 * MaxWheels + 1};
InputSource InputSource::invertedMouseX{InputSource::Type::MouseAxis, 2 * MaxWheels + 2};
InputSource InputSource::invertedMouseY{InputSource::Type::MouseAxis, 2 * MaxWheels + 3};

InputSource InputSource::keyboardKey(size_t scanCode)
{
    return InputSource(Type::Keyboard, scanCode);
}

InputSource InputSource::mouseButton(sf::Mouse::Button button)
{
    return InputSource(Type::MouseButton, (size_t)button);
}

InputSource InputSource::mouseWheel(sf::Mouse::Wheel wheel)
{
    return InputSource(Type::MouseAxis, (size_t)wheel);
}

InputSource InputSource::invertedMouseWheel(sf::Mouse::Wheel wheel)
{
    return InputSource(Type::MouseAxis, MaxWheels + (size_t)wheel);
}

InputSource InputSource::joystickButton(unsigned int button)
{
    return InputSource(Type::JoystickButton, (size_t)button);
}

InputSource InputSource::joystickAxis(sf::Joystick::Axis axis)
{
    return InputSource(Type::JoystickAxis, (size_t)axis);
}

InputSource InputSource::invertedJoystickAxis(sf::Joystick::Axis axis)
{
    return InputSource(Type::JoystickAxis, sf::Joystick::AxisCount + (size_t)axis);
}

bool operator==(InputSource in1, InputSource in2)
{
    return in1.type == in2.type && in1.attribute == in2.attribute;
}

bool operator<(InputSource in1, InputSource in2)
{
    return in1.type == in2.type ? in1.attribute < in2.attribute : in1.type < in2.type;
}

bool readFromStream(sf::InputStream& stream, InputSource& in)
{
    return readFromStream(stream, in.type, varLength(in.attribute));
}

bool writeToStream(OutputStream& stream, const InputSource& in)
{
    return writeToStream(stream, in.type, varLength(in.attribute));
}

std::string InputSource::getInputName(LocalizationManager& lm)
{
    switch (type)
    {
        case Type::None: return lm.getString("input-source-none");
        case Type::Keyboard: return scanCodeToKeyName(attribute, lm);
        case Type::JoystickButton:
            return lm.getFormattedString("joystick-button-id", {}, {{"n",attribute}}, {});
        case Type::JoystickAxis:
        {
            if (attribute >= sf::Joystick::AxisCount)
                return lm.getFormattedString("joystick-inv-axis-id", {}, {{"n",attribute-sf::Joystick::AxisCount}}, {});
            return lm.getFormattedString("joystick-axis-id", {}, {{"n",attribute}}, {});
        }
    }
    return "";
}
