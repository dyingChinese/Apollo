//
// Created by chineseblack23 on 2024/6/22.
//

#ifndef APOLLO_UTILS_HPP
#define APOLLO_UTILS_HPP
#pragma once
#include <any>
#include <deque>
#include <string>
#include "apollo.hpp"

std::string valueToStdString(apollo::ValueDeclaration v);

std::string repeatString(int count, const std::string& str);

std::vector<apollo::ValueDeclaration> repeatArray(int count, std::vector<apollo::ValueDeclaration>&& arr);

template <typename _DesireType, typename... _ArgumentType>
inline bool anyone(_DesireType k, _ArgumentType... args) {
    return ((args == k) || ...);
}

[[noreturn]] void panic(char const* const format, ...);


#endif //APOLLO_UTILS_HPP
