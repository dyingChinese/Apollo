//
// Created by chineseblack23 on 2024/6/22.
//
#include "AbstractSyntaxTree.hpp"
#include "Utils.hpp"

std::string Expression::astString() { return "Expr()"; }

std::string Statement::astString() { return "Stmt()"; }

std::string BooleanExpression::astString() {
    return "BoolExpr(" + std::to_string(literal) + ")";
}



std::string NullExpression::astString() { return "NullExpr()"; }

std::string NumberExpression::astString() {
    return "NumberExpression(" + std::to_string(literal) + ")";
}



std::string StringExpression::astString() { return "StringExpr(" + literal + ")"; }

std::string ArrayExpression::astString() {
    std::string str = "ArrayExpr(elements=[";
    if (literal.size() != 0) {
        for (auto& e : literal) {
            str += e->astString();
        }
    }
    str += "])";
    return str;
}

std::string IdentExpression::astString() { return "IdentExpr(" + identName + ")"; }

std::string IndexExpression::astString() {
    std::string str = "IndexExpr(index=";
    str += index->astString();
    str += ")";
    return str;
}

std::string BinaryExpression::astString() {
    std::string str = "BinaryExpr(";
    if (opt != INVALID) {
        str += "opt=";
        switch (opt) {
            case TK_BITAND:
                str += "&";
                break;
            case TK_BITOR:
                str += "|";
                break;
            case TK_BITNOT:
                str += "!";
                break;
            case TK_LOGAND:
                str += "&&";
                break;
            case TK_LOGOR:
                str += "||";
                break;
            case TK_LOGNOT:
                str += "!";
                break;
            case TK_PLUS:
                str += "+";
                break;
            case TK_MINUS:
                str += "-";
                break;
            case TK_TIMES:
                str += "*";
                break;
            case TK_DIV:
                str += "/";
                break;
            case TK_MOD:
                str += "%";
                break;
            case TK_EQ:
                str += "==";
                break;
            case TK_NE:
                str += "!=";
                break;
            case TK_GT:
                str += ">";
                break;
            case TK_GE:
                str += ">=";
                break;
            case TK_LT:
                str += "<";
                break;
            case TK_LE:
                str += "<=";
                break;
            case TK_ASSIGN:
                str += "=";
                break;
            default:
                str += std::to_string(opt);
                break;
        }
    }
    if (leftExpression) {
        str += ",lhs=";
        str += leftExpression->astString();
    }
    if (rightExpression) {
        str += ",rhs=";
        str += rightExpression->astString();
    }
    str += ")";
    return str;
}

std::string FunCallExpression::astString() {
    std::string str = "FunCallExpr(func=";
    str += funName;
    str += ",args=[";
    for (auto& arg : args) {
        str += arg->astString();
        str += ",";
    }
    str += "])";
    return str;
}

std::string AssignExpression::astString() {
    std::string str = "AssignExpr(lhs=";
    str += leftExpression->astString();
    str += ",rhs=";
    str += rightExperssion->astString();
    str += ")";
    return str;
}

std::string ExpressionStmt::astString() {
    std::string str = "ExpressionStmt(expr=";
    str += expression->astString();
    str += ")";
    return str;
}

std::string WhileStmt::astString() {
    std::string str = "WhileStmt(cond=";
    str += cond->astString();
    str += ",exprs=[";
    for (auto& e : blockStatement->stmts) {
        str += e->astString();
        str += ",";
    }
    str += "])";
    return str;
}

std::string IfStmt::astString() {
    std::string str = "IfStmt(cond=";
    str += cond->astString();
    str += ",exprs=[";
    for (auto& e : blockStatement->stmts) {
        str += e->astString();
        str += ",";
    }
    str += "])";
    return str;
}

std::string ReturnStmt::astString() {
    std::string str = "ReturnStmt(";
    if (expression) {
        str += "ret=";
        str += expression->astString();
    }
    str += ")";
    return str;
}

std::string BreakStmt::astString() { return "BreakStmt()"; }

std::string ContinueStmt::astString() { return "ContinueStmt()"; }
