cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
option(build_tests "build tests" OFF)

project(Server)

find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

find_library(logger log4cplus NAMES liblog4cplus liblog4cplus-1.1.3)
find_library(resiprocate resip NAMES libresip-1.10)
find_library(resiputil rutil NAMES librutil-1.10)

add_compile_options(-std=c++11 -Wall -Wextra)

add_executable(
    server
    src/main.cpp
    lib/json.hpp
    src/configuration.cpp
    src/logger.cpp
    src/server.cpp
)

target_link_libraries(
    server
    ${Boost_LIBRARIES}
    ${logger}
    ${resiprocate}
    ${resiputil}
)

if(build_tests)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest")

    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest/googletest/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest/googlemock/include
    )

    add_executable(
        test_list
        test/test_list.cpp
    )

    target_link_libraries(
        test_list
        PRIVATE
        gtest
        gtest_main
        ${logger}
        ${Boost_LIBRARIES}
    )
endif(build_tests)