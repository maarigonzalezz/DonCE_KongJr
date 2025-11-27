#include "juego.h"
#include "constants.h"
#include <stdio.h>
#include <math.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "mensajes.h"

// Helpers simples para botón
static SDL_FRect btn1 = { BTN1_X, BTN1_Y, BTN1_W, BTN1_H };
static SDL_FRect btn2 = { BTN2_X, BTN2_Y, BTN2_W, BTN2_H };

// Array de lianas que son FIJAS a lo que nos piden en el juego clasico
const Liana lianas[NUM_LIANAS] = {
    { LIANA1_X,  LIANA1_TOP_Y, LIANA1_BOT_Y },
    { LIANA2_X,  LIANA1_TOP_Y, LIANA2_BOT_Y },
    { LIANA3_X,  LIANA1_TOP_Y, LIANA3_BOT_Y },
    { LIANA4_X,  LIANA1_TOP_Y, LIANA4_BOT_Y },
    { LIANA5_X,  LIANA5_TOP_Y, LIANA5_BOT_Y },
    { LIANA6_X,  LIANA6_TOP_Y, LIANA6_BOT_Y },
    { LIANA7_X,  LIANA7_TOP_Y, LIANA7_BOT_Y },
    { LIANA8_X,  LIANA7_TOP_Y, LIANA8_BOT_Y },
    { LIANA9_X,  LIANA9_TOP_Y, LIANA9_BOT_Y },
    { LIANA10_X, LIANA9_TOP_Y, LIANA9_BOT_Y },
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

// ==================== FUNCIONES DE SPRITES ====================

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* filename) {
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        fprintf(stderr, "No se pudo cargar %s: %s\n",
                filename, SDL_GetError());   // <-- antes IMG_GetError()
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!texture) {
        fprintf(stderr, "No se pudo crear textura: %s\n", SDL_GetError());
        return NULL;
    }

    printf("Textura cargada: %s\n", filename);
    return texture;
}
// ==================== FUNCIONES DE jr ====================
bool load_jr_sprites(SDL_Renderer* renderer, GameState* st) {
    const char* sprite_files[JR_NUM_SPRITES] = {
        "sprites/jr_frente.png",
        "sprites/jr_liana_izq.png",
        "sprites/jr_liana_der.png"
    };

    for (int i = 0; i < JR_NUM_SPRITES; i++) {
        st->jr_sprites[i] = load_texture(renderer, sprite_files[i]);
        if (!st->jr_sprites[i]) {
            fprintf(stderr, "Error cargando sprite: %s\n", sprite_files[i]);
            return false;
        }
    }

    st->current_sprite = JR_SPRITE_FRENTE;
    return true;
}

void update_jr_sprite(GameState* st) {
    if (st->jr_mode == JR_MODE_VINE) {
        if (st->jr_facing == JR_FACE_LEFT) {
            st->current_sprite = JR_SPRITE_LIANA_IZQ;
        } else {
            st->current_sprite = JR_SPRITE_LIANA_DER;
        }
    } else {
        st->current_sprite = JR_SPRITE_FRENTE;
    }
}

void free_jr_sprites(GameState* st) {
    for (int i = 0; i < JR_NUM_SPRITES; i++) {
        if (st->jr_sprites[i]) {
            SDL_DestroyTexture(st->jr_sprites[i]);
            st->jr_sprites[i] = NULL;
        }
    }
}
// ==================== FUNCIONES DE COCODRILOS ====================

bool load_croc_sprites(SDL_Renderer* renderer, GameState* st) {
    const char* sprite_files[CROC_NUM_SPRITES] = {
        "sprites/CocodriloRojo.png",
        "sprites/CocodriloAzul.png"
    };

    const char* sprite_names[CROC_NUM_SPRITES] = {
        "ROJO", "AZUL"
    };

    for (int i = 0; i < CROC_NUM_SPRITES; i++) {
        printf("Cargando cocodrilo %d: %s...\n", i, sprite_names[i]);
        st->croc_sprites[i] = load_texture(renderer, sprite_files[i]);
        if (!st->croc_sprites[i]) {
            fprintf(stderr, "ERROR cargando cocodrilo %s: %s\n",
                    sprite_names[i], sprite_files[i]);
            return false;
        } else {
            printf("✓ Cocodrilo %s cargado exitosamente\n", sprite_names[i]);
        }
    }

    printf("Todos los sprites de cocodrilos cargados\n");
    return true;
}

