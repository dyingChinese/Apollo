//
// Created by chineseblack23 on 2024/6/22.
//
#include "apollo.hpp"
#include "Utils.hpp"

namespace apollo {
    Context::~Context() {
        for (auto v: vars) {
            delete v.second;
        }
    }

    Runtime::Runtime() {

    }

    bool Runtime::hasBuiltinFunctionDeclaration(const string &name) {
        return builtin.count(name) == 1;
    }

    Runtime::BuiltinFuncType Runtime::getBuiltinFunctionDeclaration(const string &name) {
        if (auto res = builtin.find(name); res != builtin.end()) {
            return res->second;
        }
        return builtin[name];
    }

    void Runtime::addStatement(Statement *stmt) { stmts.push_back(stmt); }

    std::vector<Statement *> Runtime::getStatements() { return stmts; }

    bool Context::hasVariable(const std::string &identName) {
        return vars.count(identName) == 1;
    }

    void Context::createVariable(const std::string &identName, ValueDeclaration value) {
        auto *var = new VariableDeclaration;
        var->id.name = identName;
        var->value = value;
        vars.emplace(identName, var);
    }

    VariableDeclaration *Context::getVariable(const string &identName) {
        if (auto res = vars.find(identName); res != vars.end()) {
            return res->second;
        }
        return nullptr;
    }

    void Context::addFunction(const std::string &name, FunctionDeclaration *f) {
        funcs.insert(std::make_pair(name, f));
    }

    bool Context::hasFunction(const std::string &name) {
        return funcs.count(name) == 1;
    }

    FunctionDeclaration *Context::getFunctionDeclaration(const string &name) {
        if (auto f = funcs.find(name); f != funcs.end()) {
            return f->second;
        }
        return nullptr;
    }

    ValueDeclaration ValueDeclaration::operator+(ValueDeclaration rhs) {
        ValueDeclaration result;
        // Basic
        if (isType<apollo::Number>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Number;
            //左int 右int 直接相加
            if (isSameType(this->data, rhs.data) && isInt(data)) {
                result.data = castingType<int>() + rhs.castingType<int>();
            }
                //左double 右double, 两个Double 直接相加
            else if (isSameType(this->data, rhs.data) && isDouble(data)) {
                result.data = castingType<double>() + rhs.castingType<double>();
            }

                //左Double 右int 两个转为Double相加
            else if (!isSameType(this->data, rhs.data) && isDouble(data) && isInt(rhs.data)) {
                result.data = castingType<double>() + rhs.castingType<double>();
            }
                //左int 右Double,直接相加
            else if (!isSameType(this->data, rhs.data) && isInt(data) && isDouble(rhs.data)) {
                result.data = castingType<double>() + rhs.castingType<double>();
            }
        } else if (isType<apollo::String>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::String;
            result.data = static_cast<char>(castingType<char>() + rhs.castingType<int>());
        } else if (isType<apollo::Number>() && rhs.isType<apollo::String>()) {
            result.type = apollo::String;
            result.data = static_cast<char>(castingType<int>() + rhs.castingType<char>());
        } else if (isType<apollo::String>() && rhs.isType<apollo::String>()) {
            result.type = apollo::String;
            result.data = valueToStdString(*this) + valueToStdString(rhs);
        } else if (isType<apollo::String>() || rhs.isType<apollo::String>()) {
            result.type = apollo::String;
            result.data = valueToStdString(*this) + valueToStdString(rhs);
        }
            // Array
        else if (isType<apollo::Array>()) {
            result.type = apollo::Array;
            auto resultArr = this->castingType<std::vector<apollo::ValueDeclaration>>();
            resultArr.push_back(rhs);
            result.data = resultArr;
        } else if (rhs.isType<apollo::Array>()) {
            result.type = apollo::Array;
            auto resultArr = rhs.castingType<std::vector<apollo::ValueDeclaration>>();
            resultArr.push_back(*this);
            result.data = resultArr;
        }
            // Invalid
        else {
            panic("TypeError: unexpected arguments of operator +");
        }
        return result;
    }


