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
#include <iostream>

class ExpressionTree final
{
public:
    enum class Type
    {
        Number, Letter,
        Add, Subtract, Multiply, Divide, Modulo,
        Equal, NotEqual, Less, Greater, LessEqual, GreaterEqual,
        And, Or, Xor
    };
    
private:
    Type type;
    std::unique_ptr<ExpressionTree> treeNodeLeft = nullptr, treeNodeRight = nullptr;
    intmax_t number = 0;
    
public:
    ExpressionTree(Type type, intmax_t number) : type(type), number(number) {}
    ExpressionTree(Type type, ExpressionTree* left, ExpressionTree* right)
    : type(type), treeNodeLeft(left), treeNodeRight(right) {}
    ~ExpressionTree() {}
    
    auto getType() const { return type; }
    auto getTreeNodeLeft() const { return treeNodeLeft.get(); }
    auto getTreeNodeRight() const { return treeNodeRight.get(); }
    auto getNumber() const { return number; }
};

std::unique_ptr<ExpressionTree> compileExpression(std::string expr);
void printTree(const ExpressionTree& tree);
void writeExpressionTree(std::ostream& out, const ExpressionTree& tree);

