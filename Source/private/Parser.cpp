//
// Created by chineseblack23 on 2024/6/22.
//
#include <typeinfo>
#include "apollo.hpp"

#include "Parser.hpp"
#include "Utils.hpp"

void Parser::printLex(const std::string &fileName) {
    Parser parser(fileName);
    std::tuple<Token, std::string> tk;
    do {
        tk = parser.next();
        std::cout << "[" << std::get<0>(tk) << "," << std::get<1>(tk) << "]\n";
    } while (std::get<0>(tk) != TK_EOF);
}

Parser::Parser(const std::string &fileName) : keywords({
                                                               {"if",       KW_IF},
                                                               {"else",     KW_ELSE},
                                                               {"while",    KW_WHILE},
                                                               {"null",     KW_NULL},
                                                               {"true",     KW_TRUE},
                                                               {"false",    KW_FALSE},
                                                               {"for",      KW_FALSE},
                                                               {"func",     KW_FUNC},
                                                               {"return",   KW_RETURN},
                                                               {"break",    KW_BREAK},
                                                               {"continue", KW_CONTINUE}
                                                       }) {
    fs.open(fileName);
    if (!fs.is_open()) {
        panic("ParserError: can not open source file");
    }
}

Parser::~Parser() { fs.close(); }

Expression *Parser::parsePrimaryExpr() {
    if (getCurrentToken() == TK_IDENT) {
        auto ident = getCurrentLexeme();
        currentToken = next();
        switch (getCurrentToken()) {
            case TK_LPAREN: {
                currentToken = next();
                auto *val = new FunCallExpression(start, end);
                val->funName = ident;
                while (getCurrentToken() != TK_RPAREN) {
                    val->args.push_back(parseExpression());
                    if (getCurrentToken() == TK_COMMA) {
                        currentToken = next();
                    }
                }
                assert(getCurrentToken() == TK_RPAREN);
                currentToken = next();
                return val;
            }
            case TK_LBRACKET: {
                currentToken = next();
                auto *val = new IndexExpression(start, end);
                val->identName = ident;
                val->index = parseExpression();
                assert(val->index != nullptr);
                assert(getCurrentToken() == TK_RBRACKET);
                currentToken = next();
                return val;
            }
            default: {
                return new IdentExpression(ident, start, end);
            }
        }
    } else if (getCurrentToken() == LIT_NUMBER) {
        auto val = atoi(getCurrentLexeme().c_str());
        currentToken = next();
        auto *ret = new NumberExpression(start, end);
        ret->literal = val;
        return ret;
    } else if (getCurrentToken() == LIT_STRING) {
        auto val = getCurrentLexeme();
        currentToken = next();
        auto *ret = new StringExpression(start, end);
        ret->literal = val;
        return ret;
    } else if (getCurrentToken() == KW_TRUE || getCurrentToken() == KW_FALSE) {
        auto val = (KW_TRUE == getCurrentToken());
        currentToken = next();
        auto *ret = new BooleanExpression(start, end);
        ret->literal = val;
        return ret;
    } else if (getCurrentToken() == KW_NULL) {
        currentToken = next();
        return new NullExpression(start, end);
    } else if (getCurrentToken() == TK_LPAREN) {
        currentToken = next();
        auto val = parseExpression();
        assert(getCurrentToken() == TK_RPAREN);
        currentToken = next();
        return val;
    } else if (getCurrentToken() == TK_LBRACKET) {
        currentToken = next();
        auto *ret = new ArrayExpression(start, end);
        if (getCurrentToken() != TK_RBRACKET) {
            while (getCurrentToken() != TK_RBRACKET) {
                ret->literal.push_back(parseExpression());
                if (getCurrentToken() == TK_COMMA) {
                    currentToken = next();
                }
            }
            assert(getCurrentToken() == TK_RBRACKET);
            currentToken = next();
            return ret;
        } else {
            currentToken = next();
            // It's an empty array literal
            return ret;
        }
    }
    return nullptr;
}

