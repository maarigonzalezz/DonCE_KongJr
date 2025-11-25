#include "juego.h"
#include "constants.h"
#include <stdio.h>
#include <math.h>
#include <SDL3/SDL.h>
#include "mensajes.h"

// Helpers simples para botón
static SDL_FRect btn1 = { BTN1_X, BTN1_Y, BTN1_W, BTN1_H };   // Botón "1 - Jugador"
static SDL_FRect btn2 = { BTN2_X, BTN2_Y, BTN2_W, BTN2_H };   // Botón "2 - Espectador"


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

// ---------------------------------- MENU PRINCIPAL ---------------------------------
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

// AHORA: el menú NO sabe de sockets, solo devuelve la opción escogida
MenuOpcion juego_menu(Juego* j){
    int running = 1;
    MenuOpcion opcion = MENU_OPCION_NINGUNA;
    SDL_Event ev;

    while (running){
        while (SDL_PollEvent(&ev)){
            if (ev.type == SDL_EVENT_QUIT){
                running = 0;  // cierra ventana
            } else if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
                float mx = (float)ev.button.x, my = (float)ev.button.y;
                if (hit(btn1, mx, my)){
                    // Clic en 1 -> Jugador
                    opcion = MENU_OPCION_JUGADOR;
                    running = 0;   // salimos del menú
                } else if (hit(btn2, mx, my)){
                    // Clic en 2 -> Espectador
                    opcion = MENU_OPCION_ESPECTADOR;
                    running = 0;   // salimos del menú
                }
            }
        }
        draw_menu(j);
        SDL_Delay(16);
    }

    return opcion;
}

// ----------------------------------- MENU SECUNDARIO ESPECTADOR ----------------------------------------------
// Botones para el menú "which" (selección de sala a observar)
static SDL_FRect btnA_rect = { WHICH_BTN_A_X, WHICH_BTN_A_Y, WHICH_BTN_A_W, WHICH_BTN_A_H };
static SDL_FRect btnB_rect = { WHICH_BTN_B_X, WHICH_BTN_B_Y, WHICH_BTN_B_W, WHICH_BTN_B_H };

// Dibuja el menú de selección de sala (A / B)
static void draw_menu_which(Juego* j, int tieneA, int tieneB) {
    SDL_Renderer* r = (SDL_Renderer*)j->renderer;

    // Fondo
    SDL_SetRenderDrawColor(r, 20, 24, 28, 255);
    SDL_RenderClear(r);

    // Botón sala A (solo si existe)
    if (tieneA) {
        SDL_SetRenderDrawColor(r, 180, 180, 80, 255);
        SDL_RenderFillRect(r, &btnA_rect);
        // TODO: aquí podrías dibujar el texto "Sala A" usando SDL_ttf
    }

    // Botón sala B (solo si existe)
    if (tieneB) {
        SDL_SetRenderDrawColor(r, 80, 140, 200, 255);
        SDL_RenderFillRect(r, &btnB_rect);
        // TODO: aquí podrías dibujar el texto "Sala B"
    }

    SDL_RenderPresent(r);
}

// Menú "which": el espectador elige qué sala ver.
//  tieneA / tieneB indican si esa opción está disponible (1 = sí, 0 = no).
//  Devuelve WHICH_A, WHICH_B o WHICH_NONE si se cierra la ventana.
WhichOpcion juego_menu_which(Juego* j, int tieneA, int tieneB) {
    int running = 1;
    SDL_Event ev;
    WhichOpcion opcion = WHICH_NONE;

    // Si no hay ninguna opción disponible, salimos de una.
    if (!tieneA && !tieneB) {
        return WHICH_NONE;
    }

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                // Usuario cerró la ventana
                running = 0;
                opcion = WHICH_NONE;
                break;
            } else if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = (float)ev.button.x;
                float my = (float)ev.button.y;

                // Click en Sala A
                if (tieneA && hit(btnA_rect, mx, my)) {
                    opcion = WHICH_A;
                    running = 0;
                    break;
                }

                // Click en Sala B
                if (tieneB && hit(btnB_rect, mx, my)) {
                    opcion = WHICH_B;
                    running = 0;
                    break;
                }
            }
        }

        // Dibujar la pantalla de selección
        draw_menu_which(j, tieneA, tieneB);
        SDL_Delay(16);
    }
    return opcion;
}

