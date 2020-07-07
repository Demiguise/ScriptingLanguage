cmake_minimum_required(VERSION 3.14)
project(Scripting_Language VERSION 0.1)

set(Target_Platform x64 CACHE STRING "Target platform")
set_property(CACHE Target_Platform PROPERTY STRINGS x64 Linux)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
