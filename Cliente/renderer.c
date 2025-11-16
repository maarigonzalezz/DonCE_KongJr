#include "renderer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//  Constantes del nivel (deben coincidir con el servidor)
#define TILE_SIZE 16
#define LEVEL_COLS 15
#define LEVEL_ROWS 20
#define SCALE 2.0f

//  Parser JSON simple
static char* extraer_string(const char* json, const char* clave) {
    char patron[64];
    snprintf(patron, sizeof(patron), "\"%s\":\"", clave);
    char* inicio = strstr(json, patron);
    if (!inicio) return NULL;

    inicio += strlen(patron);
    char* fin = strchr(inicio, '"');
    if (!fin) return NULL;

    int len = fin - inicio;
    char* resultado = (char*)malloc(len + 1);
    strncpy(resultado, inicio, len);
    resultado[len] = '\0';
    return resultado;
}

static float extraer_float(const char* json, const char* clave) {
    char patron[64];
    snprintf(patron, sizeof(patron), "\"%s\":", clave);
    char* inicio = strstr(json, patron);
    if (!inicio) return 0.0f;

    inicio += strlen(patron);
    return (float)atof(inicio);
}

static int extraer_int(const char* json, const char* clave) {
    char patron[64];
    snprintf(patron, sizeof(patron), "\"%s\":", clave);
    char* inicio = strstr(json, patron);
    if (!inicio) return 0;

    inicio += strlen(patron);
    return atoi(inicio);
}

//  Función para contar entidades en el array
static int contar_entidades(const char* json) {
    int count = 0;
    const char* ptr = json;

    // Buscar el inicio del array de entidades
    ptr = strstr(json, "\"entidades\":[");
    if (!ptr) return 0;
    ptr += 12; // Saltar "\"entidades\":["

    // Contar objetos en el array
    while (*ptr && *ptr != ']') {
        if (*ptr == '{') {
            count++;
            // Avanzar hasta el cierre de este objeto
            while (*ptr && *ptr != '}' && *ptr != ']') ptr++;
        }
        if (*ptr) ptr++;
    }

    return count;
}

//  Función para extraer una entidad específica del array
static int extraer_entidad(const char* json, int index, Entidad* entidad) {
    const char* ptr = json;

    // Buscar el array de entidades
    ptr = strstr(json, "\"entidades\":[");
    if (!ptr) return 0;
    ptr += 12;

    // Buscar la entidad en el índice especificado
    int current_index = 0;
    while (*ptr && *ptr != ']' && current_index <= index) {
        if (*ptr == '{' && current_index == index) {
            // Encontramos nuestra entidad
            char* tipo = extraer_string(ptr, "tipo");
            if (tipo) {
                strncpy(entidad->tipo, tipo, 31);
                entidad->tipo[31] = '\0';
                free(tipo);
            }

            entidad->x = extraer_float(ptr, "x");
            entidad->y = extraer_float(ptr, "y");
            entidad->w = extraer_float(ptr, "w");
            entidad->h = extraer_float(ptr, "h");

            return 1; // Éxito
        }
        else if (*ptr == '{') {
            current_index++;
        }
        ptr++;
    }

    return 0; // No se encontró
}

