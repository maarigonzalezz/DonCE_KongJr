#ifndef JUEGO_H
#define JUEGO_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <winsock2.h>
#include "renderer.h"  // Incluir el renderer

// Estructura principal del juego
typedef struct {
    Renderer renderer;  // Usar el renderer en lugar de SDL directamente
} Juego;

bool juego_init(Juego* j, const char* title, int w, int h);
void juego_shutdown(Juego* j);
void juego_menu_network(Juego* j, SOCKET sock);
void juego_principal(Juego* j, SOCKET sock);

// Lo exportamos para que juego.c pueda llamar:
int net_send_line(SOCKET s, const char* str);
const char* get_partida_actual();

#endif //JUEGO_H