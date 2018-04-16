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

#if _WIN32
#include <typeindex>
#endif

namespace util
{
#if _WIN32
	using type_id_t = std::type_index;

	template <typename T>
	type_id_t type_id() { return std::type_index(typeid(T));  }
#else
    ///
    /// \brief Class to encapsulate type_id
    /// Its implementation is by iavr (http://codereview.stackexchange.com/users/39083/iavr)
    /// and its code can be found on the following link:
    /// http://codereview.stackexchange.com/questions/48594/unique-type-id-no-rtti
    ///
    class type_id_t
    {
        using sig = type_id_t();

        sig* id;
        type_id_t(sig* id) : id{id} {}

    public:
        template<typename T>
        friend type_id_t type_id();

        bool operator==(type_id_t o) const { return id == o.id; }
        bool operator!=(type_id_t o) const { return id != o.id; }
    };

    ///
    /// \brief This function is guaranteed to return an ID unique to each type
    ///
    template<typename T>
    inline type_id_t type_id() { return &type_id<T>; }
#endif

    const type_id_t notype = type_id<void>();
}
