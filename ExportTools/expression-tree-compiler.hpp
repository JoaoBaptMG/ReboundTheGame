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

