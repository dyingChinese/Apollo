//
// Created by chineseblack23 on 2024/6/22.
//

#ifndef APOLLO_INTERPRETER_HPP
#define APOLLO_INTERPRETER_HPP

#include <deque>
#include <string>
#include "apollo.hpp"
#include "Parser.hpp"

using namespace std;

class Interpreter {
public:
    explicit Interpreter(const string &fileName);

    ~Interpreter();

public:
    void execute();

public:
    static void enterContext(std::deque<apollo::Context *> &ctxChain);

    static void leaveContext(std::deque<apollo::Context *> &ctxChain);

    static apollo::ValueDeclaration callFunction(apollo::Runtime *rt, apollo::FunctionDeclaration *f,
                                                 std::deque<apollo::Context *> previousCtxChain,
                                                 std::vector<Expression *> args);

    static apollo::ValueDeclaration
    calcBinaryExpr(ValueDeclaration lhs, Token opt, ValueDeclaration rhs,
                   int line, int column);

    static apollo::ValueDeclaration calcUnaryExpr(ValueDeclaration &lhs, Token opt, int line,
                                                  int column);

    static apollo::ValueDeclaration assignSwitch(Token opt, ValueDeclaration lhs, ValueDeclaration rhs);

private:
    void parseCommandOption(int argc, char *argv) {}

private:
    std::deque<apollo::Context *> ctxChain;
    apollo::Runtime *rt;
    Parser *p;
};


#endif //APOLLO_INTERPRETER_HPP
