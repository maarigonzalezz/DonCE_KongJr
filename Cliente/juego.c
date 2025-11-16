#include "juego.h"
#include "constants.h"
#include <stdio.h>

// Helpers simples para botón
static SDL_FRect btn1 = { 300, 220, 300, 60 };
static SDL_FRect btn2 = { 300, 320, 300, 60 };

bool juego_init(Juego* j, const char* title, int w, int h) {
    printf(" Inicializando juego...\n");
    bool resultado = renderer_init(&j->renderer, title, w, h);
    printf(" Juego inicializado: %s\n", resultado ? "ÉXITO" : "FALLO");
    return resultado;
}

void juego_shutdown(Juego* j) {
    renderer_shutdown(&j->renderer);
}

// Detecta si un click (x,y) está dentro del rect
static int hit(SDL_FRect r, float x, float y) {
    return (x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h);
}

void juego_menu_network(Juego* j, SOCKET sock) {
    int running = 1;
    SDL_Event ev;

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                running = 0;
            } else if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = (float)ev.button.x, my = (float)ev.button.y;
                if (hit(btn1, mx, my)) {
                    if (net_send_line(sock, "{\"type_message\":\"register\",\"type\":\"jugador\"}") != 0) {
                        fprintf(stderr, "Error enviando 'jugador'\n");
                    } else {
                        printf(">> jugador\n");
                        juego_principal(j, sock);
                        return;
                    }
                } else if (hit(btn2, mx, my)) {
                    if (net_send_line(sock, "{\"type_message\":\"register\",\"type\":\"espectador\"}") != 0) {
                        fprintf(stderr, "Error enviando 'espectador'\n");
                    } else {
                        printf(">> espectador\n");
                    }
                }
            }
        }

        // Dibujar menú básico
        SDL_SetRenderDrawColor(j->renderer.renderer, 20, 24, 28, 255);
        SDL_RenderClear(j->renderer.renderer);

        SDL_SetRenderDrawColor(j->renderer.renderer, 100, 180, 100, 255);
        SDL_RenderFillRect(j->renderer.renderer, &btn1);

        SDL_SetRenderDrawColor(j->renderer.renderer, 100, 140, 200, 255);
        SDL_RenderFillRect(j->renderer.renderer, &btn2);

        SDL_RenderPresent(j->renderer.renderer);
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
                        printf("⬆  climb_up\n");
                        break;
                    case SDLK_DOWN:
                        net_send_line(sock, "{\"type_message\":\"input\",\"action\":\"climb_down\",\"partida\":\"A\"}");
                        printf("⬇  climb_down\n");
                        break;
                    case SDLK_LEFT:
                        net_send_line(sock, "{\"type_message\":\"input\",\"action\":\"move_left\",\"partida\":\"A\"}");
                        printf("⬅  move_left\n");
                        break;
                    case SDLK_RIGHT:
                        net_send_line(sock, "{\"type_message\":\"input\",\"action\":\"move_right\",\"partida\":\"A\"}");
                        printf("➡  move_right\n");
                        break;
                    case SDLK_SPACE:
                        net_send_line(sock, "{\"type_message\":\"input\",\"action\":\"release\",\"partida\":\"A\"}");
                        printf(" release\n");
                        break;
                    case SDLK_ESCAPE:
                        running = 0;
                        break;
                }
            }
        }

        //  Redibujar constantemente (temporal para debug)
        renderer_draw(&j->renderer);
        SDL_Delay(33);
    }
}