Expression *Parser::parseUnaryExpr() {
    if (anyone(getCurrentToken(), TK_MINUS, TK_LOGNOT, TK_BITNOT)) {
        auto val = new BinaryExpression(start, end);
        val->opt = getCurrentToken();
        currentToken = next();
        val->leftExpression = parseUnaryExpr();
        return val;
    } else if (anyone(getCurrentToken(), LIT_NUMBER, LIT_STRING,
                      TK_IDENT, TK_LPAREN, TK_LBRACKET, KW_TRUE, KW_FALSE,
                      KW_NULL)) {
        return parsePrimaryExpr();
    }
    return nullptr;
}

Expression *Parser::parseExpression(short oldPrecedence) {
    auto *p = parseUnaryExpr();

    if (anyone(getCurrentToken(), TK_ASSIGN, TK_PLUS_AGN, TK_MINUS_AGN,
               TK_TIMES_AGN, TK_DIV_AGN, TK_MOD_AGN)) {
        if (typeid(*p) != typeid(IndexExpression) &&
            typeid(*p) != typeid(IndexExpression)) {
            panic("SyntaxError: can not assign to %s", typeid(*p).name());
        }
        auto *assignExpr = new AssignExpression(start, end);
        assignExpr->opt = getCurrentToken();
        assignExpr->leftExpression = p;
        currentToken = next();
        assignExpr->rightExperssion = parseExpression();
        return assignExpr;
    }

    while (anyone(getCurrentToken(), TK_BITOR, TK_BITAND, TK_BITNOT, TK_LOGOR,
                  TK_LOGAND, TK_LOGNOT, TK_EQ, TK_NE, TK_GT, TK_GE, TK_LT,
                  TK_LE, TK_PLUS, TK_MINUS, TK_MOD, TK_TIMES, TK_DIV)) {
        short currentPrecedence = Parser::precedence(getCurrentToken());
        if (oldPrecedence > currentPrecedence) {
            return p;
        }
        auto tmp = new BinaryExpression(start, end);
        tmp->leftExpression = p;
        tmp->opt = getCurrentToken();
        currentToken = next();
        tmp->rightExpression = parseExpression(currentPrecedence + 1);
        p = tmp;
    }
    return p;
}

ExpressionStmt *Parser::parseExpressionStmt() {
    ExpressionStmt *node = nullptr;
    if (auto p = parseExpression(); p != nullptr) {
        node = new ExpressionStmt(p, start, end);
    }
    return node;
}

IfStmt *Parser::parseIfStmt() {
    auto *node = new IfStmt(start, end);
    currentToken = next();
    node->cond = parseExpression();
    assert(getCurrentToken() == TK_RPAREN);
    currentToken = next();
    node->blockStatement = parseBlock();
    if (getCurrentToken() == KW_ELSE) {
        currentToken = next();
        node->elseBlock = parseBlock();
    }
    return node;
}

WhileStmt *Parser::parseWhileStmt() {
    auto *node = new WhileStmt(start, end);
    currentToken = next();
    node->cond = parseExpression();
    assert(getCurrentToken() == TK_RPAREN);
    currentToken = next();
    node->blockStatement = parseBlock();
    return node;
}

ReturnStmt *Parser::parseReturnStmt() {
    auto *node = new ReturnStmt(start, end);
    node->expression = parseExpression();
    return node;
}

Statement *Parser::parseStatement() {
    Statement *node;
    switch (getCurrentToken()) {
        case KW_IF:
            currentToken = next();
            node = parseIfStmt();
            break;
        case KW_WHILE:
            currentToken = next();
            node = parseWhileStmt();
            break;
        case KW_RETURN:
            currentToken = next();
            node = parseReturnStmt();
            break;
        case KW_BREAK:
            currentToken = next();
            node = new BreakStmt(start, end);
            break;
        case KW_CONTINUE:
            currentToken = next();
            node = new ContinueStmt(start, end);
            break;
        default:
            node = parseExpressionStmt();
            break;
    }
    return node;
}

std::vector<Statement *> Parser::parseStatementList() {
    std::vector<Statement *> node;
    Statement *p;
    while ((p = parseStatement()) != nullptr) {
        node.push_back(p);
    }
    return node;
}

struct BlockStatement *Parser::parseBlock() {
    struct BlockStatement *node{new struct BlockStatement};
    currentToken = next();
    node->stmts = parseStatementList();
    assert(getCurrentToken() == TK_RBRACE);
    currentToken = next();
    return node;
}

