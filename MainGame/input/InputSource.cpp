#include "InputSource.hpp"

#include <algorithm>

constexpr auto MaxWheels = std::max(sf::Mouse::HorizontalWheel, sf::Mouse::VerticalWheel);

InputSource InputSource::mouseX{InputSource::Type::MouseAxis, MaxWheels + 1};
InputSource InputSource::mouseY{InputSource::Type::MouseAxis, MaxWheels + 2};

InputSource InputSource::keyboardKey(sf::Keyboard::Key key)
{
    return InputSource(Type::Keyboard, (uint32_t)key);
}

InputSource InputSource::mouseButton(sf::Mouse::Button button)
{
    return InputSource(Type::MouseButton, (uint32_t)button);
}

InputSource InputSource::mouseWheel(sf::Mouse::Wheel wheel)
{
    return InputSource(Type::MouseAxis, (uint32_t)wheel);
}

InputSource InputSource::joystickButton(unsigned int joystick, unsigned int button)
{
    return InputSource(Type::JoystickButton, (uint32_t)(joystick * sf::Joystick::ButtonCount + button));
}

InputSource InputSource::joystickAxis(unsigned int joystick, sf::Joystick::Axis axis)
{
    return InputSource(Type::JoystickAxis, (uint32_t)(joystick * sf::Joystick::AxisCount + (unsigned int)axis));
}

bool operator==(InputSource in1, InputSource in2)
{
    return in1.type == in2.type && in1.attribute == in2.attribute;
}

bool operator<(InputSource in1, InputSource in2)
{
    return in1.type == in2.type ? in1.attribute < in2.attribute : in1.type < in2.type;
}

bool readFromStream(std::istream& stream, InputSource& in)
{
    return readFromStream(stream, in.type, in.attribute);
}

bool writeToStream(std::ostream& stream, const InputSource& in)
{
    return writeToStream(stream, in.type, in.attribute);
}