    ValueDeclaration ValueDeclaration::operator-(ValueDeclaration rhs) {
        ValueDeclaration result;
        if (isType<apollo::Number>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Number;
            if (isSameType(this->data, rhs.data) && isInt(data)) {
                result.data = castingType<int>() - rhs.castingType<int>();
            } else if (isSameType(this->data, rhs.data) && isDouble(data)) {
                result.data = castingType<double>() - rhs.castingType<double>();
            } else if (!isSameType(this->data, rhs.data)) {
                result.data = castingType<double>() + rhs.castingType<double>();
            }
        } else if (isType<apollo::String>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Null;
            result.data = NULL;
        } else if (isType<apollo::Number>() && rhs.isType<apollo::String>()) {
            result.type = apollo::Null;
            result.data = NULL;
        } else if (isType<apollo::String>() && rhs.isType<apollo::String>()) {
            result.type = apollo::Null;
            result.data = NULL;
        } else {
            panic("TypeError: unexpected arguments of operator -");
        }

        return result;
    }

    ValueDeclaration ValueDeclaration::operator*(ValueDeclaration rhs) {
        ValueDeclaration result;
        // Basic
        if (isType<apollo::Number>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Number;
            if (isSameType(this->data, rhs.data) && isInt(data)) {
                result.data = castingType<int>() * rhs.castingType<int>();
            } else if (isSameType(this->data, rhs.data) && isDouble(data)) {
                result.data = castingType<double>() * rhs.castingType<double>();
            } else if (!isSameType(this->data, rhs.data)) {
                result.data = castingType<double>() * rhs.castingType<double>();
            }
        }
            // String
        else if (isType<apollo::String>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::String;
            result.data = NULL;
        } else if (isType<apollo::Number>() && rhs.isType<apollo::String>()) {
            result.type = apollo::String;
            result.data = NULL;
        }
            // Array
        else if (isType<apollo::Number>() && rhs.isType<apollo::Array>()) {
            result.type = apollo::Array;
            result.data = NULL;
        } else if (isType<apollo::Array>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Array;
            result.data = NULL;
        } else {
            panic("TypeError: unexpected arguments of operator *");
        }
        return result;
    }

    ValueDeclaration ValueDeclaration::operator/(ValueDeclaration rhs) {
        ValueDeclaration result;
        if (isType<apollo::Number>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Number;
            if (isSameType(this->data, rhs.data) && isInt(data)) {
                result.data = castingType<int>() / rhs.castingType<int>();
            } else if (isSameType(this->data, rhs.data) && isDouble(data)) {
                result.data = castingType<double>() / rhs.castingType<double>();
            } else if (!isSameType(this->data, rhs.data)) {
                result.data = castingType<double>() / rhs.castingType<double>();
            }
        } else {
            panic("TypeError: unexpected arguments of operator /");
        }
        return result;
    }


    ValueDeclaration ValueDeclaration::operator%(ValueDeclaration rhs) {
        ValueDeclaration result;
        if (isType<apollo::Number>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Number;
            if (isSameType(this->data, rhs.data) && isInt(data)) {
                result.data = castingType<int>() % rhs.castingType<int>();
            } else if (isSameType(this->data, rhs.data) && isDouble(data)) {
                result.data = castingType<double>() % rhs.castingType<double>();
            } else if (!isSameType(this->data, rhs.data)) {
                result.data = castingType<double>() % rhs.castingType<double>();
            }
        } else {
            panic("TypeError: unexpected arguments of operator %");
        }
        return result;
    }


    ValueDeclaration ValueDeclaration::operator&&(ValueDeclaration rhs) {
        ValueDeclaration result;
        if (isType<apollo::Boolean>() && rhs.isType<apollo::Boolean>()) {
            result.type = apollo::Boolean;
            result.data = (castingType<bool>() && rhs.castingType<bool>());
        } else {
            panic("TypeError: unexpected arguments of operator &&");
        }
        return result;
    }

