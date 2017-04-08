#include "varlength.hpp"

#include <stack>

using namespace std;

size_t write_varlength(ostream& out, size_t val)
{
    stack<uint8_t> chunks;

    while (val)
    {
        chunks.push(val % 128);
        val /= 128;
    }

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
