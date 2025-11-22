#include <SDL3/SDL.h>
#include <stdbool.h>
#include <winsock2.h>
#ifndef MENSAJES_H
#define MENSAJES_H


// PARA ENVIAR MENSAJES
int net_send_line(SOCKET s, const char* str);  // <- está definido en cliente.c
void enviar_muerte(SOCKET sock, DeathReason r);

// PARA RECIBIRLOS
void parse_start_message(const char* line, GameState* state);
void parse_snapshot(GameState* st, const char* line);
#endif //MENSAJES_H

