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
#include "renderer.h"


// Variable global para la partida (solución temporal)
static char partida_actual[10] = "A"; // Por defecto "A"

//variable global para el renderer
static Renderer* renderer_global = NULL;

static volatile int g_cerrar_por_nospace = 0;

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
            if (ctx->running) {
                fprintf(stderr, "<< conexión cerrada o error\n");
            }
            break;
        }

        printf(" Mensaje recibido (%d bytes): %s\n", n, line);

        // 1) Caso especial: reach / NoSpace -> cerrar cliente
        if (strstr(line, "\"type_message\":\"reach\"") &&
            strstr(line, "\"exp\":\"NoSpace\"")) {

            printf(">> Servidor dice NoSpace, cerrando cliente...\n");
            g_cerrar_por_nospace = 1;

            // Salimos del hilo
            break;
            }

        // 2) Resto de lógica que ya tenías
        if (strstr(line, "\"type_message\":\"start\"") != NULL) {
            printf("Partida asignada: %s\n", partida_actual);
        }

        if (strstr(line, "\"type_message\":\"snapshot\"") != NULL) {
            printf(" Snapshot detectado\n");
            if (renderer_global != NULL) {
                printf(" Llamando a renderer_update_state...\n");
                renderer_update_state(renderer_global, line);
            } else {
                printf(" ERROR: renderer_global es NULL - no se puede actualizar estado\n");
            }
        }

        fflush(stdout);
    }

    return 0;
}


// Función para obtener la partida actual (para uso en juego.c)
const char* get_partida_actual() {
    return partida_actual;
}

int leer_mensaje_servidor(SOCKET sock, char* buffer, size_t cap) {
    int n = recv_line(sock, buffer, (int)cap);
    if (n <= 0) {
        // No imprimo error fuerte aquí porque quien llame decide qué hacer
        return n;
    }

    printf(" Mensaje recibido (%d bytes): %s\n", n, buffer);
    fflush(stdout);
    return n;
}

static void cerrar_cliente(SOCKET sock, RecvCtx* ctx, uintptr_t th, Juego* juego) {
    // Avisar al servidor que nos vamos (cuando tú decidas)
    net_send_line(sock, "{\"type_message\":\"salir\"}");

    shutdown(sock, SD_SEND);
    ctx->running = 0;
    shutdown(sock, SD_RECEIVE);

    WaitForSingleObject((HANDLE)th, 2000);
    CloseHandle((HANDLE)th);

    closesocket(sock);
    WSACleanup();

    juego_shutdown(juego);
}

