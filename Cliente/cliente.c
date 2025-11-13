#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include "cliente.h"
#include "constants.h"
#include "juego.h"
#include <SDL3/SDL.h>

// Variable global para la partida (solución temporal)
static char partida_actual[10] = "A"; // Por defecto "A"

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

static SOCKET connect_tcp(const char* host, const char* port){
    struct addrinfo hints = {0}, *res = NULL;
    hints.ai_family = AF_UNSPEC; hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, port, &hints, &res) != 0) return INVALID_SOCKET;
    SOCKET s = INVALID_SOCKET;
    for (struct addrinfo* rp = res; rp; rp = rp->ai_next){
        s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (s == INVALID_SOCKET) continue;
        if (connect(s, rp->ai_addr, (int)rp->ai_addrlen) == 0) break;
        closesocket(s); s = INVALID_SOCKET;
    }
    freeaddrinfo(res);
    return s;
}

// Enviar una línea terminada en '\n'
int net_send_line(SOCKET s, const char* str){
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

// ----- Hilo lector -----
typedef struct {
    SOCKET sock;
    volatile int running;
} RecvCtx;

static unsigned __stdcall recv_thread(void* p) {
    RecvCtx* ctx = (RecvCtx*)p;
    char line[1024];
    while (ctx->running) {
        int n = recv_line(ctx->sock, line, sizeof line);
        if (n <= 0) {
            // 0 = cerrado limpio; -1 = error
            if (ctx->running) fprintf(stderr, "<< conexión cerrada o error\n");
            break;
        }
        printf("<< %s\n", line);

        // Detectar mensaje de inicio y asignar partida
        if (strstr(line, "\"type_message\":\"start\"") != NULL) {
            printf("Partida asignada: %s\n", partida_actual);
        }

        fflush(stdout);  // asegura que lo ves de inmediato en la consola
    }
    return 0;
}

// Función para obtener la partida actual (para uso en juego.c)
const char* get_partida_actual() {
    return partida_actual;
}

int main(void){
    if (winsock_startup() != 0){
        fprintf(stderr, "WSAStartup failed\n"); return 1;
    }
    SOCKET sock = connect_tcp(SERVER_HOST, SERVER_PORT);
    if (sock == INVALID_SOCKET){
        fprintf(stderr, "No se pudo conectar a %s:%s\n", SERVER_HOST, SERVER_PORT);
        winsock_cleanup(); return 1;
    }

    RecvCtx ctx = { .sock = sock, .running = 1 };
    uintptr_t th = _beginthreadex(NULL, 0, recv_thread, &ctx, 0, NULL);

    // Inicia la ventana del juego (menú)
    Juego juego = {0};
    if (!juego_init(&juego, "Menu - Selecciona rol", 900, 540)){
        fprintf(stderr, "No se pudo iniciar SDL3\n");
        closesocket(sock); winsock_cleanup(); return 1;
    }

    // Menú con red: la función atiende el loop, y manda "jugador"/"espectador" al hacer clic
    juego_menu_network(&juego, sock);

    // 1) Decirle chao al server
    net_send_line(sock, "{\"type_message\":\"salir\"}");
    // 2) Half-close de salida: manda FIN para que el servidor vea EOF al leer
    shutdown(sock, SD_SEND);
    // 3) Deja corriendo el hilo lector hasta que reciba 0 (EOF) o un último mensaje.
    ctx.running = 0;
    // 4) Despierta al hilo si está bloqueado en recv
    shutdown(sock, SD_RECEIVE);
    // 5) Espera al hilo y limpia
    WaitForSingleObject((HANDLE)th, 2000);
    CloseHandle((HANDLE)th);
    // 6) Cierra el socket y Winsock
    closesocket(sock);
    WSACleanup();
    // 7) Cierra SDL
    juego_shutdown(&juego);
    return 0;
}