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

#include <cstdint>
#include <algorithm>

class Memory
{
    uint8_t* ptr;
    size_t size;

public:
    inline Memory() : ptr(nullptr), size(0) {}
    inline Memory(size_t size) : ptr(new uint8_t[size]), size(size) {}
    inline Memory(const Memory &other) : Memory(other.size)
    {
        std::copy(other.ptr, other.ptr+size, ptr);
    }
    inline Memory(Memory &&other) noexcept : Memory() { swap(*this, other); }

    inline Memory& operator=(Memory other) noexcept
    {
        swap(*this, other);
        return *this;
    }

    inline ~Memory() { delete[] ptr; }

    inline const void* get_ptr() const { return ptr; }
    inline void* get_ptr() { return ptr; }
    inline size_t get_size() const { return size; }

    friend inline void swap(Memory &m1, Memory &m2) noexcept
    {
        using std::swap;
        swap(m1.ptr, m2.ptr);
        swap(m1.size, m2.size);
    }
};

