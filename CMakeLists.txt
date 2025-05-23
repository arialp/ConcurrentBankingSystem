cmake_minimum_required(VERSION 3.30.5)
project(ConcurrentBankingSystem C)

set(CMAKE_C_STANDARD 99)

include_directories(include)
file(GLOB SOURCES "src/*.c")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(banking_system ${SOURCES})