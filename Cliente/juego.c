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

void juego_menu_network(Juego* j, SOCKET sock){
    int running = 1;
    SDL_Event ev;

    while (running){
        while (SDL_PollEvent(&ev)){
            if (ev.type == SDL_EVENT_QUIT){
                running = 0;
            } else if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
                float mx = (float)ev.button.x, my = (float)ev.button.y;
                if (hit(btn1, mx, my)){
                    // Clic en 1 -> enviar "jugador"
                    if (net_send_line(sock, "CLIENTE QUIERE: jugador") != 0) {
                        fprintf(stderr, "Error enviando 'jugador'\n");
                    } else {
                        printf(">> jugador\n");
                    }
                } else if (hit(btn2, mx, my)){
                    // Clic en 2 -> enviar "espectador"
                    if (net_send_line(sock, "CLIENTE QUIERE: espectador") != 0) {
                        fprintf(stderr, "Error enviando 'espectador'\n");
                    } else {
                        printf(">> espectador\n");
                    }
                }
            }
        }
        draw_menu(j);
        SDL_Delay(16);
    }
}