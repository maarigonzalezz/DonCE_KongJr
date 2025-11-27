#include <SDL3/SDL.h>
#include <stdbool.h>
#include <winsock2.h>
#ifndef MENSAJES_H
#define MENSAJES_H


// PARA ENVIAR MENSAJES
int net_send_line(SOCKET s, const char* str);  // <- está definido en cliente.c
void enviar_muerte(SOCKET sock, DeathReason r, const char* partida);
void enviar_fruta_d(SOCKET sock, const char* fruit_id, const char* partida);
void enviar_win(SOCKET sock, const char* partida);
void enviar_jr_pos(SOCKET sock, float x, float y, int mode, int facing, const char* partida);


// PARA RECIBIRLOS
void parse_start_message(const char* line, GameState* state);
void parse_snapshot(GameState* st, const char* line);
void parse_jr_pos(const char* line, GameState* st);
#endif //MENSAJES_H

