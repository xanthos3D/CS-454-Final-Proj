cmake_minimum_required(VERSION 3.26.4)
project(project1Test)

set(CMAKE_CXX_STANDARD 20)

add_executable(project1Test main.cpp)

include_directories(project1Test "C:/msys64/mingw64/include/")
target_link_libraries(project1Test "C:/msys64/mingw64/lib/libgmpxx.a")
target_link_libraries(project1Test "C:/msys64/mingw64/lib/libgmp.a")