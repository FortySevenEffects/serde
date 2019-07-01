cmake_minimum_required(VERSION 3.2)
project(serde CXX)

add_subdirectory(builder)

setup_builder()

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(tests)
