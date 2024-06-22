cmake_minimum_required(VERSION 3.22)
project(Apollo)

set(CMAKE_CXX_STANDARD 20)

# 添加Include目录到包含路径
include_directories(Include)
include_directories(Source/public)
# 假设Source目录包含源文件
file(GLOB SOURCE_FILES_HPP Source/**/*.hpp)
file(GLOB SOURCE_FILES_CPP Source/**/*.cpp)

add_executable(Apollo
        main.cpp
        ${SOURCE_FILES_HPP}
        ${SOURCE_FILES_CPP}
        Source/public/AbstractSyntaxTree.hpp
        Source/public/apollo.hpp Source/public/Parser.hpp Source/private/Parser.cpp Source/public/Utils.hpp Source/private/Interpreter.cpp Source/public/Interpreter.hpp Source/public/Builtin.hpp Source/private/AbstractSyntaxTree.cpp)
