cmake_minimum_required(VERSION 3.10)

# Project Name
project(socks5 CXX)

# Project Options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Project files
set(SOURCE_FILES src/dummy.cpp)
include_directories(include/)

# Targets
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC include/)
