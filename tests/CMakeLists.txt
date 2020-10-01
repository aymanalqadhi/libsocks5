cmake_minimum_required(VERSION 3.10)

# Project name
project(socks5_tests CXX)

# Project functions
function(create_test test_name)
    add_executable(${test_name} src/${test_name}_tests.cpp)
    target_link_libraries(${test_name} ${MAIN_PROJECT_NAME}
                                       gtest
                                       gtest_main)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Add include directories
include_directories(include/)

# Add tests
create_test(basic_stream)
create_test(request)
create_test(response)
