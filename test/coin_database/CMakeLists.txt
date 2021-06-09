cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SRCS *.cpp)

include(GoogleTest)

add_executable(test_coin_database ${SRCS})
target_link_libraries(test_coin_database CryptBank ${GTest_LIBRARIES})

gtest_add_tests(TARGET test_coin_database TEST_PREFIX "")