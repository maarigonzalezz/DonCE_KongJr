#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include <stdbool.h>

// Estructura para una entidad del juego
typedef struct {
    char tipo[32];
    float x, y, w, h;
    SDL_FRect rect;
    SDL_Color color;
} Entidad;

//  Estructura para tiles del nivel
typedef struct {
    int col, row;
    char tipo[32];
    SDL_FRect rect;
    SDL_Color color;
} Tile;

// Estructura para el estado del juego
typedef struct {
    Entidad* entidades;
    int num_entidades;
    Tile* tiles;           //  Tiles
    int num_tiles;         //  Cantidad de tiles
    int score;
    int vidas;
    float speedFactor;
    bool necesita_redibujar;
} EstadoJuego;

// Estructura del renderizador
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int width, height;
    EstadoJuego estado;
} Renderer;

// Funciones públicas del renderer
bool renderer_init(Renderer* r, const char* title, int w, int h);
void renderer_shutdown(Renderer* r);
void renderer_update_state(Renderer* r, const char* snapshot_json);
void renderer_draw(Renderer* r);
bool renderer_needs_redraw(Renderer* r);

//  función para construir el nivel visual
void renderer_build_level(Renderer* r);

#endif // RENDERER_H