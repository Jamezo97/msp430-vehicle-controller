cmake_minimum_required(VERSION 3.0.0)
project(CarControl VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 11)

add_executable(CarControl test.cpp)
target_include_directories(CarControl PUBLIC Controller)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