void update_croc_sprite(Cocodrilo* croc) {
    if (croc->tipo == CROC_AZUL) {
        croc->current_sprite = CROC_SPRITE_AZUL;
    } else {
        croc->current_sprite = CROC_SPRITE_ROJO;
    }
}

void free_croc_sprites(GameState* st) {
    for (int i = 0; i < CROC_NUM_SPRITES; i++) {
        if (st->croc_sprites[i]) {
            SDL_DestroyTexture(st->croc_sprites[i]);
            st->croc_sprites[i] = NULL;
        }
    }
    printf("Sprites de cocodrilos liberados\n");
}

// ==================== FUNCIONES DE FRUTAS ====================

bool load_fruit_sprites(SDL_Renderer* renderer, GameState* st) {
    const char* sprite_files[FRUIT_NUM_SPRITES] = {
        "sprites/fruta.png"
    };

    const char* sprite_names[FRUIT_NUM_SPRITES] = {
        "NORMAL"
    };

    for (int i = 0; i < FRUIT_NUM_SPRITES; i++) {
        printf("Cargando fruta %d: %s...\n", i, sprite_names[i]);
        st->fruit_sprites[i] = load_texture(renderer, sprite_files[i]);
        if (!st->fruit_sprites[i]) {
            fprintf(stderr, "ERROR cargando fruta %s: %s\n",
                    sprite_names[i], sprite_files[i]);
            return false;
        } else {
            printf("✓ Fruta %s cargada exitosamente\n", sprite_names[i]);
        }
    }

    printf("Todos los sprites de frutas cargados\n");
    return true;
}

void update_fruit_sprite(Fruta* fruit) {
    fruit->current_sprite = FRUIT_SPRITE_NORMAL;
}

void free_fruit_sprites(GameState* st) {
    for (int i = 0; i < FRUIT_NUM_SPRITES; i++) {
        if (st->fruit_sprites[i]) {
            SDL_DestroyTexture(st->fruit_sprites[i]);
            st->fruit_sprites[i] = NULL;
        }
    }
    printf("Sprites de frutas liberados\n");
}

// ==================== FUNCIONES DE MARIO ====================

bool load_mario_sprites(SDL_Renderer* renderer, GameState* st) {
    const char* filename = "sprites/Mario.png";

    SDL_Texture* tex = load_texture(renderer, filename);
    if (!tex) {
        fprintf(stderr, "ERROR cargando sprite de Mario: %s\n", filename);
        return false;
    }

    st->mario_sprites[MARIO_SPRITE_NORMAL] = tex;
    printf("Sprite de Mario cargado exitosamente: %s\n", filename);
    return true;
}

void free_mario_sprites(GameState* st) {
    for (int i = 0; i < MARIO_NUM_SPRITES; ++i) {
        if (st->mario_sprites[i]) {
            SDL_DestroyTexture(st->mario_sprites[i]);
            st->mario_sprites[i] = NULL;
        }
    }
    printf("Sprites de Mario liberados\n");
}

// ==================== FUNCIONES DE KONG ====================

bool load_kong_sprites(SDL_Renderer* renderer, GameState* st) {
    const char* filenames[KONG_NUM_SPRITES] = {
        "sprites/kong_encerrado.png", // KONG_SPRITE_ENCERRADO
        "sprites/kong_libre.png"      // KONG_SPRITE_LIBRE
    };

    for (int i = 0; i < KONG_NUM_SPRITES; ++i) {
        st->kong_sprites[i] = load_texture(renderer, filenames[i]);
        if (!st->kong_sprites[i]) {
            fprintf(stderr, "ERROR cargando sprite de Kong: %s\n", filenames[i]);
            return false;
        }
    }

    printf("Sprites de Kong cargados correctamente\n");
    return true;
}

