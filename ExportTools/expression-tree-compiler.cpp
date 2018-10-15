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

#include "expression-tree-compiler.hpp"

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cstdlib>
#include "varlength.hpp"

#define PRED(expr) ([] (auto c) { return (expr); })

static void ensureASCII(const std::string& str)
{
    if (!std::all_of(str.begin(), str.end(), PRED(c <= 127)))
        throw std::runtime_error("Expression not in ASCII format!");
}

struct Token
{
    ExpressionTree::Type type;
    intmax_t number;
    size_t parDepth;
};

std::vector<Token> lexExpression(std::string expr);
std::unique_ptr<ExpressionTree> parseTokenList(const std::vector<Token>& tokens);

std::unique_ptr<ExpressionTree> compileExpression(std::string expr)
{
    return parseTokenList(lexExpression(expr));
}

std::vector<Token> lexExpression(std::string expr)
{
    using namespace std;
    
    ensureASCII(expr);
    
    auto isDigit = PRED(c >= '0' && c <= '9');
    auto isLetter = PRED(c == 'x' || c == 'X');
    auto isAcceptedSymbol = [](char c)
    {
        const char sym[] = "!%&()*+-/<=>^|";
        const auto symend = sym + sizeof(sym)/sizeof(char);
        return binary_search(sym, symend, c);
    };
    auto isSpace = PRED(c == ' ' || (c >= 0x9 && c <= 0xD));
    
    const char* cur = expr.data();
    const char* end = expr.data() + expr.size();
    
    size_t curParDepth = 0;
    std::vector<Token> tokens;
    
    enum { None, Left, Right } parenMatch = None;
    while (cur != end)
    {
        cur = find_if_not(cur, end, isSpace);
        if (cur == end) break;
        
        if (isDigit(*cur))
        {
            parenMatch = None;
            char* pcur;
            tokens.push_back({ ExpressionTree::Type::Number, strtoll(cur, &pcur, 0), curParDepth });
            cur = pcur;
            if (errno == ERANGE)
            {
                errno = 0;
                throw runtime_error("Too big a number to be parsed!");
            }
        }
        else if (isLetter(*cur))
        {
            parenMatch = None;
            tokens.push_back({ ExpressionTree::Type::Letter, 0, curParDepth });
            cur++;
        }
        else if (isAcceptedSymbol(*cur))
        {
            ExpressionTree::Type ty = ExpressionTree::Type::Number;
            switch (*cur)
            {
                case '+': ty = ExpressionTree::Type::Add; break;
                case '-': ty = ExpressionTree::Type::Subtract; break;
                case '*': ty = ExpressionTree::Type::Multiply; break;
                case '/': ty = ExpressionTree::Type::Divide; break;
                case '%': ty = ExpressionTree::Type::Modulo; break;
                
                case '!': ty = ExpressionTree::Type::NotEqual; break;
                case '=': ty = ExpressionTree::Type::Equal; break;
                case '<': ty = ExpressionTree::Type::Less; break;
                case '>': ty = ExpressionTree::Type::Greater; break;
                
                case '&': ty = ExpressionTree::Type::And; break;
                case '|': ty = ExpressionTree::Type::Or; break;
                case '^': ty = ExpressionTree::Type::Xor; break;
                
                case '(':
                    if (parenMatch == Right)
                        throw runtime_error("Invalid left parenthesis after right parenthesis!");
                    if (!tokens.empty() && (tokens.back().type == ExpressionTree::Type::Number ||
                        tokens.back().type == ExpressionTree::Type::Letter))
                        throw runtime_error(std::string("Invalid left parenthesis after ") +
                            (tokens.back().type == ExpressionTree::Type::Number ? "number" : "letter") + "!");
                    if (curParDepth == (size_t)-2)
                        throw runtime_error("Parenthesis limit reached!");
                    curParDepth++; break;
                    
                case ')':
                    if (parenMatch == Left)
                        throw runtime_error("Invalid right parenthesis after left parenthesis!");
                    if (!tokens.empty() && (tokens.back().type != ExpressionTree::Type::Number &&
                        tokens.back().type != ExpressionTree::Type::Letter))
                        throw runtime_error("Invalid right parenthesis after operator!");
                    if (curParDepth == 0)
                        throw runtime_error("Right parenthesis without matching left parenthesis!");
                    curParDepth--; break;
            }
            
            parenMatch = *cur == '(' ? Left : *cur == ')' ? Right : None;
            
            if (ty != ExpressionTree::Type::Number)
                tokens.push_back({ ty, 0, curParDepth });
            cur++;
        }
        else throw runtime_error(std::string("Unrecognized character: ") + *cur);
    }
    
    if (curParDepth > 0)
        throw runtime_error("Left parenthesis without matching right parenthesis!");
        
    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].parDepth == (size_t)-1) continue;
        
        switch (tokens[i].type)
        {
            case ExpressionTree::Type::Less:
            case ExpressionTree::Type::Greater:
                if (i < tokens.size()-1 && tokens[i+1].type == ExpressionTree::Type::Equal)
                {
                    tokens[i].type = tokens[i].type == ExpressionTree::Type::Less ?
                        ExpressionTree::Type::LessEqual : ExpressionTree::Type::GreaterEqual;
                    tokens[i+1].parDepth = (size_t)-1;
                } break;
                
            case ExpressionTree::Type::Equal:
            case ExpressionTree::Type::NotEqual:
                if (i < tokens.size()-1 && tokens[i+1].type == ExpressionTree::Type::Equal)
                    tokens[i+1].parDepth = (size_t)-1;
                else if (tokens[i].type == ExpressionTree::Type::Equal)
                    throw runtime_error("Unrecognized lone '=' symbol!");
                else throw runtime_error("Unrecognized lone '!' symbol!");
                break;
                
            case ExpressionTree::Type::And:
                if (i < tokens.size()-1 && tokens[i+1].type == ExpressionTree::Type::And)
                    tokens[i+1].parDepth = (size_t)-1;
                else
                    throw runtime_error("Unrecognized & without second &!");
                break;
                    
            case ExpressionTree::Type::Or:
                if (i < tokens.size()-1 && tokens[i+1].type == ExpressionTree::Type::Or)
                    tokens[i+1].parDepth = (size_t)-1;
                else
                    throw runtime_error("Unrecognized | without second |!");
                break;
            
            default: break;
        }
    }
    
    tokens.erase(remove_if(tokens.begin(), tokens.end(), PRED(c.parDepth == (size_t)-1)), tokens.end());
    
    return tokens;
}

