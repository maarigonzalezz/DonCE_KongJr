
#ifndef CLIENTE_H
#define CLIENTE_H

static int winsock_startup(void);

static void winsock_cleanup(void);

static int send_line(SOCKET s, const char *str);

static int recv_line(SOCKET s, char *buf, size_t cap);

#endif //CLIENTE_H