void free_kong_sprites(GameState* st) {
    for (int i = 0; i < KONG_NUM_SPRITES; ++i) {
        if (st->kong_sprites[i]) {
            SDL_DestroyTexture(st->kong_sprites[i]);
            st->kong_sprites[i] = NULL;
        }
    }
    printf("Sprites de Kong liberados\n");
}





// ==================== FUNCIONES DEL JUEGO ====================

bool juego_init(Juego* j, const char* title, int w, int h){
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

// ==================== FUNCIONES DE MENÚ ====================

static void draw_menu(Juego* j){
    SDL_SetRenderDrawColor((SDL_Renderer*)j->renderer, 20, 24, 28, 255);
    SDL_RenderClear((SDL_Renderer*)j->renderer);

    SDL_SetRenderDrawColor((SDL_Renderer*)j->renderer, 100, 180, 100, 255);
    SDL_RenderFillRect((SDL_Renderer*)j->renderer, &btn1);

    SDL_SetRenderDrawColor((SDL_Renderer*)j->renderer, 100, 140, 200, 255);
    SDL_RenderFillRect((SDL_Renderer*)j->renderer, &btn2);

    SDL_RenderPresent((SDL_Renderer*)j->renderer);
}

static int hit(SDL_FRect r, float x, float y){
    return (x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h);
}

MenuOpcion juego_menu(Juego* j){
    int running = 1;
    MenuOpcion opcion = MENU_OPCION_NINGUNA;
    SDL_Event ev;

    while (running){
        while (SDL_PollEvent(&ev)){
            if (ev.type == SDL_EVENT_QUIT){
                running = 0;
            } else if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
                float mx = (float)ev.button.x, my = (float)ev.button.y;
                if (hit(btn1, mx, my)){
                    opcion = MENU_OPCION_JUGADOR;
                    running = 0;
                } else if (hit(btn2, mx, my)){
                    opcion = MENU_OPCION_ESPECTADOR;
                    running = 0;
                }
            }
        }
        draw_menu(j);
        SDL_Delay(16);
    }

    return opcion;
}

// ==================== MENÚ ESPECTADOR ====================

static SDL_FRect btnA_rect = { WHICH_BTN_A_X, WHICH_BTN_A_Y, WHICH_BTN_A_W, WHICH_BTN_A_H };
static SDL_FRect btnB_rect = { WHICH_BTN_B_X, WHICH_BTN_B_Y, WHICH_BTN_B_W, WHICH_BTN_B_H };

static void draw_menu_which(Juego* j, int tieneA, int tieneB) {
    SDL_Renderer* r = (SDL_Renderer*)j->renderer;

    SDL_SetRenderDrawColor(r, 20, 24, 28, 255);
    SDL_RenderClear(r);

    if (tieneA) {
        SDL_SetRenderDrawColor(r, 180, 180, 80, 255);
        SDL_RenderFillRect(r, &btnA_rect);
    }

    if (tieneB) {
        SDL_SetRenderDrawColor(r, 80, 140, 200, 255);
        SDL_RenderFillRect(r, &btnB_rect);
    }

    SDL_RenderPresent(r);
}

WhichOpcion juego_menu_which(Juego* j, int tieneA, int tieneB) {
    int running = 1;
    SDL_Event ev;
    WhichOpcion opcion = WHICH_NONE;

    if (!tieneA && !tieneB) {
        return WHICH_NONE;
    }

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                running = 0;
                opcion = WHICH_NONE;
                break;
            } else if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = (float)ev.button.x;
                float my = (float)ev.button.y;

                if (tieneA && hit(btnA_rect, mx, my)) {
                    opcion = WHICH_A;
                    running = 0;
                    break;
                }

                if (tieneB && hit(btnB_rect, mx, my)) {
                    opcion = WHICH_B;
                    running = 0;
                    break;
                }
            }
        }

        draw_menu_which(j, tieneA, tieneB);
        SDL_Delay(16);
    }
    return opcion;
}

