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

