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
            return lm.getFormattedString("joystick-button-id", {}, {{"n",attribute+1}}, {});
        case Type::JoystickAxis:
        {
            if (attribute >= sf::Joystick::AxisCount)
                return lm.getFormattedString("joystick-inv-axis-id", {}, {{"n",attribute-sf::Joystick::AxisCount+1}}, {});
            return lm.getFormattedString("joystick-axis-id", {}, {{"n",attribute+1}}, {});
        }
    }
    return "";
}
