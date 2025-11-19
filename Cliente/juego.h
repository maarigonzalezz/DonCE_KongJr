#ifndef JUEGO_H
#define JUEGO_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <winsock2.h>

typedef struct {
    void* window;
    void* renderer;
} Juego;

// Opciones del menú
typedef enum {
    MENU_OPCION_NINGUNA = 0,
    MENU_OPCION_JUGADOR = 1,
    MENU_OPCION_ESPECTADOR = 2
} MenuOpcion;

typedef enum {
    WHICH_NONE = 0,
    WHICH_A,
    WHICH_B
} WhichOpcion;

typedef enum {
    JR_MODE_GROUND,   // está en plataforma / tierra
    JR_MODE_VINE      // está agarrado de una liana
} JrMode;

typedef enum {
    JR_FACE_RIGHT,
    JR_FACE_LEFT
} JrFacing;

typedef enum {
    PLAT_TIPO_TIERA,      // solo piso (no techo)
    PLAT_TIPO_PLATAFORMA  // piso + techo (para las que tienen liana pegada)
} PlatformTipo;

typedef struct {
    float x, y, w, h;
    PlatformTipo tipo;
} Platform;

typedef struct {
    float x;
    float top;
    float bottom;
} Liana;

typedef struct {
    float jr_x, jr_y;
    float jr_vx, jr_vy;

    JrMode   jr_mode; // piso o liana
    JrFacing jr_facing; // izquierda o derecha de la liana
    int    vine_idx;  // índice en el arreglo lianas, -1 si ninguna

    int   on_ground;    // 1 si está parado sobre plataforma/piso, 0 si está en el aire

    int vidas;
    int score;
    float speed;
    // luego más cosas: cocodrilos, frutas, etc.
} GameState;

bool juego_init(Juego* j, const char* title, int w, int h);
void juego_shutdown(Juego* j);

MenuOpcion juego_menu(Juego* j);
WhichOpcion juego_menu_which(Juego* j, int tieneA, int tieneB);

// Lo exportamos para que juego.c pueda llamar:
int net_send_line(SOCKET s, const char* str);  // <- está definido en cliente.c

// pantalla de juego
void render_scene(Juego* j, const GameState* st, int es_jugador);
void game_loop_jugador(Juego* j, SOCKET sock, GameState* st);
void actualizar_logica_jugador(GameState* st, float dt);
void manejar_input_jugador(SDL_Event* ev, GameState* st);

// para el jugador:
static int buscar_liana_cercana(const GameState* st);
static void enganchar_a_liana(GameState* st, int idx);
static void liana_bounds(int idx, float* top, float* bottom);
static void aplicar_colision_plataformas(GameState* st, float old_x, float old_y);

#endif //JUEGO_H