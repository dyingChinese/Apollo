//
// Created by chineseblack23 on 2024/6/22.
//
#include <deque>
#include <vector>
#include "Interpreter.hpp"
#include "AbstractSyntaxTree.hpp"
#include "Utils.hpp"
#include "apollo.hpp"


Interpreter::Interpreter(const std::string &fileName)
        : p(new Parser(fileName)), rt(new apollo::Runtime) {}

Interpreter::~Interpreter() {
    delete p;
    delete rt;
}

apollo::ValueDeclaration Expression::eval(apollo::Runtime *rt,
                                          std::deque<apollo::Context *> ctxChain) {
    panic(
            "RuntimeError: can not evaluate abstract expression at line %d, column "
            "%d\n",
            start, end);
}

apollo::ExecResult Statement::interpret(apollo::Runtime *rt,
                                        std::deque<apollo::Context *> ctxChain) {
    panic(
            "RuntimeError: can not interpret abstract statement at line %d, column "
            "%d\n",
            start, end);
}


void Interpreter::execute() {
    this->p->parse(this->rt);
    this->ctxChain.push_back(new apollo::Context);

    auto stmts = rt->getStatements();
    for (auto stmt: stmts) {
        stmt->interpret(rt, ctxChain);
    }
}

void Interpreter::enterContext(std::deque<apollo::Context *> &ctxChain) {
    auto *tempContext = new Context;
    ctxChain.push_back(tempContext);
}

void Interpreter::leaveContext(std::deque<apollo::Context *> &ctxChain) {
    auto *tempContext = ctxChain.back();
    ctxChain.pop_back();
    delete tempContext;
}

apollo::ValueDeclaration Interpreter::callFunction(apollo::Runtime *rt, apollo::FunctionDeclaration *f,
                                                   std::deque<apollo::Context *> previousCtxChain,
                                                   std::vector<Expression *> args) {
    std::deque<apollo::Context *> funcCtxChain;
    Interpreter::enterContext(funcCtxChain);

    auto *funcCtx = funcCtxChain.back();
    for (int i = 0; i < f->params.size(); i++) {
        std::string paramName = f->params[i];
        apollo::ValueDeclaration argValueDeclaration = args[i]->eval(rt, previousCtxChain);
        funcCtx->createVariable(f->params[i], argValueDeclaration);
    }

    apollo::ExecResult ret(apollo::ExecNormal);
    for (auto &stmt: f->body->stmts) {
        ret = stmt->interpret(rt, funcCtxChain);
        if (ret.execType == apollo::ExecReturn) {
            break;
        }
    }
    Interpreter::leaveContext(funcCtxChain);

    return ret.retValue;
}

apollo::ValueDeclaration Interpreter::calcUnaryExpr(apollo::ValueDeclaration &lhs, Token opt, int line,
                                                    int column) {
    switch (opt) {
        case TK_MINUS:
            switch (lhs.type) {
                case apollo::Number:
                    return apollo::ValueDeclaration(apollo::Number, -std::any_cast<int>(lhs.data));
                default:
                    panic(
                            "TypeError: invalid operand type for operator "
                            "-(negative) at line %d, col %d\n",
                            line, column);
            }
            break;
        case TK_LOGNOT:
            if (lhs.type == apollo::Boolean) {
                return apollo::ValueDeclaration(apollo::Boolean, !std::any_cast<bool>(lhs.data));
            } else {
                panic(
                        "TypeError: invalid operand type for operator "
                        "!(logical not) at line %d, col %d\n",
                        line, column);
            }
            break;
        case TK_BITNOT:
            if (lhs.type == apollo::Number) {
                return apollo::ValueDeclaration(apollo::Number, ~std::any_cast<int>(lhs.data));
            } else {
                panic(
                        "TypeError: invalid operand type for operator "
                        "~(bit not) at line %d, col %d\n",
                        line, column);
            }
            break;
    }

    return lhs;
}