ExpressionTree* recursiveParse(const Token* first, const Token* last);

std::unique_ptr<ExpressionTree> parseTokenList(const std::vector<Token>& tokens)
{
    using namespace std;
    
    if (tokens.front().type != ExpressionTree::Type::Number &&
        tokens.front().type != ExpressionTree::Type::Letter)
        throw runtime_error("Expression must begin with x or a number!");
        
    if (tokens.back().type != ExpressionTree::Type::Number &&
        tokens.back().type != ExpressionTree::Type::Letter)
        throw runtime_error("Expression must end with x or a number!");
        
    for (size_t i = 0; i < tokens.size()-1; i++)
    {
        bool issymbol0 = tokens[i].type != ExpressionTree::Type::Number &&
                         tokens[i].type != ExpressionTree::Type::Letter;
        bool issymbol1 = tokens[i+1].type != ExpressionTree::Type::Number &&
                         tokens[i+1].type != ExpressionTree::Type::Letter;
                         
        if (issymbol0 && issymbol1)
            throw runtime_error("Invalid succession of two operators!");
        else if (!issymbol0 && !issymbol1)
            throw runtime_error("Invalid succession of two non-operators!");
    }
    
    auto tree = recursiveParse(&tokens.front(), &tokens.back());
    
    switch (tree->getType())
    {
        case ExpressionTree::Type::Number:
        case ExpressionTree::Type::Letter:
        case ExpressionTree::Type::Add:
        case ExpressionTree::Type::Subtract:
        case ExpressionTree::Type::Multiply:
        case ExpressionTree::Type::Divide:
        case ExpressionTree::Type::Modulo:
            throw runtime_error("First parsed operator must be a logical operator!");
        default: break;
    }
    
    return std::unique_ptr<ExpressionTree>(tree);
}

auto opString(ExpressionTree::Type type)
{
    switch (type)
    {
        case ExpressionTree::Type::Add:          return "+";
        case ExpressionTree::Type::Subtract:     return "-";
        
        case ExpressionTree::Type::Multiply:     return "*";
        case ExpressionTree::Type::Divide:       return "/";
        case ExpressionTree::Type::Modulo:       return "%";
        
        case ExpressionTree::Type::Equal:        return "==";
        case ExpressionTree::Type::NotEqual:     return "!=";
        
        case ExpressionTree::Type::Less:         return "<";
        case ExpressionTree::Type::Greater:      return ">";
        case ExpressionTree::Type::LessEqual:    return "<=";
        case ExpressionTree::Type::GreaterEqual: return ">=";
        
        case ExpressionTree::Type::And:          return "&&";
        case ExpressionTree::Type::Or:           return "||";
        case ExpressionTree::Type::Xor:          return "^";
        
        default:                             return "";
    }
}

