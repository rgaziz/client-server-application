cmake_minimum_required(VERSION 3.12)
project(test_project)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/server)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/client)
