cmake_minimum_required(VERSION 3.28)
project(Cliente C)

set(CMAKE_C_STANDARD 11)

set(SDL3_DIR "${CMAKE_SOURCE_DIR}/SDL3/x86_64-w64-mingw32")

# Incluir directorios
include_directories("${SDL3_DIR}/include")
include_directories("${SDL3_DIR}/include/SDL3_image")

# Directorios de librerías
link_directories("${SDL3_DIR}/lib")

add_executable(Cliente
        cliente.h
        constants.h
        cliente.c
        juego.h
        juego.c
        renderer.h
        renderer.c
        mensajes.h
        mensajes.c
        jugador.c
)

if (WIN32)
    # Enlazar explícitamente con los archivos .a
    target_link_libraries(Cliente PRIVATE
            SDL3
            "${SDL3_DIR}/lib/libSDL3_image.dll.a"
            ws2_32
    )
endif()

add_custom_command(TARGET Cliente POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_DIR}/bin/SDL3.dll"
        $<TARGET_FILE_DIR:Cliente>
)

add_custom_command(TARGET Cliente POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_DIR}/bin/SDL3_image.dll"
        $<TARGET_FILE_DIR:Cliente>
)
# Copiar carpeta sprites COMPLETA
add_custom_command(TARGET Cliente POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/sprites"
        $<TARGET_FILE_DIR:Cliente>/sprites
)