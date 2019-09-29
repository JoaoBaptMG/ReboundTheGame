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

#include <memory>
#include <string>
#include <stdexcept>
#include "type_id.hpp"

namespace util
{
    class type_mismatch : public std::runtime_error
    {
    public:
        const char *type_name;

        type_mismatch(const char* type_name, std::string text)
            : std::runtime_error(text), type_name(type_name) {}

        template <typename T>
        static type_mismatch for_type()
        {
            return type_mismatch(typeid(T).name(),
                std::string("Attempt to cast an incompatible generic_shared_ptr "
                "to type ") + typeid(T).name());
        }
    };

    ///
    /// \brief Represents a shared pointer that can be used to store anything
    ///
    class generic_shared_ptr
    {
        type_id_t type;
        std::shared_ptr<void> ptr;

    public:
        /// \brief Constructs an empty generic_shared_ptr
        generic_shared_ptr() : type(notype), ptr() {}

        /// \brief Constructs a generic_shared_ptr from an existing shared_ptr
        template <typename T>
        generic_shared_ptr(const std::shared_ptr<T> &other)
        : type(type_id<T>()), ptr(other) {}

        /// \brief Constructs a generic_shared_ptr from an existing shared_ptr
        template <typename T>
        generic_shared_ptr(std::shared_ptr<T> &&other)
        : type(type_id<T>()), ptr(other) {}

        friend inline void swap(generic_shared_ptr &p1, generic_shared_ptr &p2)
        {
            using std::swap;
            swap(p1.type, p2.type);
            swap(p1.ptr, p2.ptr);
        }

        /// \brief Resets the pointer
        inline void reset()
        {
            type = notype;
            ptr.reset();
        }

        inline long use_count() const
        {
            return ptr.use_count();
        }

        inline explicit operator bool() const
        {
            return type != notype && static_cast<bool>(ptr);
        }

        template <typename T>
        bool is()
        {
            return type == type_id<T>();
        }

        bool empty() { return type == notype; }

        template <typename T>
        std::shared_ptr<T> try_convert()
        {
            if (is<T>())
                return std::static_pointer_cast<T>(ptr);
            else
                return std::shared_ptr<T>();
        }

        template <typename T>
        std::shared_ptr<T> as()
        {
            if (auto p = try_convert<T>())
                return p;
            else
                throw type_mismatch::for_type<T>();
        }

        template <typename T>
        inline explicit operator std::shared_ptr<T>()
        {
            return as<T>();
        }
    };
}
