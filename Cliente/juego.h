
#ifndef JUEGO_H
#define JUEGO_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <winsock2.h>

// Estructura básica del juego (ventana + renderer)
typedef struct {
    SDL_Window   *win;
    SDL_Renderer *ren;
    void* window;
    void* renderer;
    int width, height;
} Juego;

bool juego_init(Juego* j, const char* title, int w, int h);
void juego_shutdown(Juego* j);

// menú que recibe el socket y envía al hacer clic
void juego_menu_network(Juego* j, SOCKET sock);

// Para actualizar el juego
void juego_principal(Juego* j, SOCKET sock);

// Lo exportamos para que juego.c pueda llamar:
int net_send_line(SOCKET s, const char* str);  // <- está definido en cliente.c
const char* get_partida_actual();
#endif //JUEGO_H