apollo::ValueDeclaration Interpreter::calcBinaryExpr(apollo::ValueDeclaration lhs, Token opt, ValueDeclaration rhs,
                                                     int line, int column) {
    apollo::ValueDeclaration result{apollo::Null};

    switch (opt) {
        case TK_PLUS:
            result = (lhs + rhs);
            break;
        case TK_MINUS:
            result = (lhs - rhs);
            break;
        case TK_TIMES:
            result = (lhs * rhs);
            break;
        case TK_DIV:
            result = (lhs / rhs);
            break;
        case TK_MOD:
            result = (lhs % rhs);
            break;
        case TK_LOGAND:
            result = (lhs && rhs);
            break;
        case TK_LOGOR:
            result = (lhs || rhs);
            break;
        case TK_EQ:
            result = (lhs == rhs);
            break;
        case TK_NE:
            result = (lhs != rhs);
            break;
        case TK_GT:
            result = (lhs > rhs);
            break;
        case TK_GE:
            result = (lhs >= rhs);
            break;
        case TK_LT:
            result = (lhs < rhs);
            break;
        case TK_LE:
            result = (lhs <= rhs);
            break;
        case TK_BITAND:
            result = (lhs & rhs);
            break;
        case TK_BITOR:
            result = (lhs | rhs);
            break;
    }

    return result;
}

apollo::ValueDeclaration Interpreter::assignSwitch(Token opt, apollo::ValueDeclaration lhs,
                                                   apollo::ValueDeclaration rhs) {
    switch (opt) {
        case TK_ASSIGN:
            return rhs;
        case TK_PLUS_AGN:
            return lhs + rhs;
        case TK_MINUS_AGN:
            return lhs - rhs;
        case TK_TIMES_AGN:
            return lhs * rhs;
        case TK_DIV_AGN:
            return lhs / rhs;
        case TK_MOD_AGN:
            return lhs % rhs;
        default:
            panic("InteralError: unexpects branch reached");
    }
}

apollo::ExecResult IfStmt::interpret(apollo::Runtime *rt,
                                     std::deque<apollo::Context *> ctxChain) {
    apollo::ExecResult ret(apollo::ExecNormal);
    ValueDeclaration cond = this->cond->eval(rt, ctxChain);
    if (!cond.isType<apollo::Boolean>()) {
        panic(
                "TypeError: expects bool type in while condition at line %d, "
                "col %d\n",
                start, end);
    }
    if (cond.castingType<bool>()) {
        Interpreter::enterContext(ctxChain);
        for (auto &stmt: blockStatement->stmts) {
            ret = stmt->interpret(rt, ctxChain);
            if (ret.execType == apollo::ExecReturn) {
                break;
            } else if (ret.execType == apollo::ExecBreak) {
                break;
            } else if (ret.execType == apollo::ExecContinue) {
                break;
            }
        }
        Interpreter::leaveContext(ctxChain);
    } else {
        if (elseBlock != nullptr) {
            Interpreter::enterContext(ctxChain);
            for (auto &elseStmt: elseBlock->stmts) {
                ret = elseStmt->interpret(rt, ctxChain);
                if (ret.execType == apollo::ExecReturn) {
                    break;
                } else if (ret.execType == apollo::ExecBreak) {
                    break;
                } else if (ret.execType == apollo::ExecContinue) {
                    break;
                }
            }
            Interpreter::leaveContext(ctxChain);
        }
    }
    return ret;
}

apollo::ExecResult WhileStmt::interpret(apollo::Runtime *rt,
                                        std::deque<apollo::Context *> ctxChain) {
    apollo::ExecResult ret;
    ValueDeclaration cond = this->cond->eval(rt, ctxChain);

    Interpreter::enterContext(ctxChain);
    while (cond.castingType<bool>()) {
        for (auto &stmt: blockStatement->stmts) {

            ret = stmt->interpret(rt, ctxChain);
            if (ret.execType == apollo::ExecReturn) {
                goto outside;
            } else if (ret.execType == apollo::ExecBreak) {

                ret.execType = apollo::ExecNormal;
                goto outside;
            } else if (ret.execType == apollo::ExecContinue) {

                ret.execType = apollo::ExecNormal;
                break;
            }
        }
        cond = this->cond->eval(rt, ctxChain);
        if (!cond.isType<apollo::Boolean>()) {
            panic(
                    "TypeError: expects bool type in while condition at line %d, "
                    "col %d\n",
                    start, end);
        }
    }

    outside:
    Interpreter::leaveContext(ctxChain);
    return ret;
}

