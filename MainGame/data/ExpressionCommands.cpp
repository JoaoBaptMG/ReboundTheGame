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

#include "ExpressionCommands.hpp"

#include <streamReaders.hpp>
#include <algorithm>

bool readFromStream(InputStream &stream, ExpressionCommands& cmds)
{
    size_t size;
    if (!readFromStream(stream, varLength(size))) return false;

    cmds.opcodes.resize(size);
    return std::all_of(cmds.opcodes.begin(), cmds.opcodes.end(),
    [&] (size_t& opcode) { return readFromStream(stream, varLength(opcode)); });
}

intmax_t runExpression(const ExpressionCommands& cmds, size_t x)
{
    if (cmds.opcodes.empty()) return 0;
    
    // This is to preserve memory and make execution faster
    static std::vector<intmax_t> stack;
    stack.clear();
    
    for (auto opcode : cmds.opcodes)
    {
        if (opcode >= 15) stack.push_back(opcode - 15);
        else if (opcode == 14) stack.push_back(x);
        else
        {
            if (stack.size() <= 1) return 0;
            
            intmax_t a = stack[stack.size()-2];
            intmax_t b = stack[stack.size()-1];
            stack.pop_back();
            
            switch (opcode)
            {
                case 0: stack.back() = a + b; break;
                case 1: stack.back() = a - b; break;
                case 2: stack.back() = a * b; break;
                case 3: if (b != 0) stack.back() = a / b;
                    else return 0; break;
                case 4: if (b != 0) stack.back() = a % b;
                    else return 0; break;
                case 5: stack.back() = a == b; break;
                case 6: stack.back() = a != b; break;
                case 7: stack.back() = a < b; break;
                case 8: stack.back() = a > b; break;
                case 9: stack.back() = a <= b; break;
                case 10: stack.back() = a >= b; break;
                case 11: stack.back() = a && b; break;
                case 12: stack.back() = a || b; break;
                case 13: stack.back() = (a != 0) != (b != 0); break;
            }
        }
    }
    
    return stack[0];
}

#ifndef NDEBUG

#include <string>
#include <sstream>

std::string ExpressionCommands::prettyPrint()
{
    bool nf = false;
    
    std::stringstream ss;
    for (size_t op : opcodes)
    {
        if (nf) ss << ", ";
        
        if (op >= 15) ss << (op - 15);
        else if (op == 14) ss << 'x';
        else
        {
            switch (op)
            {
                case 0: ss << "+"; break;
                case 1: ss << "-"; break;
                case 2: ss << "*"; break;
                case 3: ss << "/"; break;
                case 4: ss << "%"; break;
                case 5: ss << "=="; break;
                case 6: ss << "!="; break;
                case 7: ss << "<"; break;
                case 8: ss << ">"; break;
                case 9: ss << "<="; break;
                case 10: ss << ">="; break;
                case 11: ss << "&&"; break;
                case 12: ss << "||"; break;
                case 13: ss << "^"; break;
            }
        }
    }
    
    return ss.str();
}

#endif