void juego_mostrar_game_over(Juego* j, const GameState* st) {
    SDL_Renderer* r = (SDL_Renderer*)j->renderer;
    int running = 1;

    // Botón "VOLVER"
    SDL_FRect btn_volver = {
        WINDOW_WIDTH * 0.5f - 90.0f,   // centrado más o menos
        WINDOW_HEIGHT * 0.5f + 40.0f,  // debajo del texto
        180.0f,
        40.0f
    };

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                // Cerrar ventana
                running = 0;
            } else if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = (float)ev.button.x;
                float my = (float)ev.button.y;

                // ¿Click dentro del botón?
                if (mx >= btn_volver.x &&
                    mx <= btn_volver.x + btn_volver.w &&
                    my >= btn_volver.y &&
                    my <= btn_volver.y + btn_volver.h) {

                    // Salimos de la pantalla de Game Over
                    running = 0;
                }
            }
        }

        // Dibujamos el juego "congelado" de fondo
        render_scene(j, st, 1);

        // Overlay oscuro
        SDL_SetRenderDrawColor(r, 0, 0, 0, 180);
        SDL_FRect overlay = { 0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT };
        SDL_RenderFillRect(r, &overlay);

        // Texto GAME OVER
        SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        float cx = WINDOW_WIDTH * 0.5f - 60.0f;
        float cy = WINDOW_HEIGHT * 0.5f - 30.0f;
        draw_text(r, cx,     cy,     "GAME", 3.0f);
        draw_text(r, cx + 5, cy+30,  "OVER", 3.0f);

        // Score final
        char buf[64];
        snprintf(buf, sizeof(buf), "S%d", st->score);
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        draw_text(r, cx, cy + 80, buf, 2.0f);

        // Dibujar botón "VOLVER"
        SDL_SetRenderDrawColor(r, 80, 80, 80, 255);
        SDL_RenderFillRect(r, &btn_volver);

        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        draw_text(r, btn_volver.x + 20.0f,
                     btn_volver.y + 10.0f,
                     "VOLVER", 1.5f);

        SDL_RenderPresent(r);
        SDL_Delay(16);
    }
}


// ---------------------------- RENDXERIZAR NUMEROS Y LETRAS
static const Glyph* find_glyph(char c) {
    int n = (int)(sizeof(g_glyphs)/sizeof(g_glyphs[0]));
    for (int i = 0; i < n; ++i) {
        if (g_glyphs[i].ch == c) return &g_glyphs[i];
    }
    return NULL;
}

// Dibuja un carácter usando la mini fuente
static void draw_char(SDL_Renderer* r, float x, float y, char c, float scale) {
    const Glyph* g = find_glyph(c);
    if (!g) return; // carácter no soportado

    const char* p = g->pattern;
    int cell_w = (int)(2 * scale);
    int cell_h = (int)(2 * scale);

    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 3; ++col) {
            char bit = p[row*3 + col];
            if (bit == '1') {
                SDL_FRect rect = {
                    x + col * (float)cell_w,
                    y + row * (float)cell_h,
                    (float)cell_w,
                    (float)cell_h
                };
                SDL_RenderFillRect(r, &rect);
            }
        }
    }
}

// Dibuja una cadena simple (caracteres soportados)
static void draw_text(SDL_Renderer* r, float x, float y, const char* text, float scale) {
    float advance = 3.0f * 2.0f * scale; // ancho básico del char
    for (const char* p = text; *p; ++p) {
        if (*p == ' ') {
            x += advance;
            continue;
        }
        draw_char(r, x, y, *p, scale);
        x += advance;
    }
}

// ------------------------------ RENDERIZADOR DEL JUEGO -----------------------------------------
// Array de lianas que son FIJAS a lo que nos piden en el juego clasico
const Liana lianas[NUM_LIANAS] = {
    { LIANA1_X,  LIANA1_TOP_Y, LIANA1_BOT_Y },
    { LIANA2_X,  LIANA1_TOP_Y, LIANA2_BOT_Y }, // comparte TOP con liana1
    { LIANA3_X,  LIANA1_TOP_Y, LIANA3_BOT_Y }, // igual
    { LIANA4_X,  LIANA1_TOP_Y, LIANA4_BOT_Y }, // igual
    { LIANA5_X,  LIANA5_TOP_Y, LIANA5_BOT_Y },
    { LIANA6_X,  LIANA6_TOP_Y, LIANA6_BOT_Y },
    { LIANA7_X,  LIANA7_TOP_Y, LIANA7_BOT_Y },
    { LIANA8_X,  LIANA7_TOP_Y, LIANA8_BOT_Y }, // comparte TOP con liana7
    { LIANA9_X,  LIANA9_TOP_Y, LIANA9_BOT_Y },
    { LIANA10_X, LIANA9_TOP_Y, LIANA9_BOT_Y }, // comparte TOP con liana9
    { LIANA9_X,  LIANA11_TOP_Y, LIANA11_BOT_Y },
    { LIANA10_X, LIANA11_TOP_Y, LIANA11_BOT_Y },
    { LIANA13_X, LIANA13_TOP_Y, LIANA13_BOT_Y }
};

