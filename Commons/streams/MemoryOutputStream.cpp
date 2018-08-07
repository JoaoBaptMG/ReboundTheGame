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


#include "MemoryOutputStream.hpp"

#include <algorithm>
#include <cstring>

bool MemoryOutputStream::write(const void* data, uint64_t size)
{
    try
    {
        auto prevSize = contents.size();
        contents.resize(prevSize + size);
		std::copy_n((const char*)data, size, contents.begin() + prevSize);
        return true;
    }
    catch (std::bad_alloc&)
    {
        return false;
    }
}

void MemoryOutputStream::alignTo(uint64_t align)
{
    // PKCS7 alignment
    auto newSize = ((contents.size() + align - 1) / align) * align;
    auto diff = newSize - contents.size();
    contents.resize(newSize, diff);
}
