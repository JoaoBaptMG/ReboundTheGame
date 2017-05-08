#include "FilesystemResourceLocator.hpp"
#include <cstdio>
#include <vector>
#include <algorithm>

#if _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

std::string getResourceDirectory()
{
	static std::string dir;

	if (dir.empty())
	{
		std::vector<char> buffer;

		HMODULE hCurModule = GetModuleHandleA(NULL);

		DWORD size = 8;
		do
		{
			size *= 2;
			buffer.resize(size, 0);
			size = GetModuleFileNameA(hCurModule, buffer.data(), size);
		} while (size == buffer.size());

		if (size == 0) return std::string{};

		std::replace(buffer.begin(), buffer.end(), '\\', '/');
		for (; size > 1; size--)
			if (buffer[size] == '/') break;

		dir = std::string(buffer.data(), size) + "/Resources";
	}

	return dir;
}

#elif __APPLE__
#elif __linux__
extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
}

std::string getResourceDirectory()
{
    static std::string dir;

    if (dir.empty())
    {
        std::vector<char> buffer;

        intmax_t size = 8;
        do
        {
            size *= 2;
            buffer.resize(size, 0);
            size = readlink("/proc/self/exe", buffer.data(), buffer.size());
        } while (size == buffer.size());

        if (size < 0) return std::string{};

        for (; size > 1; size--)
            if (buffer[size] == '/') break;

        dir = std::string(buffer.data(), size) + "/Resources";
    }

    return dir;
}
#endif

std::unique_ptr<sf::InputStream> FilesystemResourceLocator::getResource(std::string name)
{
    std::string fullname = basename + '/' + name;

    std::unique_ptr<sf::FileInputStream> ptr{new sf::FileInputStream()};
    if (!ptr->open(fullname))
        throw FileNotFound(name);

    return std::unique_ptr<sf::InputStream>{ptr.release()};
}