    ValueDeclaration ValueDeclaration::operator||(ValueDeclaration rhs) {
        ValueDeclaration result;
        if (isType<apollo::Boolean>() && rhs.isType<apollo::Boolean>()) {
            result.type = apollo::Boolean;
            result.data = (castingType<bool>() || rhs.castingType<bool>());
        } else {
            panic("TypeError: unexpected arguments of operator ||");
        }
        return result;
    }

    ValueDeclaration ValueDeclaration::operator==(ValueDeclaration rhs) {
        ValueDeclaration result;
        if (isType<apollo::Number>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Boolean;
            if (isSameType(this->data, rhs.data) && isInt(data)) {
                result.data = castingType<int>() == rhs.castingType<int>();
            } else if (isSameType(this->data, rhs.data) && isDouble(data)) {
                result.data = castingType<double>() == rhs.castingType<double>();
            } else if (!isSameType(this->data, rhs.data)) {
                result.data = false;
            }
        } else if (isType<apollo::String>() && rhs.isType<apollo::String>()) {
            result.type = apollo::Boolean;
            std::string lhsStr, rhsStr;
            lhsStr = valueToStdString(*this);
            rhsStr = valueToStdString(rhs);
            result.data = (lhsStr == rhsStr);
        } else if (isType<apollo::Boolean>() && rhs.isType<apollo::Boolean>()) {
            result.type = apollo::Boolean;
            result.data = (castingType<bool>() == rhs.castingType<bool>());
        } else if (this->type == apollo::Null && rhs.type == apollo::Null) {
            result.type = apollo::Boolean;
            result.data = std::make_any<bool>(true);
        } else {
            panic("TypeError: unexpected arguments of operator ==");
        }
        return result;
    }

    ValueDeclaration ValueDeclaration::operator!=(ValueDeclaration rhs) {
        ValueDeclaration result;
        if (isType<apollo::Number>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Boolean;
            if (isSameType(this->data, rhs.data) && isInt(data)) {
                result.data = castingType<int>() != rhs.castingType<int>();
            } else if (isSameType(this->data, rhs.data) && isDouble(data)) {
                result.data = castingType<double>() != rhs.castingType<double>();
            } else if (!isSameType(this->data, rhs.data)) {
                result.data = true;
            }
        } else if (isType<apollo::String>() && rhs.isType<apollo::String>()) {
            result.type = apollo::Boolean;
            std::string lhsStr, rhsStr;
            lhsStr = valueToStdString(*this);
            rhsStr = valueToStdString(rhs);
            result.data = (lhsStr != rhsStr);
        } else if (isType<apollo::Boolean>() && rhs.isType<apollo::Boolean>()) {
            result.type = apollo::Boolean;
            result.data = (castingType<bool>() != rhs.castingType<bool>());
        } else if (this->type == apollo::Null && rhs.type == apollo::Null) {
            result.type = apollo::Boolean;
            result.data = std::make_any<bool>(false);
        } else {
            panic("TypeError: unexpected arguments of operator !=");
        }
        return result;
    }

    ValueDeclaration ValueDeclaration::operator>(ValueDeclaration rhs) {
        ValueDeclaration result;
        if (isType<apollo::Number>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Boolean;
            if (isSameType(this->data, rhs.data) && isInt(data)) {
                result.data = castingType<int>() > rhs.castingType<int>();
            } else if (isSameType(this->data, rhs.data) && isDouble(data)) {
                result.data = castingType<double>() > rhs.castingType<double>();
            } else if (!isSameType(this->data, rhs.data)) {
                result.data = castingType<double>() > rhs.castingType<double>();
            }
        } else if (isType<apollo::String>() && rhs.isType<apollo::String>()) {
            result.type = apollo::Boolean;
            std::string lhsStr, rhsStr;
            lhsStr = valueToStdString(*this);
            rhsStr = valueToStdString(rhs);
            result.data = (lhsStr > rhsStr);
        } else {
            panic("TypeError: unexpected arguments of operator >");
        }
        return result;
    }

