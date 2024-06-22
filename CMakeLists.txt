cmake_minimum_required(VERSION 3.22)
project(Apollo)

set(CMAKE_CXX_STANDARD 20)

# 添加Include目录到包含路径
include_directories(Include)

# 假设Source目录包含源文件
file(GLOB SOURCE_FILES_HPP Source/**/*.hpp)
file(GLOB SOURCE_FILES_CPP Source/**/*.cpp)

add_executable(Apollo
        main.cpp
        ${SOURCE_FILES_HPP}
        ${SOURCE_FILES_CPP}
        Source/public/AbstractSyntaxTree.hpp
        Source/public/apollo.hpp)
