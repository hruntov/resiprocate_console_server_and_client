cmake_minimum_required(VERSION 2.8)
project(imclient)

add_compile_options(-std=c++11)

find_package(Boost COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

find_library(logger log4cplus NAMES liblog4cplus liblog4cplus-1.1.3)

find_library(resiprocate resip NAMES libresip-1.10)
find_library(resiputil rutil NAMES librutil-1.10)

add_executable(imclient
               src/main.cpp
               src/client_logger.cpp
               src/message_transceiver.cpp
               src/user.cpp lib/json.hpp
)
target_link_libraries(imclient ${Boost_LIBRARIES})
target_link_libraries(imclient ${logger})
target_link_libraries(imclient ${resiprocate} ${resiputil})

if(build_tests)
    add_subdirectory(lib/googletest)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest/googletest/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest/googlemock/include)

    add_executable(test/tests
                   test/nt/tests.cpp
                   src/client_logger.cpp
                   src/user.cpp
                   src/message_transceiver.cpp
    )
    target_link_libraries(test/tests gtest_main log4cplus ${resiprocate} ${resiputil})
endif(build_tests)
