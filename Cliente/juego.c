#include "juego.h"
#include "constants.h"
#include <SDL3/SDL.h>
#include <stdio.h>

// Helpers simples para botón
static SDL_FRect btn1 = { 300, 220, 300, 60 };   // Botón "1 - Jugador"
static SDL_FRect btn2 = { 300, 320, 300, 60 };   // Botón "2 - Espectador"

bool juego_init(Juego* j, const char* title, int w, int h){
    // Éxito: >= 0 ; Falla: < 0
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    j->window = SDL_CreateWindow(title, w, h, 0);
    if (!j->window){
        fprintf(stderr,"SDL_CreateWindow: %s\n", SDL_GetError());
        return false;
    }

    j->renderer = SDL_CreateRenderer((SDL_Window*)j->window, NULL);
    if (!j->renderer){
        fprintf(stderr,"SDL_CreateRenderer: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

void juego_shutdown(Juego* j){
    if (j->renderer) SDL_DestroyRenderer((SDL_Renderer*)j->renderer);
    if (j->window)   SDL_DestroyWindow((SDL_Window*)j->window);
    SDL_Quit();
}

// Dibuja el menú
static void draw_menu(Juego* j){
    SDL_SetRenderDrawColor((SDL_Renderer*)j->renderer, 20, 24, 28, 255);
    SDL_RenderClear((SDL_Renderer*)j->renderer);

    // Botón 1
    SDL_SetRenderDrawColor((SDL_Renderer*)j->renderer, 100, 180, 100, 255);
    SDL_RenderFillRect((SDL_Renderer*)j->renderer, &btn1);

    // Botón 2
    SDL_SetRenderDrawColor((SDL_Renderer*)j->renderer, 100, 140, 200, 255);
    SDL_RenderFillRect((SDL_Renderer*)j->renderer, &btn2);

    // (Opcional: texto si ya tienes TTF; si no, basta el color)
    SDL_RenderPresent((SDL_Renderer*)j->renderer);
}

// Detecta si un click (x,y) está dentro del rect
static int hit(SDL_FRect r, float x, float y){
    return (x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h);
}

void juego_principal(Juego* j, SOCKET sock);
void juego_menu_network(Juego* j, SOCKET sock) {
    int running = 1;
    SDL_Event ev;
    int es_jugador = 0;

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                running = 0;
            } else if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN && !es_jugador) {
                float mx = (float)ev.button.x, my = (float)ev.button.y;
                if (hit(btn1, mx, my)) {
                    // Clic en 1 -> enviar "jugador"
                    if (net_send_line(sock, "{\"type_message\":\"register\",\"type\":\"jugador\"}") != 0) {
                        fprintf(stderr, "Error enviando 'jugador'\n");
                    } else {
                        printf(">> jugador\n");
                        es_jugador = 1; // Ahora es jugador, pasar a pantalla de juego
                    }
                } else if (hit(btn2, mx, my)) {
                    // Clic en 2 -> enviar "espectador"
                    if (net_send_line(sock, "{\"type_message\":\"register\",\"type\":\"espectador\"}") != 0) {
                        fprintf(stderr, "Error enviando 'espectador'\n");
                    } else {
                        printf(">> espectador\n");
                        // Espectador se queda en menú o va a pantalla de observación
                    }
                }
            } else if (es_jugador) {
                // Si ya es jugador, manejar controles del juego
                juego_principal(j, sock);
                return; // Salir del menú
            }
        }
        draw_menu(j);
        SDL_Delay(16);
    }
}


void juego_principal(Juego* j, SOCKET sock) {
    int running = 1;
    SDL_Event ev;

    printf("=== MODO JUGADOR ACTIVADO ===\n");
    printf("Controles: UP=trepar, DOWN=bajar, LEFT/RIGHT=mover, SPACE=soltar, ESC=salir\n");

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                running = 0;
            } else if (ev.type == SDL_EVENT_KEY_DOWN) {
                const char* partida = get_partida_actual();
                switch (ev.key.key) {
                    case SDLK_UP:
                        net_send_line(sock, "{\"type_message\":\"input\",\"action\":\"climb_up\",\"partida\":\"A\"}");
                        printf(">> climb_up (partida: %s)\n", partida);
                        break;
                    case SDLK_DOWN:
                        net_send_line(sock, "{\"type_message\":\"input\",\"action\":\"climb_down\",\"partida\":\"A\"}");
                        printf(">> climb_down (partida: %s)\n", partida);
                        break;
                    case SDLK_LEFT:
                        net_send_line(sock, "{\"type_message\":\"input\",\"action\":\"move_left\",\"partida\":\"A\"}");
                        printf(">> move_left (partida: %s)\n", partida);
                        break;
                    case SDLK_RIGHT:
                        net_send_line(sock, "{\"type_message\":\"input\",\"action\":\"move_right\",\"partida\":\"A\"}");
                        printf(">> move_right (partida: %s)\n", partida);
                        break;
                    case SDLK_SPACE:
                        net_send_line(sock, "{\"type_message\":\"input\",\"action\":\"release\",\"partida\":\"A\"}");
                        printf(">> release (partida: %s)\n", partida);
                        break;
                    case SDLK_ESCAPE:
                        running = 0;
                        break;
                }
            }
        }

        // Dibujar pantalla de juego básica
        SDL_SetRenderDrawColor((SDL_Renderer*)j->renderer, 0, 0, 0, 255);
        SDL_RenderClear((SDL_Renderer*)j->renderer);

        // Aquí luego dibujarás el juego basado en los snapshots
        // Por ahora solo un mensaje
        SDL_RenderPresent((SDL_Renderer*)j->renderer);
        SDL_Delay(16);
    }
}


// Función helper para convertir tecla a acción
const char* get_action_from_key(SDL_Keycode key) {
    switch (key) {
        case SDLK_UP: return "climb_up";
        case SDLK_DOWN: return "climb_down";
        case SDLK_SPACE: return "release";
        case SDLK_LEFT: return "move_left";   // Para luego
        case SDLK_RIGHT: return "move_right"; // Para luego
        default: return "";
    }
}