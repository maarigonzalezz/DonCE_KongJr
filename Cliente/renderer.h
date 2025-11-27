#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include <stdbool.h>

// Estructura del renderizador
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int width, height;
} Renderer;



#endif // RENDERER_H