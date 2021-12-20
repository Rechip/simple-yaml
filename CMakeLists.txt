cmake_minimum_required(VERSION 3.18)
project(simple_yaml LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories("include")
add_executable(simple_yaml test.cpp)
target_link_libraries(simple_yaml ${CONAN_LIBS})

enable_testing()
add_test(NAME simple_yaml
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        COMMAND simple_yaml)
