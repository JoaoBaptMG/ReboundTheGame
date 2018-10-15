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


#include "varlength.hpp"

#include <stack>

using namespace std;

size_t write_varlength(ostream& out, size_t val)
{
    stack<uint8_t> chunks;

    do
    {
        chunks.push(val % 128);
        val /= 128;
    } while (val);

    size_t ret = chunks.size() * sizeof(uint8_t);

    while (!chunks.empty())
    {
        uint8_t b = chunks.top();
        chunks.pop();

        if (!chunks.empty()) b += 128;
        out.put(b);
    }

    return ret;
} 

size_t read_varlength(std::istream& in)
{
    size_t result = 0;

    uint8_t curByte = 128;
    while (curByte & 128)
    {
        curByte = in.get();
        result = result * 128 + curByte % 128;
    }
    
    return result;
}
