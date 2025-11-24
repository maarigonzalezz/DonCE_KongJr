#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SDL3/SDL.h>
#include <winsock2.h>

#include "constants.h"
#include "juego.h"
#include "mensajes.h"



static int rects_intersect(float ax, float ay, float aw, float ah,
                           float bx, float by, float bw, float bh)
{
    return !(ax + aw <= bx ||
             bx + bw <= ax ||
             ay + ah <= by ||
             by + bh <= ay);
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

        if (st->pending_death != DEATH_NONE)
        {
            enviar_muerte(sock, st->pending_death);

            // --------------------------------- RESPAWN BASICO HASTA QUE SE MANEJEN LOS MENSAJES BIEN------------------------
            // respawn básico mientras tanto:
            st->jr_x = JR_START_X;
            st->jr_y = JR_START_Y;
            st->jr_vx = st->jr_vy = 0;
            st->jr_mode = JR_MODE_GROUND;
            st->vine_idx = -1;
            st->on_ground = 1;

            st->pending_death = DEATH_NONE;
        }


        if (st->pending_fruit) {
            enviar_fruta_d(sock, st->pending_fruit_id);
            st->pending_fruit = 0;
            st->pending_fruit_id[0] = '\0';
        }

        if (st->pending_win) {
            enviar_win(sock);
            // respawn básico mientras tanto:
            st->jr_x = JR_START_X;
            st->jr_y = JR_START_Y;
            st->jr_vx = st->jr_vy = 0;
            st->jr_mode = JR_MODE_GROUND;
            st->vine_idx = -1;
            st->on_ground = 1;
            st->pending_win = 0;
        }

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

        // 6) Muerte por agua
        float feet = st->jr_y + JR_HEIGHT;
        if (feet >= WATER_Y && st->pending_death == DEATH_NONE) {
            st->pending_death = DEATH_WATER;
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

        // 3) También checar plataformas (para techos, etc.)
        aplicar_colision_plataformas(st, old_x, old_y);
    }

    // Mantener dentro de la pantalla en X
    if (st->jr_x < 0)
        st->jr_x = 0;
    if (st->jr_x + JR_WIDTH > WINDOW_WIDTH)
        st->jr_x = WINDOW_WIDTH - JR_WIDTH;

    // --- Colisión con frutas ---
    if (!st->pending_fruit) {  // solo si no hay una fruta pendiente de envío
        for (int i = 0; i < st->num_frutas; ++i) {
            Fruta* f = &st->frutas[i];
            if (!f->activa) continue;

            // Si ya reportamos esta fruta antes, la ignoramos
            if (st->last_fruit_destroyed_id[0] != '\0' &&
                strcmp(f->id, st->last_fruit_destroyed_id) == 0) {
                continue;
                }

            if (rects_intersect(st->jr_x, st->jr_y, JR_WIDTH, JR_HEIGHT,
                                f->x, f->y, FRUIT_S, FRUIT_S)) {

                st->pending_fruit = 1;
                strncpy(st->pending_fruit_id,
                        f->id,
                        sizeof(st->pending_fruit_id) - 1);
                st->pending_fruit_id[sizeof(st->pending_fruit_id) - 1] = '\0';

                // Recordar que esta fruta ya fue destruida
                strncpy(st->last_fruit_destroyed_id,
                        f->id,
                        sizeof(st->last_fruit_destroyed_id) - 1);
                st->last_fruit_destroyed_id[sizeof(st->last_fruit_destroyed_id) - 1] = '\0';

                printf(">> Colisión con fruta id=%s\n", st->pending_fruit_id);
                break;
                                }
        }
    }

    // --- Colisión con Cocodrilo = muerte ---
    if (st->pending_death == DEATH_NONE) {  // no spamear si ya está muriendo
        for (int i = 0; i < st->num_cocodrilos; ++i) {
            Cocodrilo* c = &st->cocodrilos[i];
            if (!c->activo) continue;

            if (rects_intersect(st->jr_x, st->jr_y, JR_WIDTH, JR_HEIGHT,
                                c->x, c->y, CROC_W, CROC_H)) {

                st->pending_death = DEATH_CROC;
                printf(">> Colisión con cocodrilo id=%s\n", c->id);
                break; // con uno basta para morir
            }
        }
    }

    // --- Colisión con Mario = muerte ---
    if (st->pending_death == DEATH_NONE) {
        if (rects_intersect(st->jr_x, st->jr_y, JR_WIDTH, JR_HEIGHT,
                            MARIO_X, MARIO_Y, MARIO_W, MARIO_H)) {

            st->pending_death = DEATH_MARIO;
            printf(">> Muerte por Mario\n");
                            }
    }

    // --- Colisión con jaula de Donkey = victoria ---
    if (!st->pending_win) {
        if (rects_intersect(st->jr_x, st->jr_y, JR_WIDTH, JR_HEIGHT,
                            DK_CAGE_X, DK_CAGE_Y, DK_CAGE_W, DK_CAGE_H)) {

            st->pending_win = 1;
            printf(">> Victoria: contacto con jaula de Donkey\n");
                            }
    }


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