// Todas las superficies donde Jr puede pararse
const Platform platforms[] = {
    // Tierras (solo piso)
    { PLAT_TOP_X,  PLAT_TOP_Y,  PLAT_TOP_W,  PLAT_TOP_H,  PLAT_TIPO_TIERA },

    { TIERRA_MID2_X, TIERRA_MID2_Y, TIERRA_MID2_W, TIERRA_MID2_H, PLAT_TIPO_TIERA },
    { TIERRA_MID3_X, TIERRA_MID3_Y, TIERRA_MID3_W, TIERRA_MID3_H, PLAT_TIPO_TIERA },
    { TIERRA_MID4_X, TIERRA_MID4_Y, TIERRA_MID4_W, TIERRA_MID4_H, PLAT_TIPO_TIERA },
    { TIERRA_MID5_X, TIERRA_MID5_Y, TIERRA_MID5_W, TIERRA_MID5_H, PLAT_TIPO_TIERA },
    { TIERRA_MID6_X, TIERRA_MID6_Y, TIERRA_MID6_W, TIERRA_MID6_H, PLAT_TIPO_TIERA },

    // Plataformas (piso + techo)
    { PLAT_MID1_X, PLAT_MID1_Y, PLAT_MID1_W, PLAT_MID1_H, PLAT_TIPO_PLATAFORMA },
    { PLAT_MID2_X, PLAT_MID2_Y, PLAT_MID2_W, PLAT_MID2_H, PLAT_TIPO_PLATAFORMA },
    { PLAT_MID3_X, PLAT_MID3_Y, PLAT_MID3_W, PLAT_MID3_H, PLAT_TIPO_PLATAFORMA },
    { PLAT_MID4_X, PLAT_MID4_Y, PLAT_MID4_W, PLAT_MID4_H, PLAT_TIPO_PLATAFORMA },
    { PLAT_MID5_X, PLAT_MID5_Y, PLAT_MID5_W, PLAT_MID5_H, PLAT_TIPO_PLATAFORMA },
    { PLAT_MID6_X, PLAT_MID6_Y, PLAT_MID6_W, PLAT_MID6_H, PLAT_TIPO_PLATAFORMA },
};

