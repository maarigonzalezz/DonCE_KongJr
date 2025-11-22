#ifndef JUEGO_H
#define JUEGO_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <winsock2.h>
#include "constants.h"

// ------------------------------ STRUCTS --------------------
// para lo que es la ventana de juego y renderizar
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

// Opciones del espectador
typedef enum {
    WHICH_NONE = 0,
    WHICH_A,
    WHICH_B
} WhichOpcion;

// Como puede estar JR
typedef enum {
    JR_MODE_GROUND,   // está en plataforma / tierra
    JR_MODE_VINE      // está agarrado de una liana
} JrMode;

// Razones de muerte
typedef enum {
    DEATH_NONE = 0,
    DEATH_WATER,
    DEATH_CROC
} DeathReason;

// Facing del mono
typedef enum {
    JR_FACE_RIGHT,
    JR_FACE_LEFT
} JrFacing;

// tipos de plataforma en el juego
typedef enum {
    PLAT_TIPO_TIERA,      // solo piso (no techo)
    PLAT_TIPO_PLATAFORMA  // piso + techo (para las que tienen liana pegada)
} PlatformTipo;

// plataforma en sí
typedef struct {
    float x, y, w, h;
    PlatformTipo tipo;
} Platform;

// struct de una liana
typedef struct {
    float x;
    float top;
    float bottom;
} Liana;

typedef struct {
    char  id[64];   // UUID como string
    float x, y;
    int   activa;   // 1 si existe, 0 si no
} Fruta;

typedef enum {
    CROC_AZUL = 0,
    CROC_ROJO = 1
} CrocTipo;

typedef struct {
    char  id[64];
    float x, y;
    CrocTipo tipo;
    int   activo;
} Cocodrilo;

// STRUCT CON EL ESTADO DEL JUEGO SUPER IMPORTANTE
typedef struct {
    float jr_x, jr_y; // posicion de Jr
    float jr_vx, jr_vy;

    JrMode   jr_mode; // piso o liana
    JrFacing jr_facing; // izquierda o derecha de la liana
    int    vine_idx;  // índice en el arreglo lianas, -1 si ninguna

    int   on_ground;    // 1 si está parado sobre plataforma/piso, 0 si está en el aire

    int vidas; // numero de vidas
    int score; // puntaje
    float speed; // velocidad de enemigos

    DeathReason pending_death; // manejo de muerte

    char  partida[8];

    // Entidades
    // NUEVO: entidades que vienen del servidor
    Fruta     frutas[MAX_FRUTAS];
    int       num_frutas;

    Cocodrilo cocodrilos[MAX_COCODRILOS];
    int       num_cocodrilos;


} GameState;


// ----------------------------- FUNCIONES Y DEMAS --------------------------------
bool juego_init(Juego* j, const char* title, int w, int h);
void juego_shutdown(Juego* j);

MenuOpcion juego_menu(Juego* j);
WhichOpcion juego_menu_which(Juego* j, int tieneA, int tieneB);

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