//  Función para construir el nivel visual (plataformas y lianas)
void renderer_build_level(Renderer* r) {
    // Liberar tiles anteriores si existen
    if (r->estado.tiles) {
        free(r->estado.tiles);
        r->estado.tiles = NULL;
        r->estado.num_tiles = 0;
    }

    //  Definir las plataformas (basado en el servidor)
    int plataformas[][3] = {
        {1, 15, 1},   // plataforma completa inferior
        {3, 5, 4},    {10, 12, 4},
        {2, 3, 7},    {6, 8, 7},   {12, 13, 7},
        {5, 6, 10},   {9, 11, 10},
        {2, 4, 13},   {7, 9, 13},  {12, 14, 13},
        {4, 6, 16},   {10, 12, 16},
        {2, 5, 19}    // zona de meta
    };
    int num_plataformas = sizeof(plataformas) / sizeof(plataformas[0]);

    //  Definir las lianas (col, rowStart, rowEnd)
    int lianas[][3] = {
        {2, 2, 18},
        {4, 5, 19},
        {7, 8, 16}, {7, 4, 7},
        {9, 11, 16}, {9, 7, 10},
        {12, 8, 19},
        {14, 2, 18}
    };
    int num_lianas = sizeof(lianas) / sizeof(lianas[0]);

    // Calcular total de tiles
    int total_tiles = 0;
    
    // Contar tiles de plataformas
    for (int i = 0; i < num_plataformas; i++) {
        total_tiles += (plataformas[i][1] - plataformas[i][0] + 1);
    }
    
    // Contar tiles de lianas
    for (int i = 0; i < num_lianas; i++) {
        total_tiles += (lianas[i][2] - lianas[i][1] + 1);
    }

    // Asignar memoria para tiles
    r->estado.tiles = (Tile*)malloc(total_tiles * sizeof(Tile));
    r->estado.num_tiles = 0;

    //  Crear tiles de PLATAFORMAS
    for (int i = 0; i < num_plataformas; i++) {
        int col_start = plataformas[i][0];
        int col_end = plataformas[i][1];
        int row = plataformas[i][2];
        
        for (int col = col_start; col <= col_end; col++) {
            Tile* t = &r->estado.tiles[r->estado.num_tiles++];
            t->col = col;
            t->row = row;
            strcpy(t->tipo, "PLATAFORMA");
            
            // Calcular posición y tamaño
            t->rect.x = col * TILE_SIZE * SCALE;
            t->rect.y = r->height - (row * TILE_SIZE * SCALE);
            t->rect.w = TILE_SIZE * SCALE;
            t->rect.h = TILE_SIZE * SCALE;
            
            // Color marrón para plataformas
            t->color = (SDL_Color){139, 69, 19, 255};
        }
    }

    //  Crear tiles de LIANAS
    for (int i = 0; i < num_lianas; i++) {
        int col = lianas[i][0];
        int row_start = lianas[i][1];
        int row_end = lianas[i][2];
        
        for (int row = row_start; row <= row_end; row++) {
            Tile* t = &r->estado.tiles[r->estado.num_tiles++];
            t->col = col;
            t->row = row;
            strcpy(t->tipo, "LIANA");
            
            // Calcular posición y tamaño (más delgadas que las plataformas)
            t->rect.x = (col * TILE_SIZE + TILE_SIZE/4) * SCALE;
            t->rect.y = r->height - (row * TILE_SIZE * SCALE);
            t->rect.w = (TILE_SIZE / 2) * SCALE;
            t->rect.h = TILE_SIZE * SCALE;
            
            // Color verde para lianas
            t->color = (SDL_Color){0, 128, 0, 255};
        }
    }

    printf("🎮 Nivel construido: %d tiles (%d plataformas, %d lianas)\n", 
           total_tiles, num_plataformas, num_lianas);
}

bool renderer_init(Renderer* r, const char* title, int w, int h) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    r->window = SDL_CreateWindow(title, w, h, 0);
    if (!r->window) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        return false;
    }

    r->renderer = SDL_CreateRenderer(r->window, NULL);
    if (!r->renderer) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        return false;
    }

    r->width = w;
    r->height = h;

    // Inicializar estado del juego
    r->estado.entidades = NULL;
    r->estado.num_entidades = 0;
    r->estado.tiles = NULL;        //  Inicializar tiles
    r->estado.num_tiles = 0;
    r->estado.score = 0;
    r->estado.vidas = 2;
    r->estado.speedFactor = 1.0f;
    r->estado.necesita_redibujar = true;

    //  Construir el nivel
    renderer_build_level(r);

    printf("🎮 Renderer inicializado: %dx%d\n", w, h);
    return true;
}

void renderer_shutdown(Renderer* r) {
    if (r->estado.entidades) {
        free(r->estado.entidades);
        r->estado.entidades = NULL;
    }
    
    if (r->estado.tiles) {         //  Liberar tiles
        free(r->estado.tiles);
        r->estado.tiles = NULL;
    }

    if (r->renderer) SDL_DestroyRenderer(r->renderer);
    if (r->window) SDL_DestroyWindow(r->window);
    SDL_Quit();
}

