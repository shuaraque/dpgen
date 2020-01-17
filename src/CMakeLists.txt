cmake_minimum_required(VERSION 3.4.1)

project(dpgen)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(dpgen dpgen.cpp Expressions.cpp ExpressionInfo.cpp Variables.cpp VariableInfo.cpp)
