//
// Created by chineseblack23 on 2024/6/22.
//词法分析器
//

#ifndef APOLLO_PARSER_HPP
#define APOLLO_PARSER_HPP

#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include "AbstractSyntaxTree.hpp"
#include "apollo.hpp"

class Parser {
public:
    explicit Parser(const std::string &fileName);

    ~Parser();

public:
    void parse(apollo::Runtime *rt);

    static void printLex(const std::string &fileName);

    short precedence(Token op);

private:
    Expression *parsePrimaryExpr();

    Expression *parseUnaryExpr();

    Expression *parseExpression(
            short oldPrecedence = 1 /*default to lowest precedence(0) + 1*/);

    ExpressionStmt *parseExpressionStmt();

    IfStmt *parseIfStmt();

    WhileStmt *parseWhileStmt();

    ReturnStmt *parseReturnStmt();

    Statement *parseStatement();

    std::vector<Statement *> parseStatementList();

    struct BlockStatement *parseBlock();

    std::vector<std::string> parseParameterList();

    apollo::FunctionDeclaration *parseFuncDef(apollo::Context *context);

private:
    std::tuple<Token, std::string> next();

    inline char getNextChar() {
        end++;
        return static_cast<char>(fs.get());
    }

    inline char peekNextChar() { return static_cast<char>(fs.peek()); }

    inline Token getCurrentToken() const {
        return std::get<Token>(currentToken);
    }

    inline std::string getCurrentLexeme() const {
        return std::get<std::string>(currentToken);
    }

private:
    const std::map<std::string, Token> keywords;

    std::tuple<Token, std::string> currentToken;

    std::fstream fs;

    int start = 1;

    int end = 0;
};


#endif //APOLLO_PARSER_HPP
