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

        template<typename _CastingType>
        inline _CastingType castingType();

        template<typename _DataType>
        inline void set(_DataType data);

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

        ValueDeclaration *getVariable(const string &identName);

        void addFunction(const string &name, FunctionDeclaration *f);

        FunctionDeclaration *getFunctionDeclaration(const string &name);

    private:
        std::unordered_map<std::string, ValueDeclaration *> vars;
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

}
#endif //APOLLO_APOLLO_HPP
