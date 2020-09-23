cmake_minimum_required(VERSION 3.10)

# Project Name
project(socks CXX)

# Project files
set(SOURCE_FILES)
include_directories(include/)

# Targets
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC include/)