apollo::ExecResult ExpressionStmt::interpret(apollo::Runtime *rt,
                                             std::deque<apollo::Context *> ctxChain) {

    this->expression->eval(rt, ctxChain);
    return apollo::ExecResult(apollo::ExecNormal);
}

apollo::ExecResult ReturnStmt::interpret(apollo::Runtime *rt,
                                         std::deque<apollo::Context *> ctxChain) {
    ValueDeclaration retVal = this->expression->eval(rt, ctxChain);
    return apollo::ExecResult(apollo::ExecReturn, retVal);
}

apollo::ExecResult BreakStmt::interpret(apollo::Runtime *rt,
                                        std::deque<apollo::Context *> ctxChain) {
    return apollo::ExecResult(apollo::ExecBreak);
}

apollo::ExecResult ContinueStmt::interpret(apollo::Runtime *rt,
                                           std::deque<apollo::Context *> ctxChain) {
    return apollo::ExecResult(apollo::ExecContinue);
}

apollo::ValueDeclaration NullExpression::eval(apollo::Runtime *rt,
                                              std::deque<apollo::Context *> ctxChain) {
    return apollo::ValueDeclaration(apollo::Null);
}

apollo::ValueDeclaration BooleanExpression::eval(apollo::Runtime *rt,
                                                 std::deque<apollo::Context *> ctxChain) {
    return apollo::ValueDeclaration(apollo::Boolean, this->literal);
}


apollo::ValueDeclaration NumberExpression::eval(apollo::Runtime *rt, std::deque<apollo::Context *> ctxChain) {
    return apollo::ValueDeclaration(apollo::Number, this->literal);
}


apollo::ValueDeclaration StringExpression::eval(apollo::Runtime *rt,
                                                std::deque<apollo::Context *> ctxChain) {
    return apollo::ValueDeclaration(apollo::String, this->literal);
}

apollo::ValueDeclaration ArrayExpression::eval(apollo::Runtime *rt,
                                               std::deque<apollo::Context *> ctxChain) {
    std::vector<apollo::ValueDeclaration> elements;
    for (auto &e: this->literal) {
        elements.push_back(e->eval(rt, ctxChain));
    }

    return apollo::ValueDeclaration(apollo::Array, elements);
}

apollo::ValueDeclaration IdentExpression::eval(apollo::Runtime *rt,
                                               std::deque<apollo::Context *> ctxChain) {
    for (auto p = ctxChain.crbegin(); p != ctxChain.crend(); ++p) {
        auto *ctx = *p;
        if (auto *var = ctx->getVariable(this->identName); var != nullptr) {
            return *var;
        }
    }
    panic("RuntimeError: use of undefined variable \"%s\" at line %d, col %d\n",
          identName.c_str(), this->start, this->end);
}

apollo::ValueDeclaration IndexExpression::eval(apollo::Runtime *rt,
                                               std::deque<apollo::Context *> ctxChain) {
    for (auto p = ctxChain.crbegin(); p != ctxChain.crend(); ++p) {
        auto *ctx = *p;
        if (auto *var = ctx->getVariable(this->identName); var != nullptr) {
            auto idx = this->index->eval(rt, ctxChain);
            if (!idx.isType<apollo::Number>()) {
                panic(
                        "TypeError: expects int type within indexing expression at "
                        "line %d, col %d\n",
                        start, end);
            }
            if (idx.castingType<int>() >=
                var->castingType<std::vector<apollo::ValueDeclaration>>().size()) {
                panic("IndexError: index %d out of range at line %d, col %d\n",
                      idx.castingType<int>(), start, end);
            }
            return var->castingType<std::vector<apollo::ValueDeclaration>>()[idx.castingType<int>()];
        }
    }
    panic("RuntimeError: use of undefined variable \"%s\" at line %d, col %d\n",
          identName.c_str(), this->start, this->end);
}

