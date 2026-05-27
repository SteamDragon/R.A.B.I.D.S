cmake_minimum_required(VERSION 3.11)
project(rabids_test CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

include(FetchContent)
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(googletest)

add_executable(rabids_test
    test_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/utils.cpp
)
target_include_directories(rabids_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(rabids_test GTest::gtest GTest::gtest_main OpenSSL::SSL)

include(GoogleTest)
gtest_discover_tests(rabids_test)