    ValueDeclaration ValueDeclaration::operator>=(ValueDeclaration rhs) {
        ValueDeclaration result;
        if (isType<apollo::Number>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Boolean;
            if (isSameType(this->data, rhs.data) && isInt(data)) {
                result.data = castingType<int>() >= rhs.castingType<int>();
            } else if (isSameType(this->data, rhs.data) && isDouble(data)) {
                result.data = castingType<double>() >= rhs.castingType<double>();
            } else if (!isSameType(this->data, rhs.data)) {
                result.data = castingType<double>() >= rhs.castingType<double>();
            }
        } else if (isType<apollo::String>() && rhs.isType<apollo::String>()) {
            result.type = apollo::Boolean;
            std::string lhsStr, rhsStr;
            lhsStr = valueToStdString(*this);
            rhsStr = valueToStdString(rhs);
            result.data = (lhsStr >= rhsStr);
        } else {
            panic("TypeError: unexpected arguments of operator >=");
        }
        return result;
    }

    ValueDeclaration ValueDeclaration::operator<(ValueDeclaration rhs) {
        ValueDeclaration result;
        if (isType<apollo::Number>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Boolean;
            if (isSameType(this->data, rhs.data) && isInt(data)) {
                result.data = castingType<int>() < rhs.castingType<int>();
            } else if (isSameType(this->data, rhs.data) && isDouble(data)) {
                result.data = castingType<double>() < rhs.castingType<double>();
            } else if (!isSameType(this->data, rhs.data)) {
                result.data = castingType<double>() < rhs.castingType<double>();
            }
        } else if (isType<apollo::String>() && rhs.isType<apollo::String>()) {
            result.type = apollo::Boolean;
            std::string lhsStr, rhsStr;
            lhsStr = valueToStdString(*this);
            rhsStr = valueToStdString(rhs);
            result.data = (lhsStr < rhsStr);
        } else {
            panic("TypeError: unexpected arguments of operator <");
        }
        return result;
    }

    ValueDeclaration ValueDeclaration::operator<=(ValueDeclaration rhs) {
        ValueDeclaration result;
        if (isType<apollo::Number>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Boolean;
            if (isSameType(this->data, rhs.data) && isInt(data)) {
                result.data = castingType<int>() <= rhs.castingType<int>();
            } else if (isSameType(this->data, rhs.data) && isDouble(data)) {
                result.data = castingType<double>() <= rhs.castingType<double>();
            } else if (!isSameType(this->data, rhs.data)) {
                result.data = castingType<double>() <= rhs.castingType<double>();
            }
        } else if (isType<apollo::String>() && rhs.isType<apollo::String>()) {
            result.type = apollo::Boolean;
            std::string lhsStr, rhsStr;
            lhsStr = valueToStdString(*this);
            rhsStr = valueToStdString(rhs);
            result.data = (lhsStr <= rhsStr);
        } else {
            panic("TypeError: unexpected arguments of operator <=");
        }
        return result;
    }


    ValueDeclaration ValueDeclaration::operator&(ValueDeclaration rhs) {
        ValueDeclaration result;
        if (isType<apollo::Number>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Number;
            if (isSameType(this->data, rhs.data) && isInt(data)) {
                result.data = castingType<int>() & rhs.castingType<int>();
            } else {
                panic("TypeError: unexpected arguments of operator |");
            }
        } else {
            panic("TypeError: unexpected arguments of operator &");
        }
        return result;
    }

    ValueDeclaration ValueDeclaration::operator|(ValueDeclaration rhs) {
        ValueDeclaration result;
        if (isType<apollo::Number>() && rhs.isType<apollo::Number>()) {
            result.type = apollo::Number;
            if (isSameType(this->data, rhs.data) && isInt(data)) {
                result.data = castingType<int>() | rhs.castingType<int>();
            } else {
                panic("TypeError: unexpected arguments of operator |");
            }
        } else {
            panic("TypeError: unexpected arguments of operator |");
        }
        return result;
    }

}