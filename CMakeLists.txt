cmake_minimum_required(VERSION 3.22)
set(CMAKE_TOOLCHAIN_FILE "G:/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(Meta)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

#add_subdirectory(include)
add_subdirectory(test)