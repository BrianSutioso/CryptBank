cmake_minimum_required(VERSION 3.17)
project(CryptBank LANGUAGES CXX C VERSION 0.0.0 DESCRIPTION "Crypto Chain")

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(core)
add_subdirectory(test)
