#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "constants.h"
#include "cliente.h"

// manejar bien la librería de Winsock
static int winsock_startup(void) {
    WSADATA wsa;
    int r = WSAStartup(MAKEWORD(2,2), &wsa); // pide la versión de de winsock
    if (r != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", r);
        return -1;
    }
    return 0;
}

static void winsock_cleanup(void) {
    WSACleanup();
}

// enviar lineas de texto
static int send_line(SOCKET s, const char *str) {
    size_t n = strlen(str);
    if (send(s, str, (int)n, 0) != (int)n) return -1;
    if (send(s, "\n", 1, 0) != 1) return -1;
    return 0;
}

// Lee hasta '\n' (lo consume) o fin de conexión.
// Devuelve cantidad de bytes (sin '\n'), 0 si cerrado limpio, -1 en error.
// El buffer siempre queda null-terminado.
static int recv_line(SOCKET s, char *buf, size_t cap) {
    size_t i = 0;
    while (i + 1 < cap) {
        char c;
        int r = recv(s, &c, 1, 0);
        if (r == 0) { // conexión cerrada
            buf[i] = '\0';
            return 0;
        }
        if (r < 0) {
            buf[0] = '\0';
            return -1;
        }
        if (c == '\n') { buf[i] = '\0'; return (int)i; }
        buf[i++] = c;
    }
    buf[cap - 1] = '\0';
    return (int)(cap - 1);
}

int main(void) {
    if (winsock_startup() != 0) return 1; // Iniciaaliza Winsock

    // Resolver host/puerto
    struct addrinfo hints, *res = NULL;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;   // IPv4 o IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    int err = getaddrinfo(SERVER_HOST, SERVER_PORT, &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerrorA(err));
        winsock_cleanup();
        return 1;
    }

    // Intentar conectarse usando las direcciones devueltas
    SOCKET sock = INVALID_SOCKET;
    for (struct addrinfo *rp = res; rp; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock == INVALID_SOCKET) continue;
        if (connect(sock, rp->ai_addr, (int)rp->ai_addrlen) == 0) break;
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
    freeaddrinfo(res);

    if (sock == INVALID_SOCKET) {
        fprintf(stderr, "No se pudo conectar a %s:%s\n", SERVER_HOST, SERVER_PORT);
        winsock_cleanup();
        return 1;
    }

    // Leer saludo inicial (WELCOME)
    char line[RECV_BUF];
    int n = recv_line(sock, line, sizeof line);
    if (n < 0) { fprintf(stderr, "Error recibiendo bienvenida\n"); goto done; }
    if (n == 0) { fprintf(stderr, "Servidor cerró la conexión\n"); goto done; }
    printf("<< %s\n", line);

    puts("Escribe y presiona Enter (Ctrl+Z y Enter para salir):");

    // Bucle de I/O de consola -> socket -> consola
    while (fgets(line, sizeof line, stdin)) {
        // Quitar '\n' de fgets si viene (opcional; send_line agrega su propio '\n')
        size_t L = strlen(line);
        if (L && (line[L-1] == '\n' || line[L-1] == '\r')) line[L-1] = '\0';

        if (send_line(sock, line) != 0) {
            fprintf(stderr, "Error enviando datos\n");
            break;
        }
        int r = recv_line(sock, line, sizeof line);
        if (r <= 0) {
            if (r == 0) puts("Conexión cerrada por el servidor.");
            else        perror("recv");
            break;
        }
        printf("<< %s\n", line);
    }

done:
    closesocket(sock);
    winsock_cleanup();
    return 0;
}