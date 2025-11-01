cmake_minimum_required(VERSION 3.28)
project(Cliente C)

set(CMAKE_C_STANDARD 11)

add_executable(Cliente
        cliente.h
        constants.h
        cliente.c
)

if (WIN32)
    target_link_libraries(Cliente PRIVATE ws2_32)
endif()