// ==================== GAME OVER ====================

void juego_mostrar_game_over(Juego* j, const GameState* st) {
    SDL_Renderer* r = (SDL_Renderer*)j->renderer;
    int running = 1;

    SDL_FRect btn_volver = {
        WINDOW_WIDTH * 0.5f - 90.0f,
        WINDOW_HEIGHT * 0.5f + 40.0f,
        180.0f,
        40.0f
    };

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                running = 0;
            } else if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = (float)ev.button.x;
                float my = (float)ev.button.y;

                if (mx >= btn_volver.x &&
                    mx <= btn_volver.x + btn_volver.w &&
                    my >= btn_volver.y &&
                    my <= btn_volver.y + btn_volver.h) {
                    running = 0;
                }
            }
        }

        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderClear(r);

        SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        float cx = WINDOW_WIDTH * 0.5f - 60.0f;
        float cy = WINDOW_HEIGHT * 0.5f - 30.0f;
        draw_text(r, cx,     cy,     "GAME", 3.0f);
        draw_text(r, cx + 5, cy+30,  "OVER", 3.0f);

        char buf[64];
        snprintf(buf, sizeof(buf), "S%d", st->score);
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        draw_text(r, cx, cy + 80, buf, 2.0f);

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

// ==================== FUNCIONES DE TEXTO ====================

static const Glyph* find_glyph(char c) {
    int n = (int)(sizeof(g_glyphs)/sizeof(g_glyphs[0]));
    for (int i = 0; i < n; ++i) {
        if (g_glyphs[i].ch == c) return &g_glyphs[i];
    }
    return NULL;
}