int main(void){
    // 1) Iniciar SDL y ventana del juego (solo una vez)
    Juego juego;
    if (!juego_init(&juego, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT)) {
        fprintf(stderr, "No se pudo iniciar el juego\n");
        return 1;
    }

    // Renderer global para snapshots
    renderer_global = &juego.renderer;
    printf(" Renderer global configurado: %p\n", (void*)renderer_global);

    int programa_corriendo = 1;

    while (programa_corriendo) {

        // --- MENÚ PRINCIPAL: jugador / espectador ---
        MenuOpcion opcion = juego_menu(&juego);
        if (opcion == MENU_OPCION_NINGUNA) {
            // Usuario cerró la ventana
            break;
        }

        // --- Winsock + conexión ---
        if (winsock_startup() != 0){
            fprintf(stderr, "WSAStartup failed\n");
            break;
        }

        SOCKET sock = connect_tcp(SERVER_HOST, SERVER_PORT);
        if (sock == INVALID_SOCKET){
            fprintf(stderr, "No se pudo conectar a %s:%s\n", SERVER_HOST, SERVER_PORT);
            winsock_cleanup();
            // Volver al menú principal para reintentar
            continue;
        }

        // --- Enviar register con jugador/espectador ---
        const char* tipo = (opcion == MENU_OPCION_JUGADOR) ? "jugador" : "espectador";

        char msg[128];
        snprintf(msg, sizeof msg,
                 "{\"type_message\":\"register\",\"type\":\"%s\"}", tipo);

        if (net_send_line(sock, msg) != 0) {
            fprintf(stderr, "Error enviando mensaje de registro\n");
            closesocket(sock);
            winsock_cleanup();
            continue; // volver al menú
        }

        char line[1024];

        // --- Leer respuesta inicial del servidor ---
        int n = leer_mensaje_servidor(sock, line, sizeof line);
        if (n <= 0) {
            fprintf(stderr, "Error o cierre al leer respuesta de registro\n");
            closesocket(sock);
            winsock_cleanup();
            continue;
        }

        // Caso general: servidor dice que no hay espacio
        if (strstr(line, "\"type_message\":\"reach\"") &&
            strstr(line, "\"exp\":\"NoSpace\"")) {

            printf("Servidor dice NoSpace, volvemos al menú principal.\n");
            closesocket(sock);
            winsock_cleanup();
            continue;  // NO cerramos la ventana
        }

        // -----------------------------------------------------------------
        //                       CAMINO JUGADOR
        // -----------------------------------------------------------------
        if (opcion == MENU_OPCION_JUGADOR) {

            // Esperamos que lo que acabamos de leer sea "start"
            if (!strstr(line, "\"type_message\":\"start\"")) {
                printf("Esperaba 'start' para jugador y llegó otra cosa.\n");
                closesocket(sock);
                winsock_cleanup();
                continue;
            }

            // Armar el estado inicial del juego
            GameState state;
            // si ya tienes un parseo de start, úsalo aquí:
            // parse_start_message(line, &state);
            // por ahora, algo básico:
            state.vidas = 3;
            state.score = 0;
            state.speed = 1.0f;
            state.jr_x = JR_START_X;
            state.jr_y = JR_START_Y;
            state.jr_mode   = JR_MODE_GROUND;
            state.jr_facing = JR_FACE_RIGHT;
            state.jr_vx = 0;
            state.jr_vy = 0;
            state.vine_idx = -1;
            state.on_ground = 1;

            // Ya arrancó la partida -> ahora sí hilo receptor
            RecvCtx ctx = { .sock = sock, .running = 1 };
            uintptr_t th = _beginthreadex(NULL, 0, recv_thread, &ctx, 0, NULL);
            if (th == 0) {
                fprintf(stderr, "No se pudo crear el hilo receptor\n");
                closesocket(sock);
                winsock_cleanup();
                continue;
            }

            game_loop_jugador(&juego, sock, &state);

            // Cerrar bien la red
            net_send_line(sock, "{\"type_message\":\"salir\"}");
            shutdown(sock, SD_SEND);
            ctx.running = 0;
            shutdown(sock, SD_RECEIVE);
            WaitForSingleObject((HANDLE)th, 2000);
            CloseHandle((HANDLE)th);
            closesocket(sock);
            winsock_cleanup();

            programa_corriendo = 0;  // terminó el juego
            break;
        }

        // -----------------------------------------------------------------
        //                       CAMINO ESPECTADOR
        // -----------------------------------------------------------------
        else { // opcion == MENU_OPCION_ESPECTADOR

            // Aquí 'line' debería ser el mensaje con las opciones, algo tipo:
            // {"type_message":"options","which":"A, B"}

            int tieneA = 0;
            int tieneB = 0;

            if (strstr(line, "\"which\":\"A\"") || strstr(line, "\"which\":\"A, B\""))
                tieneA = 1;
            if (strstr(line, "\"which\":\"B\"") || strstr(line, "\"which\":\"A, B\""))
                tieneB = 1;

            // Mostrar menú extra para espectador (A / B)
            WhichOpcion w = juego_menu_which(&juego, tieneA, tieneB);

            if (w == WHICH_NONE) {
                // Usuario cerró la ventana en el menú de salas
                closesocket(sock);
                winsock_cleanup();
                programa_corriendo = 0;
                break;
            }

            const char* choice = (w == WHICH_A) ? "A" : "B";

            // Mensaje que tú dijiste:
            // {"type_message":"schoice","type":"%s"}
            char msg_schoice[128];
            snprintf(msg_schoice, sizeof msg_schoice,
                     "{\"type_message\":\"schoice\",\"type\":\"%s\"}", choice);

            if (net_send_line(sock, msg_schoice) != 0) {
                fprintf(stderr, "Error enviando mensaje 'schoice'\n");
                closesocket(sock);
                winsock_cleanup();
                continue; // volver al menú principal
            }

            // Esperar ahora el 'start' para espectador
            n = leer_mensaje_servidor(sock, line, sizeof line);
            if (n <= 0 || !strstr(line, "\"type_message\":\"start\"")) {
                fprintf(stderr, "Error esperando 'start' para espectador\n");
                closesocket(sock);
                winsock_cleanup();
                continue;
            }

            // Ya sabemos qué partida observar -> crear hilo receptor
            RecvCtx ctx = { .sock = sock, .running = 1 };
            uintptr_t th = _beginthreadex(NULL, 0, recv_thread, &ctx, 0, NULL);
            if (th == 0) {
                fprintf(stderr, "No se pudo crear el hilo receptor\n");
                closesocket(sock);
                winsock_cleanup();
                continue;
            }

            int running = 1;
            while (running) {
                SDL_Event ev;
                while (SDL_PollEvent(&ev)) {
                    if (ev.type == SDL_EVENT_QUIT) {
                        running = 0;
                        break;
                    }
                    // TODO: aquí dibujas la vista de espectador
                }

                SDL_Delay(16);
            }

            // Cerrar bien la red
            net_send_line(sock, "{\"type_message\":\"salir\"}");
            shutdown(sock, SD_SEND);
            ctx.running = 0;
            shutdown(sock, SD_RECEIVE);
            WaitForSingleObject((HANDLE)th, 2000);
            CloseHandle((HANDLE)th);
            closesocket(sock);
            winsock_cleanup();

            programa_corriendo = 0;  // terminó la sesión de espectador
            break;
        }
    }

    juego_shutdown(&juego);
    return 0;
}