std::vector<std::string> Parser::parseParameterList() {
    std::vector<std::string> node;
    currentToken = next();
    if (getCurrentToken() == TK_RPAREN) {
        currentToken = next();
        return std::move(node);
    }

    while (getCurrentToken() != TK_RPAREN) {
        if (getCurrentToken() == TK_IDENT) {
            node.push_back(getCurrentLexeme());
        } else {
            assert(getCurrentToken() == TK_COMMA);
        }
        currentToken = next();
    }
    assert(getCurrentToken() == TK_RPAREN);
    currentToken = next();
    return move(node);
}

apollo::FunctionDeclaration *Parser::parseFuncDef(apollo::Context *context) {
    assert(getCurrentToken() == KW_FUNC);
    currentToken = next();

    // Check if function was already be defined
    if (context->hasFunction(getCurrentLexeme())) {
        panic("SyntaxError: multiply function definitions of %s found",
              getCurrentLexeme().c_str());
    }

    auto *node = new apollo::FunctionDeclaration;
    node->id.name = getCurrentLexeme();
    currentToken = next();
    assert(getCurrentToken() == TK_LPAREN);
    node->params = parseParameterList();
    node->body = parseBlock();
    return node;
}

void Parser::parse(apollo::Runtime *rt) {
    currentToken = next();
    if (getCurrentToken() == TK_EOF) {
        return;
    }
    do {
        if (getCurrentToken() == KW_FUNC) {
            auto *f = parseFuncDef(rt);
            rt->addFunction(f->id.name, f);
        } else {
            rt->addStatement(parseStatement());
        }
    } while (getCurrentToken() != TK_EOF);
}