static void draw_char(SDL_Renderer* r, float x, float y, char c, float scale) {
    const Glyph* g = find_glyph(c);
    if (!g) return;

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

static void draw_text(SDL_Renderer* r, float x, float y, const char* text, float scale) {
    float advance = 4.0f * 2.0f * scale;
    for (const char* p = text; *p; ++p) {
        if (*p == ' ') {
            x += advance;
            continue;
        }
        draw_char(r, x, y, *p, scale);
        x += advance;
    }
}

// ==================== RENDER SCENE ====================

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

    SDL_SetRenderDrawColor(r, 180, 100, 40, 255);
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
    SDL_SetRenderDrawColor(r, 200, 40, 40, 255);
    SDL_RenderFillRect(r, &dk);

    // --- Dibujar Kong (Donkey) dentro o fuera de la jaula ---
    SDL_FRect kong_rect = { DK_CAGE_X, DK_CAGE_Y, DK_CAGE_W, DK_CAGE_H };

    // Mientras NO se haya ganado, Kong encerrado; cuando se gana, libre
    KongSpriteState kong_state =
        (st->kong_free_timer > 0.0f ? KONG_SPRITE_LIBRE : KONG_SPRITE_ENCERRADO);

    SDL_Texture* kong_tex = st->kong_sprites[kong_state];

    if (kong_tex) {
        SDL_RenderTexture(r, kong_tex, NULL, &kong_rect);
    } else {
        // Si algo falla, dejamos solo la jaula roja (ya dibujada)

    }


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

    for (int i = 0; i < st->num_frutas; ++i) {
        if (!st->frutas[i].activa) continue;

      //  printf("Fruta %d: sprite_idx=%d, pos=(%.1f, %.1f)\n",
              // i, st->frutas[i].current_sprite, st->frutas[i].x, st->frutas[i].y);

        // DETERMINAR qué sprite usar
        FruitSpriteState sprite_a_usar = FRUIT_SPRITE_NORMAL;

        SDL_FRect fr = {
            st->frutas[i].x,
            st->frutas[i].y,
            FRUIT_S, FRUIT_S
        };

        // Verificar QUÉ textura vamos a usar
        SDL_Texture* fruit_texture = st->fruit_sprites[sprite_a_usar];
      //  printf("  Textura a usar: %p (índice %d)\n", fruit_texture, sprite_a_usar);

        if (fruit_texture) {
           // printf("  ✓ Renderizando fruta con textura\n");
            SDL_RenderTexture(r, fruit_texture, NULL, &fr);
        } else {
           // printf("  ✗ Textura NULA, usando cuadrado amarillo\n");
            // Fallback
            SDL_SetRenderDrawColor(r, 255, 215, 0, 255);
            SDL_RenderFillRect(r, &fr);
        }
    }
   // printf("=== FIN RENDER FRUTAS ===\n");

    // --- Dibjar Mario---
    SDL_FRect mario = { MARIO_X, MARIO_Y, MARIO_W, MARIO_H };
    SDL_Texture* mario_tex = st->mario_sprites[MARIO_SPRITE_NORMAL];

    if (mario_tex) {
        SDL_RenderTexture(r, mario_tex, NULL, &mario);
    } else {
        // fallback: si algo falla, dibujar el rectángulo rojo como antes
        SDL_SetRenderDrawColor(r, 200, 40, 40, 255);
        SDL_RenderFillRect(r, &mario);
    }

    // --- Dibujar cocodrilos ---

    //printf("Cocodrilos activos: %d\n", st->num_cocodrilos);

    for (int i = 0; i < st->num_cocodrilos; ++i) {
        if (!st->cocodrilos[i].activo) continue;



        // DETERMINAR qué sprite usar
        CrocSpriteState sprite_a_usar;
        if (st->cocodrilos[i].tipo == CROC_AZUL) {
            sprite_a_usar = CROC_SPRITE_AZUL;
        } else {
            sprite_a_usar = CROC_SPRITE_ROJO;
        }

        SDL_FRect cr = {
            st->cocodrilos[i].x,
            st->cocodrilos[i].y,
            CROC_W, CROC_H
        };

        // Verificar QUÉ textura se va a usar
        SDL_Texture* croc_texture = st->croc_sprites[sprite_a_usar];


        if (croc_texture) {
           // printf("  ✓ Renderizando con textura\n");
            SDL_RenderTexture(r, croc_texture, NULL, &cr);
        } else {
            printf("  ✗ Textura NULA, usando cuadrado de color\n");
            // Fallback
            if (st->cocodrilos[i].tipo == CROC_AZUL) {
                SDL_SetRenderDrawColor(r, 0, 0, 255, 255);
            } else {
                SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
            }
            SDL_RenderFillRect(r, &cr);
        }
    }
   // printf("=== FIN RENDER COCODRILOS ===\n");
    // --- Jr (jugador) ---
    SDL_FRect jr_rect = {
        st->jr_x,
        st->jr_y,
        JR_WIDTH,
        JR_HEIGHT
    };

    // Usar el sprite actual de DK Jr
    SDL_Texture* current_jr_texture = st->jr_sprites[st->current_sprite];
    if (current_jr_texture) {
        SDL_RenderTexture(r, current_jr_texture, NULL, &jr_rect);
    } else {
        // Fallback: mantener el rectángulo azul si no hay textura
        SDL_SetRenderDrawColor(r, 40, 120, 220, 255);
        SDL_RenderFillRect(r, &jr_rect);
    }

    // ================= HUD =================
    SDL_FRect hud = { 0, 0, (float)WINDOW_WIDTH, 30.0f };
    SDL_SetRenderDrawColor(r, 0, 0, 0, 200);
    SDL_RenderFillRect(r, &hud);

    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);

    char buf[64];
    float scale = 2.0f;

    snprintf(buf, sizeof(buf), "S%d", st->score);
    draw_text(r, 10.0f, 8.0f, buf, scale);

    snprintf(buf, sizeof(buf), "L%d", st->vidas);
    draw_text(r, 210.0f, 8.0f, buf, scale);

    char letra = (st->partida[0] != '\0') ? st->partida[0] : 'A';
    snprintf(buf, sizeof(buf), "P%c", letra);
    draw_text(r, 410.0f, 8.0f, buf, scale);

    SDL_RenderPresent(r);
}