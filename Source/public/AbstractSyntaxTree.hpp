//
// Created by chineseblack23 on 2024/6/20.
//

#ifndef APOLLO_ABSTRACTSYNTAXTREE_HPP
#define APOLLO_ABSTRACTSYNTAXTREE_HPP

#include <deque>
#include <map>
#include "apollo.hpp"

using namespace std;

enum Token {
    INVALID = 0,  // <invalid>

    TK_IDENT,    // <identifier>
    TK_EOF,      // <eof>
    LIT_NUMBER,     // <Number>
    LIT_STRING,     // <string>

    TK_BITAND,  // &
    TK_BITOR,   // |
    TK_BITNOT,  // ~
    TK_LOGAND,  // &&
    TK_LOGOR,   // ||
    TK_LOGNOT,  // !

    TK_PLUS,   // +
    TK_MINUS,  // -
    TK_TIMES,  // *
    TK_DIV,    // /
    TK_MOD,    // %

    TK_EQ,  // ==
    TK_NE,  // !=
    TK_GT,  // >
    TK_GE,  // >=
    TK_LT,  // <
    TK_LE,  // <=

    TK_ASSIGN,     // =
    TK_PLUS_AGN,   // +=
    TK_MINUS_AGN,  // -=
    TK_TIMES_AGN,  // *=
    TK_DIV_AGN,    // /=
    TK_MOD_AGN,    // %=
    TK_COMMA,      // ,
    TK_LPAREN,     // (
    TK_RPAREN,     // )
    TK_LBRACE,     // {
    TK_RBRACE,     // }
    TK_LBRACKET,   // [
    TK_RBRACKET,   // ]

    KW_IF,        // if
    KW_ELSE,      // else
    KW_TRUE,      // true
    KW_FALSE,     // false
    KW_WHILE,     // while
    KW_FOR,       // for
    KW_FOROF,      //for...of..
    KW_NULL,      // null
    KW_FUNC,      // func
    KW_RETURN,    // return
    KW_BREAK,     // break
    KW_CONTINUE,  // continue
};

using apollo::BlockStatement;
using apollo::Context;
using apollo::ExecResult;
using apollo::Runtime;
using apollo::ValueDeclaration;

struct Statement;
struct Expression;


struct AbstractSyntaxTreeNode {
    explicit AbstractSyntaxTreeNode(int start, int end) : start(start), end(end) {};

    virtual ~AbstractSyntaxTreeNode() = default;

    virtual string astString() { return "ASTNode()"; };
    int start;
    int end;
};

/**
 * ==========================================================================
 * =========            Expression                              =============
 * ==========================================================================
 */

struct Expression : public AbstractSyntaxTreeNode {
    using AbstractSyntaxTreeNode::AbstractSyntaxTreeNode;

    virtual ~Expression() = default;

    virtual ValueDeclaration eval(Runtime *runtime, std::deque<Context *> ctxChain);

    string astString() override;
};

struct BooleanExpression : public Expression {
    explicit BooleanExpression(int start, int end) : Expression(start, end) {};

    bool literal;

    ValueDeclaration eval(Runtime *runtime, std::deque<Context *> ctxChain) override;

    string astString() override;
};

struct NullExpression : public Expression {
    explicit NullExpression(int start, int end) : Expression(start, end) {};

    ValueDeclaration eval(Runtime *runtime, std::deque<Context *> ctxChain) override;

    string astString() override;

};

struct NumberExpression : public Expression {
    explicit NumberExpression(int start, int end) : Expression(start, end) {};

    ValueDeclaration eval(Runtime *runtime, std::deque<Context *> ctxChain) override;

    double literal;

    string astString() override;
};

struct StringExpression : public Expression {
    explicit StringExpression(int start, int end) : Expression(start, end) {};

    ValueDeclaration eval(Runtime *runtime, std::deque<Context *> ctxChain) override;

    string literal;

    string astString() override;
};
struct ArrayExpression : public Expression {
    explicit ArrayExpression(int start, int end) : Expression(start, end) {}

    std::vector<Expression*> literal;

    ValueDeclaration eval(Runtime* rt, std::deque<Context*> ctxChain) override;
    std::string astString();
};
struct IdentExpression : public Expression {
    explicit IdentExpression(string identName, int start, int end)
            : Expression(start, end), identName(std::move(identName)) {}

    string identName;

    ValueDeclaration eval(Runtime *runtime, std::deque<Context *> ctxChain) override;

    string astString() override;
};

struct IndexExpression : public Expression {
    explicit IndexExpression(int start, int end) : Expression(start, end) {}

    string identName;
    Expression *index;

    ValueDeclaration eval(Runtime *runtime, std::deque<Context *> ctxChain) override;

    string astString() override;
};

struct BinaryExpression : public Expression {
    explicit BinaryExpression(int start, int end) : Expression(start, end) {}

    Expression *leftExpression{};
    Token opt{};
    Expression *rightExpression{};

    ValueDeclaration eval(Runtime *rt, std::deque<Context *> ctxChain) override;

    std::string astString() override;

};

struct FunCallExpression : public Expression {
    explicit FunCallExpression(int start, int end) : Expression(start, end) {};

    string funName;
    vector<Expression *> args;

    ValueDeclaration eval(Runtime *runtime, std::deque<Context *> ctxChain) override;

    string astString() override;
};

struct AssignExpression : public Expression {
    explicit AssignExpression(int start, int end) : Expression(start, end) {};

    Expression *leftExpression{};
    Expression *rightExperssion{};
    Token opt;

    ValueDeclaration eval(Runtime *runtime, std::deque<Context *> ctxChain) override;

    string astString() override;
};

struct Statement : public AbstractSyntaxTreeNode {
    using AbstractSyntaxTreeNode::AbstractSyntaxTreeNode;

    virtual ~Statement() = default;

    virtual ExecResult interpret(Runtime *runtime, std::deque<Context *> ctxChain);

    string astString() override;
};

struct BreakStmt : public Statement {
    explicit BreakStmt(int start, int end) : Statement(start, end) {};

    ExecResult interpret(Runtime *runtime, std::deque<Context *> ctxChain) override;

    string astString() override;
};

struct ContinueStmt : public Statement {
    explicit ContinueStmt(int start, int end) : Statement(start, end) {};

    ExecResult interpret(Runtime *runtime, std::deque<Context *> ctxChain) override;

    string astString() override;
};

struct ExpressionStmt : public Statement {

    explicit ExpressionStmt(Expression *expression, int start, int end) : Statement(start, end),
                                                                          expression(expression) {};
    Expression *expression;

    ExecResult interpret(Runtime *runtime, std::deque<Context *> ctxChain) override;

    string astString() override;

};

struct ReturnStmt : public Statement {
    explicit ReturnStmt(int start, int end) : Statement(start, end) {};

    Expression *expression;

    ExecResult interpret(Runtime *runtime, std::deque<Context *> ctxChain) override;

    string astString() override;
};

struct IfStmt : public Statement {

    explicit IfStmt(int start, int end) : Statement(start, end) {};

    Expression *cond{};
    struct BlockStatement *blockStatement{};
    struct BlockStatement *elseBlock{};

    ExecResult interpret(Runtime *runtime, std::deque<Context *> ctxChain) override;

    string astString();
};

struct WhileStmt : public Statement {
    explicit WhileStmt(int start, int end) : Statement(start, end) {};

    Expression *cond{};
    struct BlockStatement *blockStatement;

    ExecResult interpret(Runtime *runtime, std::deque<Context *> ctxChain) override;

    string astString() override;

};

#endif //APOLLO_ABSTRACTSYNTAXTREE_HPP
