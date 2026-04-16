cmake_minimum_required(VERSION 3.20)

if (WIN32)
    set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic" CACHE STRING "" FORCE)
endif()

project(gBoy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 CONFIG REQUIRED)

add_executable(gBoy src/main.cpp)

target_link_libraries(gBoy PRIVATE $<TARGET_NAME:SDL3::SDL3>)