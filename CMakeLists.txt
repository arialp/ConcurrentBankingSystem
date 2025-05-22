cmake_minimum_required(VERSION 3.30.5)
project(ConcurrentBankingSystem C)

# C99 standardı
set(CMAKE_C_STANDARD 99)

# Kaynak ve header dizinleri
include_directories(include)
file(GLOB SOURCES "src/*.c")

# Çıktı klasörü
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Derlenebilir hedef
add_executable(banking_system ${SOURCES})