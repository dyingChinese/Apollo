//
// Created by chineseblack23 on 2024/6/20.
//

#ifndef APOLLO_APOLLO_HPP
#define APOLLO_APOLLO_HPP

#include <vector>
#include <string>
#include <any>
#include <unordered_map>
#include <deque>

using namespace std;
struct Statement;
struct Expression;

namespace apollo {
    enum ValueType {
        Number, String, Boolean, Null, Array, Object
    };
    enum ExecutionResultType {
        ExecNormal, ExecReturn, ExecBreak, ExecContinue
    };


    enum CodeType {
        Program,

        BlockStatement,
        ForStatement,
        ForOfStatement,
        ExpressionStatement,

        BinaryExpression,
        UpdateExpression,

        Identifier,
    };
    enum SourceType {
        Module,
        Package,
        File
    };

    enum KeywordKind {

    };

    struct Identifier {
        explicit Identifier() = default;

        string name;
    };


    struct BlockStatement {
        explicit BlockStatement() = default;

        std::vector<Statement *> stmts;
    };


    struct FunctionDeclaration {
        explicit FunctionDeclaration() = default;

        ~FunctionDeclaration() { delete body; };

        struct Identifier id;
        bool expression;
        bool generator;
        bool async;
        vector<string> params;
        struct BlockStatement *body{};
        Expression *retExpr{};
    };

    struct ValueDeclaration {
        explicit ValueDeclaration() {};

        explicit ValueDeclaration(apollo::ValueType type) : type(type) {};

        explicit ValueDeclaration(apollo::ValueType type, std::any data) : type(type), data(std::move(data)) {};


        template<int _apollo>
        inline bool isType();

        template<typename T>
        inline T getTypeName();

        template<typename T>
        inline bool isSameType(T val);

        template<typename T1, typename T2>
        inline bool isSameType(T1 val1, T2 val2);

        template<typename _CastingType>
        inline _CastingType castingType();

        template<typename _DataType>
        inline void set(_DataType data);


        template<typename T>
        inline bool isInt(T value);

        template<>
        inline bool isInt<int>(int value);

        template<>
        inline bool isInt<double>(double value);

        template<typename T>
        inline bool isDouble(T value);

        template<>
        inline bool isDouble<int>(int value);

        template<>
        inline bool isDouble<double>(double value);

        ValueDeclaration operator+(ValueDeclaration rhs);

        ValueDeclaration operator-(ValueDeclaration rhs);

        ValueDeclaration operator*(ValueDeclaration rhs);

        ValueDeclaration operator/(ValueDeclaration rhs);

        ValueDeclaration operator%(ValueDeclaration rhs);

        ValueDeclaration operator&&(ValueDeclaration rhs);

        ValueDeclaration operator||(ValueDeclaration rhs);

        ValueDeclaration operator==(ValueDeclaration rhs);

        ValueDeclaration operator!=(ValueDeclaration rhs);

        ValueDeclaration operator>(ValueDeclaration rhs);

        ValueDeclaration operator>=(ValueDeclaration rhs);

        ValueDeclaration operator<(ValueDeclaration rhs);

        ValueDeclaration operator<=(ValueDeclaration rhs);

        ValueDeclaration operator&(ValueDeclaration rhs);

        ValueDeclaration operator|(ValueDeclaration rhs);

        ValueDeclaration operator~();

        apollo::ValueType type{};
        std::any data;
    };

    struct VariableDeclaration {
        explicit VariableDeclaration() = default;

        struct Identifier id;
//    vector<string> declarations;
        ValueDeclaration value;
    };


    struct ObjectDeclaration {
        explicit ObjectDeclaration() = default;

        struct Identifier id;
        ObjectDeclaration *superClass = nullptr; // 假设这是一个指向ObjectDeclaration的指针
    };

    struct ExecResult {
        explicit ExecResult() : execType(apollo::ExecNormal) {}

        explicit ExecResult(apollo::ExecutionResultType execType) : execType(execType) {}

        explicit ExecResult(apollo::ExecutionResultType execType, ValueDeclaration retValue)
                : execType(execType), retValue(retValue) {}

        apollo::ExecutionResultType execType;
        ValueDeclaration retValue;
    };

    class Context {
    public:
        explicit Context() = default;

        virtual ~Context();

        bool hasVariable(const string &identName);


        void createVariable(const string &identName, ValueDeclaration value);

        VariableDeclaration *getVariable(const string &identName);

        void addFunction(const string &name, FunctionDeclaration *f);

        bool hasFunction(const string &name);

        FunctionDeclaration *getFunctionDeclaration(const string &name);

    private:
        std::unordered_map<std::string, VariableDeclaration *> vars;
        std::unordered_map<std::string, FunctionDeclaration *> funcs;
    };


    class Runtime : public Context {
        using BuiltinFuncType = ValueDeclaration (*)(Runtime *, deque<Context *>,
                                                     std::vector<ValueDeclaration>);

    public:
        explicit Runtime();

        bool hasBuiltinFunctionDeclaration(const string &name);

        BuiltinFuncType getBuiltinFunctionDeclaration(const string &name);

        void addStatement(Statement *stmt);

        vector<Statement *> getStatements();

    private:
        unordered_map<string, BuiltinFuncType> builtin;
        vector<Statement *> stmts;
    };

    template<int _apolloType>
    inline bool ValueDeclaration::isType() {
        return this->type == _apolloType;
    }

    template<typename _CastingType>
    inline _CastingType ValueDeclaration::castingType() {
        return std::any_cast<_CastingType>(data);
    }

    template<typename _DataType>
    inline void ValueDeclaration::set(_DataType data) {
        this->data = std::make_any<_DataType>(std::move(data));
    }

    template<typename T>
    T ValueDeclaration::getTypeName() {
        return typeid(T).name();
    }

    template<typename T>
    bool ValueDeclaration::isSameType(T val) {
        return typeid(val) == typeid(T);
    }

    template<typename T1, typename T2>
    bool ValueDeclaration::isSameType(T1 val1, T2 val2) {
        return typeid(val1) == typeid(val2);
    }

    template<>
    bool ValueDeclaration::isInt<int>(int value) {
        return false;
    }

    template<typename T>
    bool ValueDeclaration::isInt(T value) {
        return false;
    }

    template<>
    bool ValueDeclaration::isInt<double>(double value) {
        return false;
    }

    template<typename T>
    bool ValueDeclaration::isDouble(T value) {
        return false;
    }

    template<>
    bool ValueDeclaration::isDouble<int>(int value) {
        return false;
    }

    template<>
    bool ValueDeclaration::isDouble<double>(double value) {
        return true;
    }

}
#endif //APOLLO_APOLLO_HPP