ExpressionTree* recursiveParse(const Token* first, const Token* last)
{
    if (first == last)
        return new ExpressionTree(first->type, first->number);
    
    auto opPrecedence = [](ExpressionTree::Type type)
    {
        switch (type)
        {
            case ExpressionTree::Type::Add:          return 6;
            case ExpressionTree::Type::Subtract:     return 6;
            
            case ExpressionTree::Type::Multiply:     return 7;
            case ExpressionTree::Type::Divide:       return 7;
            case ExpressionTree::Type::Modulo:       return 7;
            
            case ExpressionTree::Type::Equal:        return 4;
            case ExpressionTree::Type::NotEqual:     return 4;
            
            case ExpressionTree::Type::Less:         return 5;
            case ExpressionTree::Type::Greater:      return 5;
            case ExpressionTree::Type::LessEqual:    return 5;
            case ExpressionTree::Type::GreaterEqual: return 5;
            
            case ExpressionTree::Type::And:          return 2;
            case ExpressionTree::Type::Or:           return 1;
            case ExpressionTree::Type::Xor:          return 3;
            
            default:                             return 0;
        }
    };
    
    enum { Arithmetic, Conditional, Logic };
    auto opType = [](ExpressionTree::Type type)
    {
        switch (type)
        {
            case ExpressionTree::Type::Equal:
            case ExpressionTree::Type::NotEqual:
            case ExpressionTree::Type::Less:
            case ExpressionTree::Type::Greater:
            case ExpressionTree::Type::LessEqual:
            case ExpressionTree::Type::GreaterEqual:
                return Conditional;
            
            case ExpressionTree::Type::And:
            case ExpressionTree::Type::Or:
            case ExpressionTree::Type::Xor:
                return Logic;
            
            default: return Arithmetic;
        }
    };
    
    // based on https://artofproblemsolving.com/community/c163h141553p800759
    auto precedenceLess = [&](const Token& t1, const Token& t2)
    {
        if (t1.parDepth == t2.parDepth)
            return opPrecedence(t1.type) < opPrecedence(t2.type);
        return t1.parDepth < t2.parDepth;
    };
    
    auto leastToken = first+1;
    for (auto cur = first+1; cur < last; cur += 2)
    {
        if (precedenceLess(*cur, *leastToken))
            leastToken = cur;
    }
    
    auto ret = new ExpressionTree(leastToken->type,
        recursiveParse(first, leastToken-1),
        recursiveParse(leastToken+1, last));
    
    auto type = opType(ret->getType());
    auto type1 = opType(ret->getTreeNodeLeft()->getType());
    auto type2 = opType(ret->getTreeNodeRight()->getType());
        
    switch (type)
    {
        case Arithmetic:
        case Conditional:
            if (type1 != Arithmetic || type2 != Arithmetic)
                throw std::runtime_error(std::string("Type mismatch in operator '") + opString(ret->getType())
                    + "': both operands must be numbers!");
            break;
        
        case Logic:
            if (type1 == Arithmetic || type2 == Arithmetic)
                throw std::runtime_error(std::string("Type mismatch in operator '") + opString(ret->getType())
                    + "': both operands must NOT be numbers!");
            break;
    }
        
    return ret;
}

void printTree(const ExpressionTree& tree)
{
    using namespace std;
    
    switch (tree.getType())
    {
        case ExpressionTree::Type::Number: std::cout << tree.getNumber(); break;
        case ExpressionTree::Type::Letter: std::cout << 'x'; break;
        default:
            printTree(*tree.getTreeNodeLeft());
            std::cout << ", ";
            printTree(*tree.getTreeNodeRight());
            std::cout << ", ";
            std::cout << opString(tree.getType());
    }
}

void generateTreeOpcodes(std::vector<size_t> &opcodes, const ExpressionTree& tree)
{
    if (tree.getType() == ExpressionTree::Type::Letter)
        opcodes.push_back(14);
    else if (tree.getType() == ExpressionTree::Type::Number)
        opcodes.push_back(15 + tree.getNumber());
    else
    {
        generateTreeOpcodes(opcodes, *tree.getTreeNodeLeft());
        generateTreeOpcodes(opcodes, *tree.getTreeNodeRight());
        
        size_t id = 0;
        switch (tree.getType())
        {
            case ExpressionTree::Type::Add:          id = 0; break;
            case ExpressionTree::Type::Subtract:     id = 1; break;
            case ExpressionTree::Type::Multiply:     id = 2; break;
            case ExpressionTree::Type::Divide:       id = 3; break;
            case ExpressionTree::Type::Modulo:       id = 4; break;
            case ExpressionTree::Type::Equal:        id = 5; break;
            case ExpressionTree::Type::NotEqual:     id = 6; break;
            case ExpressionTree::Type::Less:         id = 7; break;
            case ExpressionTree::Type::Greater:      id = 8; break;
            case ExpressionTree::Type::LessEqual:    id = 9; break;
            case ExpressionTree::Type::GreaterEqual: id = 10; break;
            case ExpressionTree::Type::And:          id = 11; break;
            case ExpressionTree::Type::Or:           id = 12; break;
            case ExpressionTree::Type::Xor:          id = 13; break;
            default: break;
        }
        opcodes.push_back(id);
    }
}

void writeExpressionTree(std::ostream& out, const ExpressionTree& tree)
{
    std::vector<size_t> opcodes;
    
    generateTreeOpcodes(opcodes, tree);
    
    write_varlength(out, opcodes.size());
    for (auto opcode : opcodes) write_varlength(out, opcode);
}