void renderer_update_state(Renderer* r, const char* snapshot_json) {
    if (!snapshot_json) {
        printf(" JSON es NULL\n");
        return;
    }

    if (strstr(snapshot_json, "snapshot") == NULL) {
        printf(" No es un snapshot: %s\n", snapshot_json);
        return;
    }

    printf(" Procesando snapshot...\n");

    // Extraer datos básicos
    r->estado.score = extraer_int(snapshot_json, "score");
    r->estado.vidas = extraer_int(snapshot_json, "lifes");
    r->estado.speedFactor = extraer_float(snapshot_json, "speedFactor");

    printf(" Score: %d, Vidas: %d\n", r->estado.score, r->estado.vidas);

    // Contar entidades
    int count = contar_entidades(snapshot_json);
    printf(" Encontradas %d entidades\n", count);

    // Liberar memoria anterior
    if (r->estado.entidades) {
        free(r->estado.entidades);
    }

    if (count > 0) {
        r->estado.entidades = (Entidad*)malloc(count * sizeof(Entidad));
        r->estado.num_entidades = count;

        // Extraer cada entidad
        for (int i = 0; i < count; i++) {
            if (extraer_entidad(snapshot_json, i, &r->estado.entidades[i])) {
                //  Configurar rectángulo para renderizado (USANDO LAS MISMAS CONSTANTES)
                r->estado.entidades[i].rect.x = (r->estado.entidades[i].x - r->estado.entidades[i].w/2) * SCALE;
                r->estado.entidades[i].rect.y = r->height - (r->estado.entidades[i].y * SCALE);
                r->estado.entidades[i].rect.w = r->estado.entidades[i].w * SCALE;
                r->estado.entidades[i].rect.h = r->estado.entidades[i].h * SCALE;

                // Asignar colores
                if (strstr(r->estado.entidades[i].tipo, "DKJr")) {
                    r->estado.entidades[i].color = (SDL_Color){255, 200, 0, 255};
                    printf("🟡 DKJr: (%.1f, %.1f)\n", r->estado.entidades[i].x, r->estado.entidades[i].y);
                } else if (strstr(r->estado.entidades[i].tipo, "Rojo")) {
                    r->estado.entidades[i].color = (SDL_Color){255, 0, 0, 255};
                } else if (strstr(r->estado.entidades[i].tipo, "Azul")) {
                    r->estado.entidades[i].color = (SDL_Color){0, 0, 255, 255};
                } else {
                    r->estado.entidades[i].color = (SDL_Color){255, 255, 255, 255};
                }
            }
        }
    } else {
        r->estado.entidades = NULL;
        r->estado.num_entidades = 0;
    }

    r->estado.necesita_redibujar = true;
    printf(" Estado actualizado\n");
}

void renderer_draw(Renderer* r) {
    // Fondo negro
    SDL_SetRenderDrawColor(r->renderer, 0, 0, 0, 255);
    SDL_RenderClear(r->renderer);

    //  1. Dibujar PLATAFORMAS (fondo)
    for (int i = 0; i < r->estado.num_tiles; i++) {
        if (strstr(r->estado.tiles[i].tipo, "PLATAFORMA")) {
            SDL_SetRenderDrawColor(r->renderer,
                                  r->estado.tiles[i].color.r,
                                  r->estado.tiles[i].color.g,
                                  r->estado.tiles[i].color.b,
                                  r->estado.tiles[i].color.a);
            SDL_RenderFillRect(r->renderer, &r->estado.tiles[i].rect);
            
            // Borde más oscuro para plataformas
            SDL_SetRenderDrawColor(r->renderer, 101, 67, 33, 255);
            SDL_RenderRect(r->renderer, &r->estado.tiles[i].rect);
        }
    }

    //  2. Dibujar LIANAS (sobre plataformas)
    for (int i = 0; i < r->estado.num_tiles; i++) {
        if (strstr(r->estado.tiles[i].tipo, "LIANA")) {
            SDL_SetRenderDrawColor(r->renderer,
                                  r->estado.tiles[i].color.r,
                                  r->estado.tiles[i].color.g,
                                  r->estado.tiles[i].color.b,
                                  r->estado.tiles[i].color.a);
            SDL_RenderFillRect(r->renderer, &r->estado.tiles[i].rect);
            
            // Borde más oscuro para lianas
            SDL_SetRenderDrawColor(r->renderer, 0, 100, 0, 255);
            SDL_RenderRect(r->renderer, &r->estado.tiles[i].rect);
        }
    }

    // 3. Dibujar ENTIDADES (sobre todo)
    for (int i = 0; i < r->estado.num_entidades; i++) {
        SDL_SetRenderDrawColor(r->renderer,
                              r->estado.entidades[i].color.r,
                              r->estado.entidades[i].color.g,
                              r->estado.entidades[i].color.b,
                              r->estado.entidades[i].color.a);
        SDL_RenderFillRect(r->renderer, &r->estado.entidades[i].rect);

        // Borde blanco para mejor visibilidad
        SDL_SetRenderDrawColor(r->renderer, 255, 255, 255, 255);
        SDL_RenderRect(r->renderer, &r->estado.entidades[i].rect);
    }

    // HUD básico
    SDL_SetRenderDrawColor(r->renderer, 255, 255, 255, 255);
    SDL_FRect hud = {10, 10, 150, 40};
    SDL_RenderRect(r->renderer, &hud);

    SDL_RenderPresent(r->renderer);
    r->estado.necesita_redibujar = false;

    //printf(" Frame dibujado (%d entidades)\n", r->estado.num_entidades);
}

bool renderer_needs_redraw(Renderer* r) {
    return r->estado.necesita_redibujar;
}