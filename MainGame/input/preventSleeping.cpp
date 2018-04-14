#include "preventSleeping.hpp"

#if _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void preventComputerFromSleeping()
{
    SetThreadExecutionState(ES_DISPLAY_REQUIRED|ES_SYSTEM_REQUIRED);
}

#elif __APPLE__

#elif __linux__

#include <X11/Xlib.h>

namespace sf
{
    namespace priv
    {
        Display* OpenDisplay();
        void CloseDisplay(Display* display);
    }
}


void preventComputerFromSleeping()
{
    using namespace sf::priv;

    Display* display = OpenDisplay();
    XResetScreenSaver(display);
    CloseDisplay(display);
}

#endif
