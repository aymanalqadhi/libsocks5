cmake_minimum_required(VERSION 3.10)

# Project Name
project(socks5 CXX)

# Project Options
set(CMAKE_CXX_STANDARD 17)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
endif()

# Project files
set(SOURCE_FILES src/dummy.cpp)
include_directories(include/)

# Third-party libraries
add_subdirectory(lib/googletest)

# Targets
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC include/)

# Testing
enable_testing()
add_subdirectory(tests)
