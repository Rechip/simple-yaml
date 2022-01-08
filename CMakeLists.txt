cmake_minimum_required(VERSION 3.4)
project(simple_yaml LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/tests/*.cpp)

include(GoogleTest)
add_executable(simple_yaml_test ${SRC_FILES})
conan_target_link_libraries(simple_yaml_test)
gtest_discover_tests(simple_yaml_test)

target_include_directories(simple_yaml_test PUBLIC include)