void render_scene(Juego* j, const GameState* st, int es_jugador) {
    SDL_Renderer* r = (SDL_Renderer*)j->renderer;

    // --- Fondo negro ---
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_RenderClear(r);

    // --- Agua ---
    SDL_FRect agua = { 0, WATER_Y, (float)WINDOW_WIDTH, WATER_H };
    SDL_SetRenderDrawColor(r, 0, 0, 180, 255);
    SDL_RenderFillRect(r, &agua);

    // --- Plataformas ---
    SDL_FRect platTop  = { PLAT_TOP_X,  PLAT_TOP_Y,  PLAT_TOP_W,  PLAT_TOP_H };
    SDL_FRect platMid1 = { PLAT_MID1_X, PLAT_MID1_Y, PLAT_MID1_W, PLAT_MID1_H };
    SDL_FRect platMid2 = { PLAT_MID2_X, PLAT_MID2_Y, PLAT_MID2_W, PLAT_MID2_H };
    SDL_FRect platMid3 = { PLAT_MID3_X, PLAT_MID3_Y, PLAT_MID3_W, PLAT_MID2_H };
    SDL_FRect platMid4 = { PLAT_MID4_X, PLAT_MID4_Y, PLAT_MID4_W, PLAT_MID2_H };
    SDL_FRect platMid5 = { PLAT_MID5_X, PLAT_MID5_Y, PLAT_MID5_W, PLAT_MID2_H };
    SDL_FRect platMid6 = { PLAT_MID6_X, PLAT_MID6_Y, PLAT_MID6_W, PLAT_MID2_H };

    SDL_SetRenderDrawColor(r, 180, 100, 40, 255); // marrón
    SDL_RenderFillRect(r, &platTop);
    SDL_RenderFillRect(r, &platMid1);
    SDL_RenderFillRect(r, &platMid2);
    SDL_RenderFillRect(r, &platMid3);
    SDL_RenderFillRect(r, &platMid4);
    SDL_RenderFillRect(r, &platMid5);
    SDL_RenderFillRect(r, &platMid6);

    // --- Tierras ---
    SDL_FRect tierraMid2 = { TIERRA_MID2_X, TIERRA_MID2_Y, TIERRA_MID2_W, TIERRA_MID2_H };
    SDL_FRect tierraMid3 = { TIERRA_MID3_X, TIERRA_MID3_Y, TIERRA_MID3_W, TIERRA_MID3_H };
    SDL_FRect tierraMid4 = { TIERRA_MID4_X, TIERRA_MID4_Y, TIERRA_MID4_W, TIERRA_MID4_H };
    SDL_FRect tierraMid5 = { TIERRA_MID5_X, TIERRA_MID5_Y, TIERRA_MID5_W, TIERRA_MID5_H };
    SDL_FRect tierraMid6 = { TIERRA_MID6_X, TIERRA_MID6_Y, TIERRA_MID6_W, TIERRA_MID6_H };

    SDL_RenderFillRect(r, &tierraMid2);
    SDL_RenderFillRect(r, &tierraMid3);
    SDL_RenderFillRect(r, &tierraMid4);
    SDL_RenderFillRect(r, &tierraMid5);
    SDL_RenderFillRect(r, &tierraMid6);

    // --- Jaula de Donkey ---
    SDL_FRect dk = { DK_CAGE_X, DK_CAGE_Y, DK_CAGE_W, DK_CAGE_H };
    SDL_FRect mario = { MARIO_X, MARIO_Y, MARIO_W, MARIO_H };
    SDL_SetRenderDrawColor(r, 200, 40, 40, 255); // rojo
    SDL_RenderFillRect(r, &dk);
    SDL_RenderFillRect(r, &mario);

    // --- Lianas ---
    SDL_SetRenderDrawColor(r, 120, 200, 80, 255);
    for (int i = 0; i < NUM_LIANAS; ++i) {
        int height;
        if (i < 4){
            height = lianas[i].bottom - lianas[i].top;
        }
        else
        {
            height = lianas[i].bottom;
        }
        SDL_FRect L = {
            lianas[i].x - LIANA_W * 0.5f,
            lianas[i].top,
            LIANA_W,
            height
        };
        SDL_RenderFillRect(r, &L);
    }

    // --- Dibujar frutas ---
    SDL_SetRenderDrawColor(r, 255, 215, 0, 255); // amarillo-ish
    for (int i = 0; i < st->num_frutas; ++i) {
        if (!st->frutas[i].activa) continue;
        SDL_FRect fr = {
            st->frutas[i].x,
            st->frutas[i].y,
            FRUIT_S, FRUIT_S
        };
        SDL_RenderFillRect(r, &fr);
    }

    // --- Dibujar cocodrilos ---
    for (int i = 0; i < st->num_cocodrilos; ++i) {
        if (!st->cocodrilos[i].activo) continue;

        if (st->cocodrilos[i].tipo == CROC_AZUL) {
            SDL_SetRenderDrawColor(r, 0, 0, 255, 255);
        } else {
            SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        }

        SDL_FRect cr = {
            st->cocodrilos[i].x,
            st->cocodrilos[i].y,
            CROC_W, CROC_H  // tamaño aproximado
        };
        SDL_RenderFillRect(r, &cr);
    }

    // --- Jr (jugador) ---
    SDL_FRect jr = {
        st->jr_x,
        st->jr_y,
        JR_WIDTH,
        JR_HEIGHT
    };
    SDL_SetRenderDrawColor(r, 40, 120, 220, 255); // azulito
    SDL_RenderFillRect(r, &jr);

    // ================= HUD (score, vidas, partida) =================
    // Fondo del HUD (barra superior)
    SDL_FRect hud = { 0, 0, (float)WINDOW_WIDTH, 30.0f };
    SDL_SetRenderDrawColor(r, 0, 0, 0, 200);  // negro semi opaco
    SDL_RenderFillRect(r, &hud);

    // Color texto HUD: blanco
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);

    char buf[64];
    float scale = 2.0f;

    // Score: "S" + número
    snprintf(buf, sizeof(buf), "S%d", st->score);
    draw_text(r, 10.0f, 8.0f, buf, scale);

    // Vidas: "L" + número
    snprintf(buf, sizeof(buf), "L%d", st->vidas);
    draw_text(r, 80.0f, 8.0f, buf, scale);

    // Partida: "P" + letra A/B (usamos solo el primer char de partida)
    char letra = (st->partida[0] != '\0') ? st->partida[0] : 'A';
    snprintf(buf, sizeof(buf), "P%c", letra);
    draw_text(r, 140.0f, 8.0f, buf, scale);

    SDL_RenderPresent(r);
}





