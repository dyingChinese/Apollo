//
// Created by chineseblack23 on 2024/6/22.
//
#include <cstdarg>
#include "apollo.hpp"
#include "Utils.hpp"

std::string valueToStdString(apollo::ValueDeclaration v) {
    switch (v.type) {
        case apollo::Boolean:
            return v.castingType<bool>() ? "true" : "false";
        case apollo::Number:
            return std::to_string(v.castingType<int>());
        case apollo::Null:
            return "null";
        case apollo::Array: {
            std::string str = "[";
            auto elements = v.castingType<std::vector<apollo::ValueDeclaration>>();
            for (int i = 0; i < elements.size(); i++) {
                str += valueToStdString(elements[i]);

                if (i != elements.size() - 1) {
                    str += ",";
                }
            }
            str += "]";
            return str;
        }
        case apollo::String:
            return v.castingType<std::string>();
    }
    return "unknown";
}

std::string repeatString(int count, const std::string& str) {
    std::string result;
    for (int i = 0; i < count; i++) {
        result += str;
    }
    return result;
}

std::vector<apollo::ValueDeclaration> repeatArray(int count, std::vector<apollo::ValueDeclaration>&& arr) {
    std::vector<apollo::ValueDeclaration> result;
    for (int i = 0; i < count; i++) {
        result.insert(result.begin(), arr.begin(), arr.end());
    }
    return result;
}

[[noreturn]] void panic(char const* const format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}
