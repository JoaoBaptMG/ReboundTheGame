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


#include "execDir.hpp"
#include <cstdio>
#include <vector>
#include <algorithm>

#if _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C"
{
	// Make NVIDIA and AMD run us on dedicated card
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

std::string getExecutableDirectory()
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

		dir = std::string(buffer.data(), size);
	}

	return dir;
}

std::vector<std::string> getAllFilesInDir(std::string dir)
{
	dir += "\\*";

	std::vector<std::string> list;

	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFileA(dir.c_str(), &data);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			std::string name(data.cFileName);

			auto pos = name.find_last_of("\\/");
			if (pos == std::string::npos)
				pos = decltype(pos)(0);
			else pos++;

			list.emplace_back(name.substr(pos));
		}
		while (FindNextFileA(hFind, &data));

		FindClose(hFind);
	}

	return list;
}

#elif __APPLE__

#elif __linux__
extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
}

std::string getExecutableDirectory()
{
    static std::string dir;

    if (dir.empty())
    {
        std::vector<char> buffer;

        ssize_t size = 8;
        do
        {
            size *= 2;
            buffer.resize(size, 0);
            size = readlink("/proc/self/exe", buffer.data(), size);
        } while (size == buffer.size());

        if (size < 0) return std::string{};

        for (; size > 1; size--)
            if (buffer[size] == '/') break;

        dir = std::string(buffer.data(), size);
    }

    return dir;
}

std::vector<std::string> getAllFilesInDir(std::string dir)
{
    DIR* cur = opendir(dir.data());
    if (!cur) return std::vector<std::string>{};
    
    struct dirent* curEnt;
    std::vector<std::string> list;
    
    while ((curEnt = readdir(cur)))
    {
        std::string name(curEnt->d_name);
        
        auto pos = name.find_last_of("\\/");
        if (pos == std::string::npos)
            pos = decltype(pos)(0);
        else pos++;
        
        list.emplace_back(name.substr(pos));
    }
        
    closedir(cur);
    
    return list;
}
#endif