apollo::ValueDeclaration AssignExpression::eval(apollo::Runtime *rt,
                                                std::deque<apollo::Context *> ctxChain) {
    apollo::ValueDeclaration rhs = this->rightExperssion->eval(rt, ctxChain);

    if (typeid(*leftExpression) == typeid(IdentExpression)) {
        std::string identName = dynamic_cast<IdentExpression *>(leftExpression)->identName;

        for (auto p = ctxChain.crbegin(); p != ctxChain.crend(); ++p) {
            if (auto *var = (*p)->getVariable(identName); var != nullptr) {

                apollo::ValueDeclaration _var = *var;
//                var =
//                        Interpreter::assignSwitch(this->opt,_var, rhs);
                _var = Interpreter::assignSwitch(this->opt, _var, rhs);
                return rhs;
            }
        }

        (ctxChain.back())->createVariable(identName, rhs);
    } else if (typeid(*leftExpression) == typeid(IndexExpression)) {
        std::string identName = dynamic_cast<IndexExpression *>(leftExpression)->identName;
        apollo::ValueDeclaration index =
                dynamic_cast<IndexExpression *>(leftExpression)->index->eval(rt, ctxChain);
        if (!index.isType<apollo::Number>()) {
            panic(
                    "TypeError: expects int type when applying indexing "
                    "to variable %s at line %d, col %d\n",
                    identName.c_str(), start, end);
        }
        for (auto p = ctxChain.crbegin(); p != ctxChain.crend(); ++p) {
            if (auto *var = (*p)->getVariable(identName); var != nullptr) {
                if (!var->isType<apollo::Array>()) {
                    panic(
                            "TypeError: expects array type of variable %s "
                            "at line %d, col %d\n",
                            identName.c_str(), start, end);
                }
                auto &&temp = var->castingType<std::vector<apollo::ValueDeclaration>>();
                temp[index.castingType<int>()] = Interpreter::assignSwitch(
                        this->opt, temp[index.castingType<int>()], rhs);
                var->data = std::move(temp);
                return rhs;
            }
        }

        (ctxChain.back())->createVariable(identName, rhs);
    } else {
        panic("SyntaxError: can not assign to %s at line %d, col %d\n",
              typeid(leftExpression).name(), start, end);
    }
    return rhs;
}

apollo::ValueDeclaration FunCallExpression::eval(apollo::Runtime *rt,
                                                 std::deque<apollo::Context *> ctxChain) {
    if (auto *builtinFunc = rt->getBuiltinFunctionDeclaration(this->funName);
            builtinFunc != nullptr) {
        std::vector<ValueDeclaration> arguments;
        for (auto e: this->args) {
            arguments.push_back(e->eval(rt, ctxChain));
        }
        return builtinFunc(rt, ctxChain, arguments);
    }
    if (auto *func = rt->getFunctionDeclaration(this->funName); func != nullptr) {
        if (func->params.size() != this->args.size()) {
            panic("ArgumentError: expects %d arguments but got %d",
                  func->params.size(), this->args.size());
        }
        return Interpreter::callFunction(rt, func, ctxChain, this->args);
    }

    panic(
            "RuntimeError: can not find function definition of %s in both "
            "built-in "
            "functions and user defined functions",
            this->funName.c_str());
}

apollo::ValueDeclaration BinaryExpression::eval(apollo::Runtime *rt,
                                                std::deque<apollo::Context *> ctxChain) {
    apollo::ValueDeclaration lhs =
            this->leftExpression ? this->leftExpression->eval(rt, ctxChain) : apollo::ValueDeclaration(apollo::Null);
    apollo::ValueDeclaration rhs =
            this->rightExpression ? this->rightExpression->eval(rt, ctxChain) : apollo::ValueDeclaration(apollo::Null);
    Token opt = this->opt;

    if (!lhs.isType<apollo::Null>() && rhs.isType<apollo::Null>()) {
        return Interpreter::calcUnaryExpr(lhs, opt, start, end);
    }

    return Interpreter::calcBinaryExpr(lhs, opt, rhs, start, end);
}


