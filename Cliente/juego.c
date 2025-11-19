#include "juego.h"
#include "constants.h"
#include <stdio.h>
#include <math.h>
#include <SDL3/SDL.h>

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

static const Liana lianas[NUM_LIANAS] = {
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
static const Platform platforms[] = {
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

    // --- Jr (jugador) ---
    SDL_FRect jr = {
        st->jr_x,
        st->jr_y,
        JR_WIDTH,
        JR_HEIGHT
    };
    SDL_SetRenderDrawColor(r, 40, 120, 220, 255); // azulito
    SDL_RenderFillRect(r, &jr);

    // Aquí más adelante dibujas cocodrilos, frutas, HUD de vidas/score, etc.

    SDL_RenderPresent(r);
}

void game_loop_jugador(Juego* j, SOCKET sock, GameState* st) {
    int running = 1;
    Uint64 last_ticks = SDL_GetTicks();

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                running = 0;
                break;
            }

            manejar_input_jugador(&ev, st);
        }

        Uint64 now = SDL_GetTicks();
        float dt = (now - last_ticks) / 1000.0f;
        last_ticks = now;

        actualizar_logica_jugador(st, dt);

        // por ahora solo dibujamos el stage + Jr
        render_scene(j, st, /*es_jugador=*/1);

        SDL_Delay(16);
    }
}

void manejar_input_jugador(SDL_Event* ev, GameState* st) {
    if (ev->type == SDL_EVENT_KEY_DOWN) {
        SDL_KeyboardEvent *k = &ev->key;

        switch (k->key) {
        case SDLK_LEFT:
            st->jr_vx = -JR_SPEED;
            st->jr_facing = JR_FACE_LEFT;
            // si está en liana, saltar hacia el lado sería soltarla:
            if (st->jr_mode == JR_MODE_VINE) {
                st->jr_mode = JR_MODE_GROUND;
                st->vine_idx = -1;
                st->on_ground = 0;     // ahora está en el aire
            }
            break;

        case SDLK_RIGHT:
            st->jr_vx = JR_SPEED;
            st->jr_facing = JR_FACE_RIGHT;
            if (st->jr_mode == JR_MODE_VINE) {
                st->jr_mode = JR_MODE_GROUND;
                st->vine_idx = -1;
                st->on_ground = 0;     // ahora está en el aire
            }
            break;

        case SDLK_SPACE:
            if (st->jr_mode == JR_MODE_GROUND) {
                // dejar saltar si está "casi" quieto verticalmente (en el piso)
                if (fabsf(st->jr_vy) < 1.0f) {
                    st->jr_vy = JR_JUMP_VY;
                    st->on_ground = 0;
                }
            }
            break;

        case SDLK_UP:
            if (st->jr_mode == JR_MODE_VINE) {
                st->jr_vy = -VINE_SPEED;   // subir liana
            } else {
                // intentar engancharse
                int idx = buscar_liana_cercana(st);
                if (idx != -1) {
                    enganchar_a_liana(st, idx);
                }
            }
            break;

        case SDLK_DOWN:
            if (st->jr_mode == JR_MODE_VINE) {
                st->jr_vy = VINE_SPEED;    // bajar liana
            }
            break;

        default:
            break;
        }
    } else if (ev->type == SDL_EVENT_KEY_UP) {
        SDL_KeyboardEvent *k = &ev->key;

        switch (k->key) {
        case SDLK_LEFT:
        case SDLK_RIGHT:
            st->jr_vx = 0;
            break;

        case SDLK_UP:
        case SDLK_DOWN:
            if (st->jr_mode == JR_MODE_VINE) {
                st->jr_vy = 0; // suelta tecla -> se detiene en la liana
            }
            break;

        default:
            break;
        }
    }
}

void actualizar_logica_jugador(GameState* st, float dt) {
    if (st->jr_mode == JR_MODE_GROUND) {
        // 1) Gravedad si no está en el piso
        if (!st->on_ground) {
            st->jr_vy += JR_GRAVITY * dt;
        }

        // 2) Guardar posición anterior
        float old_x = st->jr_x;
        float old_y = st->jr_y;

        // 3) Mover
        st->jr_x += st->jr_vx * dt;
        st->jr_y += st->jr_vy * dt;

        // 4) Colisión con plataformas (suelo + paredes + techo)
        aplicar_colision_plataformas(st, old_x, old_y);

        // 5) Si sigue en el aire, puede engancharse a una liana
        if (!st->on_ground) {
            int idx = buscar_liana_cercana(st);
            if (idx != -1) {
                enganchar_a_liana(st, idx);
            }
        }

    } else if (st->jr_mode == JR_MODE_VINE) {
        // --- Modo liana ---
        float old_x = st->jr_x;
        float old_y = st->jr_y;

        // 1) Mover solo en Y por la liana
        st->jr_y += st->jr_vy * dt;

        // 2) Mantener X pegado a la liana
        if (st->vine_idx >= 0 && st->vine_idx < NUM_LIANAS) {
            float x = lianas[st->vine_idx].x;

            if (st->jr_facing == JR_FACE_LEFT)
                st->jr_x = x - JR_WIDTH;
            else
                st->jr_x = x;

            float l_top, l_bottom;
            liana_bounds(st->vine_idx, &l_top, &l_bottom);

            float minY = l_top    - JR_HEIGHT;
            float maxY = l_bottom - JR_HEIGHT;

            if (st->jr_y < minY) st->jr_y = minY;
            if (st->jr_y > maxY) st->jr_y = maxY;
        }

        // 3) IMPORTANTE: también checar techo/paredes aquí
        //    (para que al subir por la liana no se meta en la plataforma)
        aplicar_colision_plataformas(st, old_x, old_y);
    }

    // Mantener dentro de la pantalla en X
    if (st->jr_x < 0)
        st->jr_x = 0;
    if (st->jr_x + JR_WIDTH > WINDOW_WIDTH)
        st->jr_x = WINDOW_WIDTH - JR_WIDTH;
}

