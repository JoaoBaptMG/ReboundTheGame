#pragma once

#include <SFML/Window.hpp>
#include <type_traits>
#include <functional>
#include <streamReaders.hpp>
#include <streamWriters.hpp>

class InputSource final
{
private:
    enum class Type : uint8_t { Keyboard, MouseButton, MouseAxis, JoystickButton, JoystickAxis };

    Type type;
    uint32_t attribute;

    // Constructor private
    InputSource() = default;
    InputSource(Type type, uint32_t attr) : type(type), attribute(attr) {}
public:
    static InputSource keyboardKey(sf::Keyboard::Key key);
    
    static InputSource mouseButton(sf::Mouse::Button buton);
    static InputSource mouseX, mouseY;
    static InputSource mouseWheel(sf::Mouse::Wheel wheel);

    static InputSource joystickButton(unsigned int joystick, unsigned int button);
    static InputSource joystickAxis(unsigned int joystick, sf::Joystick::Axis axis);

    friend bool operator==(InputSource in1, InputSource in2);
    friend bool operator<(InputSource in1, InputSource in2);

    friend bool readFromStream(sf::InputStream& stream, InputSource& in);
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
            return (std::hash<uint8_t>()((uint8_t)in.type) << (21 - (uint8_t)in.type))
                 ^ std::hash<uint32_t>()(in.attribute);
        }
    };
}
