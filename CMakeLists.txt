cmake_minimum_required(VERSION 3.28)
project(dircpy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(dircpy main.cpp)
target_link_libraries(dircpy libcmt.lib)