std::tuple<Token, std::string> Parser::next() {
    char c = getNextChar();

    if (c == EOF) {
        return std::make_tuple(TK_EOF, "");
    }
    if (anyone(c, ' ', '\n', '\r', '\t')) {
        while (anyone(c, ' ', '\n', '\r', '\t')) {
            if (c == '\n') {
                start++;
                end = 0;
            }
            c = getNextChar();
        }
        if (c == EOF) {
            return std::make_tuple(TK_EOF, "");
        }
    }

    if (c == '#') {
        another_comment:
        while (c != '\n' && c != EOF) {
            c = getNextChar();
        }
        // consume newlines
        while (c == '\n') {
            start++;
            end = 0;
            c = getNextChar();
        }
        if (c == '#') {
            goto another_comment;
        }
        if (c == EOF) {
            return std::make_tuple(TK_EOF, "");
        }
    }

    if (c >= '0' && c <= '9') {
        std::string lexeme{c};
        bool isDouble = false;
        char cn = peekNextChar();
        while ((cn >= '0' && cn <= '9') || (!isDouble && cn == '.')) {
            if (c == '.') {
                isDouble = true;
            }
            c = getNextChar();
            cn = peekNextChar();
            lexeme += c;
        }
        return !isDouble ? make_tuple(LIT_NUMBER, lexeme)
                         : make_tuple(LIT_NUMBER, lexeme);
    }
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        std::string lexeme{c};
        char cn = peekNextChar();
        while ((cn >= 'a' && cn <= 'z') || (cn >= 'A' && cn <= 'Z') ||
               (cn >= '0' && cn <= '9') || cn == '_') {
            c = getNextChar();
            lexeme += c;
            cn = peekNextChar();
        }
        auto result = keywords.find(lexeme);
        return result != keywords.end()
               ? std::make_tuple(result->second, lexeme)
               : std::make_tuple(TK_IDENT, lexeme);
    }

    if (c == '\'') {
        std::string lexeme;
        lexeme += getNextChar();
        if (peekNextChar() != '\'') {
            panic(
                    "SynxaxError: a character literal should surround with "
                    "single-quote");
        }
        c = getNextChar();
        return std::make_tuple(LIT_STRING, lexeme);
    }
    if (c == '"') {
        std::string lexeme;
        char cn = peekNextChar();
        while (cn != '"') {
            c = getNextChar();
            lexeme += c;
            cn = peekNextChar();
        }
        c = getNextChar();
        return std::make_tuple(LIT_STRING, lexeme);
    }

    if (c == '\'') {
        std::string lexeme;
        char cn = peekNextChar();
        while (cn != '\'') {
            c = getNextChar();
            lexeme += c;
            cn = peekNextChar();
        }
        c = getNextChar();
        return std::make_tuple(LIT_STRING, lexeme);
    }

    if (c == '[') {
        return std::make_tuple(TK_LBRACKET, "[");
    }
    if (c == ']') {
        return std::make_tuple(TK_RBRACKET, "]");
    }
    if (c == '{') {
        return std::make_tuple(TK_LBRACE, "{");
    }
    if (c == '}') {
        return std::make_tuple(TK_RBRACE, "}");
    }
    if (c == '(') {
        return std::make_tuple(TK_LPAREN, "(");
    }
    if (c == ')') {
        return std::make_tuple(TK_RPAREN, ")");
    }
    if (c == ',') {
        return std::make_tuple(TK_COMMA, ",");
    }
    if (c == '+') {
        if (peekNextChar() == '=') {
            c = getNextChar();
            return std::make_tuple(TK_PLUS_AGN, "+=");
        }
        return std::make_tuple(TK_PLUS, "+");
    }
    if (c == '-') {
        if (peekNextChar() == '=') {
            c = getNextChar();
            return std::make_tuple(TK_MINUS_AGN, "-=");
        }
        return std::make_tuple(TK_MINUS, "-");
    }
    if (c == '*') {
        if (peekNextChar() == '=') {
            c = getNextChar();
            return std::make_tuple(TK_TIMES_AGN, "*=");
        }
        return std::make_tuple(TK_TIMES, "*");
    }
    if (c == '/') {
        if (peekNextChar() == '=') {
            c = getNextChar();
            return std::make_tuple(TK_DIV_AGN, "/=");
        }
        return std::make_tuple(TK_DIV, "/");
    }
    if (c == '%') {
        if (peekNextChar() == '=') {
            c = getNextChar();
            return std::make_tuple(TK_MOD_AGN, "%=");
        }
        return std::make_tuple(TK_MOD, "%");
    }
    if (c == '~') {
        return std::make_tuple(TK_BITNOT, "~");
    }
    if (c == '=') {
        if (peekNextChar() == '=') {
            c = getNextChar();
            return std::make_tuple(TK_EQ, "==");
        }
        return std::make_tuple(TK_ASSIGN, "=");
    }
    if (c == '!') {
        if (peekNextChar() == '=') {
            c = getNextChar();
            return std::make_tuple(TK_NE, "!=");
        }
        return std::make_tuple(TK_LOGNOT, "!");
    }
    if (c == '|') {
        if (peekNextChar() == '|') {
            c = getNextChar();
            return std::make_tuple(TK_LOGOR, "||");
        }
        return std::make_tuple(TK_BITOR, "|");
    }
    if (c == '&') {
        if (peekNextChar() == '&') {
            c = getNextChar();
            return std::make_tuple(TK_LOGAND, "&&");
        }
        return std::make_tuple(TK_BITAND, "&");
    }
    if (c == '>') {
        if (peekNextChar() == '=') {
            c = getNextChar();
            return std::make_tuple(TK_GE, ">=");
        }
        return std::make_tuple(TK_GT, ">");
    }
    if (c == '<') {
        if (peekNextChar() == '=') {
            c = getNextChar();
            return std::make_tuple(TK_LE, "<=");
        }
        return std::make_tuple(TK_LT, "<");
    }
    panic("SynxaxError: unknown token %c", c);
    return std::make_tuple(INVALID, "invalid");
}

short Parser::precedence(Token op) {
    switch (op) {
        case TK_LOGOR:
            return 1;
        case TK_LOGAND:
            return 2;
        case TK_EQ:
        case TK_NE:
        case TK_GT:
        case TK_GE:
        case TK_LT:
        case TK_LE:
            return 3;
        case TK_PLUS:
        case TK_MINUS:
        case TK_BITOR:
            return 4;
        case TK_TIMES:
        case TK_MOD:
        case TK_DIV:
        case TK_BITAND:
            return 5;
        default:
            // Lowest precedence
            return 0;
    }
}