static void liana_bounds(int idx, float* top, float* bottom) {
    const Liana* L = &lianas[idx];

    *top = L->top;

    if (idx < 4) {
        // En las primeras 4, bottom ya ES coordenada Y
        *bottom = L->bottom;
    } else {
        // En las demás, bottom está guardando la ALTURA
        *bottom = L->top + L->bottom;
    }
}

static int buscar_liana_cercana(const GameState* st) {
    float jr_cx   = st->jr_x + JR_WIDTH * 0.5f;
    float jr_feet = st->jr_y + JR_HEIGHT;

    int   best   = -1;
    float best_dx = HOOK_RADIUS + 1.0f;

    for (int i = 0; i < NUM_LIANAS; ++i) {
        float l_top, l_bottom;
        liana_bounds(i, &l_top, &l_bottom);

        // ¿está Jr a la altura vertical de la liana?
        if (jr_feet < l_top || jr_feet > l_bottom)
            continue;

        float dx = fabsf(jr_cx - lianas[i].x);
        if (dx < best_dx && dx <= HOOK_RADIUS) {
            best_dx = dx;
            best = i;
        }
    }
    return best;
}

static void enganchar_a_liana(GameState* st, int idx) {
    if (idx < 0 || idx >= NUM_LIANAS) return;

    st->jr_mode = JR_MODE_VINE;
    st->vine_idx = idx;
    st->jr_vx = 0;
    st->jr_vy = 0;

    float x = lianas[idx].x;
    if (st->jr_facing == JR_FACE_LEFT) {
        st->jr_x = x - JR_WIDTH;
    } else {
        st->jr_x = x;
    }

    float l_top, l_bottom;
    liana_bounds(idx, &l_top, &l_bottom);

    float jr_feet = st->jr_y + JR_HEIGHT;
    if (jr_feet < l_top)
        st->jr_y = l_top - JR_HEIGHT;
    else if (jr_feet > l_bottom)
        st->jr_y = l_bottom - JR_HEIGHT;
}

// Ajusta jr_y / jr_vy / on_ground si aterriza sobre una plataforma.
static void aplicar_colision_plataformas(GameState* st, float old_x, float old_y) {
    st->on_ground = 0;  // asumimos aire; si aterriza, la ponemos en 1

    float jr_left_old   = old_x;
    float jr_right_old  = old_x + JR_WIDTH;
    float jr_top_old    = old_y;
    float jr_bottom_old = old_y + JR_HEIGHT;

    float jr_left_new   = st->jr_x;
    float jr_right_new  = st->jr_x + JR_WIDTH;
    float jr_top_new    = st->jr_y;
    float jr_bottom_new = st->jr_y + JR_HEIGHT;

    for (int i = 0; i < NUM_PLATFORMS; ++i) {
        const Platform* p = &platforms[i];

        float plat_left   = p->x;
        float plat_right  = p->x + p->w;
        float plat_top    = p->y;
        float plat_bottom = p->y + p->h;

        // ¿Hay traslape vertical y horizontal en la posición nueva?
        int overlap_x = (jr_right_new > plat_left && jr_left_new < plat_right);
        int overlap_y = (jr_bottom_new > plat_top && jr_top_new < plat_bottom);

        // --- A) Aterrizar sobre la plataforma (suelo) ---
        if (st->jr_vy > 0.0f) { // venía cayendo
            // pies pasan de arriba del top a abajo del top
            if (jr_bottom_old <= plat_top && jr_bottom_new >= plat_top && overlap_x) {
                st->jr_y      = plat_top - JR_HEIGHT;
                st->jr_vy     = 0.0f;
                st->on_ground = 1;
                // actualiza valores nuevos para no liar las otras colisiones
                jr_top_new    = st->jr_y;
                jr_bottom_new = st->jr_y + JR_HEIGHT;
                continue;
            }
        }

        // --- B) Techo (solo para plataformas, NO tierras) ---
        if (p->tipo == PLAT_TIPO_PLATAFORMA && st->jr_vy < 0.0f) { // subiendo
            // cabeza pasa de debajo del bottom a encima del bottom
            if (jr_top_old >= plat_bottom && jr_top_new <= plat_bottom && overlap_x) {
                st->jr_y  = plat_bottom;
                st->jr_vy = 0.0f;
                jr_top_new    = st->jr_y;
                jr_bottom_new = st->jr_y + JR_HEIGHT;
                continue;
            }
        }

        // --- C) Paredes izquierda/derecha (tanto tierras como plataformas) ---
        if (overlap_y) {
            // moviéndose hacia la derecha -> choca con lado izquierdo
            if (st->jr_vx > 0.0f &&
                jr_right_old <= plat_left && jr_right_new >= plat_left) {

                st->jr_x  = plat_left - JR_WIDTH;
                st->jr_vx = 0.0f;
                jr_left_new  = st->jr_x;
                jr_right_new = st->jr_x + JR_WIDTH;
                continue;
            }

            // moviéndose hacia la izquierda -> choca con lado derecho
            if (st->jr_vx < 0.0f &&
                jr_left_old >= plat_right && jr_left_new <= plat_right) {

                st->jr_x  = plat_right;
                st->jr_vx = 0.0f;
                jr_left_new  = st->jr_x;
                jr_right_new = st->jr_x + JR_WIDTH;
                continue;
            }
        }
    }
}



