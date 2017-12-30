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
