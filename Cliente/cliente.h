
#ifndef CLIENTE_H
#define CLIENTE_H

static int winsock_startup(void);

static void winsock_cleanup(void);

static int recv_line(SOCKET s, char *buf, size_t cap);

static unsigned __stdcall recv_thread(void* p);

const char* get_partida_actual();

int leer_mensaje_servidor(SOCKET sock, char* buffer, size_t cap);

#endif //CLIENTE_H
