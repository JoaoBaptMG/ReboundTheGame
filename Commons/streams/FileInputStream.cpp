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


#include "FileInputStream.hpp"

FileInputStream::~FileInputStream()
{
    if (file) std::fclose(file);
}

bool FileInputStream::open(const std::string& filename)
{
    if (file) std::fclose(file);
    return (file = std::fopen(filename.c_str(), "rb")) != nullptr;
}

uint64_t FileInputStream::read(void* data, uint64_t size)
{
    if (!file) return 0;
    return std::fread(data, 1, size, file);
}

uint64_t FileInputStream::size()
{
	if (!file) return -1;
	auto last = std::ftell(file);
	std::fseek(file, 0, SEEK_END);
	auto sz = std::ftell(file);
	std::fseek(file, last, SEEK_CUR);
	return sz;
}

uint64_t FileInputStream::seek(uint64_t pos)
{
	if (!file) return -1;
	std::fseek(file, pos, SEEK_SET);
	return std::ftell(file);
}

uint64_t FileInputStream::tell()
{
	if (!file) return -1;
	return std::ftell(file);
}
