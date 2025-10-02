cmake_minimum_required(VERSION 4.0)

project(mvec LANGUAGES C)

include(CTest)

include_directories(include)

add_subdirectory(examples)
add_subdirectory(tests)
