cmake_minimum_required(VERSION 3.10)

# Project name
project(socks5_examples CXX)

# Project functions
function(create_example example_name)
    add_executable(${example_name} ${example_name}.cpp)
    target_link_libraries(${example_name} -lpthread)
    add_test(NAME ${example_name} COMMAND ${example_name})
endfunction()

# Add example
create_example(torget)
