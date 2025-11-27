#ifndef JUEGO_H
#define JUEGO_H

#include <SDL3/SDL.h>
#include <SDL3_image//SDL_image.h>
#include <stdbool.h>
#include <winsock2.h>
#include "constants.h"

extern int g_game_over;

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
    DEATH_CROC,
    DEATH_MARIO
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

extern const Platform platforms[NUM_PLATFORMS];
extern const Liana    lianas[NUM_LIANAS];

typedef struct {
    char  id[64];
    float x, y;
    FruitSpriteState current_sprite;  //
    int   activa;
} Fruta;

typedef enum {
    CROC_AZUL = 0,
    CROC_ROJO = 1
} CrocTipo;

typedef struct {
    char  id[64];
    float x, y;
    CrocTipo tipo;
    CrocSpriteState current_sprite;
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
    Fruta     frutas[MAX_FRUTAS];
    int       num_frutas;

    Cocodrilo cocodrilos[MAX_COCODRILOS];
    int       num_cocodrilos;

    int  pending_fruit;              // 0 = nada, 1 = hay fruta por reportar
    char pending_fruit_id[64];       // id de la fruta que tocamos
    char last_fruit_destroyed_id[64];

    int  pending_win;
    float kong_free_timer;  // segundos que Kong debe seguir libre


    // Sprites de DK Jr
    SDL_Texture* jr_sprites[JR_NUM_SPRITES];
    JrSpriteState current_sprite;
    // Sprites de cocodrilos
    SDL_Texture* croc_sprites[CROC_NUM_SPRITES];
    // Sprites de frutas
    SDL_Texture* fruit_sprites[FRUIT_NUM_SPRITES];
    // Sprites de Mario
    SDL_Texture* mario_sprites[MARIO_NUM_SPRITES];
    // Sprites de Kong
    SDL_Texture* kong_sprites[KONG_NUM_SPRITES];

} GameState;

// ----------------- MINI FUENTE PARA HUD -----------------

typedef struct {
    char ch;
    const char* pattern; // 3 columnas x 5 filas = 15 chars '0' o '1'
} Glyph;

static const Glyph g_glyphs[] = {
    // Dígitos
    { '0', "111" "101" "101" "101" "111" },
    { '1', "001" "001" "001" "001" "001" },
    { '2', "111" "001" "111" "100" "111" },
    { '3', "111" "001" "111" "001" "111" },
    { '4', "101" "101" "111" "001" "001" },
    { '5', "111" "100" "111" "001" "111" },
    { '6', "111" "100" "111" "101" "111" },
    { '7', "111" "001" "001" "001" "001" },
    { '8', "111" "101" "111" "101" "111" },
    { '9', "111" "101" "111" "001" "111" },

    // Letras que vamos a usar: S, L, P, A, B
    { 'S', "111" "100" "111" "001" "111" },
    { 'L', "100" "100" "100" "100" "111" },
    { 'P', "111" "101" "111" "100" "100" },
    { 'A', "111" "101" "111" "101" "101" },
    { 'B', "110" "101" "110" "101" "110" },
    { 'G', "111" "100" "101" "101" "111" },
    { 'M', "101" "111" "101" "101" "101" },
    { 'E', "111" "100" "111" "100" "111" },
    { 'O', "111" "101" "101" "101" "111" },
    { 'V', "101" "101" "101" "101" "010" },
    { 'R', "111" "101" "111" "101" "101" },
};


// ----------------------------- FUNCIONES Y DEMAS --------------------------------
bool juego_init(Juego* j, const char* title, int w, int h);
void juego_shutdown(Juego* j);
bool load_jr_sprites(SDL_Renderer* renderer, GameState* st);
//  Funciones de cocodrilos
bool load_croc_sprites(SDL_Renderer* renderer, GameState* st);
void update_croc_sprite(Cocodrilo* croc);
void free_croc_sprites(GameState* st);
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* filename);
//Funciones de jr
void update_jr_sprite(GameState* st);
void free_jr_sprites(GameState* st);
// Funciones de frutas
bool load_fruit_sprites(SDL_Renderer* renderer, GameState* st);
void update_fruit_sprite(Fruta* fruit);
void free_fruit_sprites(GameState* st);
// Funciones de Mario
bool load_mario_sprites(SDL_Renderer* renderer, GameState* st);
void free_mario_sprites(GameState* st);
// Funciones de Kong
bool load_kong_sprites(SDL_Renderer* renderer, GameState* st);
void free_kong_sprites(GameState* st);



MenuOpcion juego_menu(Juego* j);
WhichOpcion juego_menu_which(Juego* j, int tieneA, int tieneB);

void juego_mostrar_game_over(Juego* j, const GameState* st);

// pantalla de juego
static void draw_text(SDL_Renderer* r, float x, float y, const char* text, float scale);
void render_scene(Juego* j, const GameState* st, int es_jugador);

// para el jugador:
void game_loop_jugador(Juego* j, SOCKET sock, GameState* st);
void actualizar_logica_jugador(GameState* st, float dt);
void manejar_input_jugador(SDL_Event* ev, GameState* st);
static int buscar_liana_cercana(const GameState* st);
static void enganchar_a_liana(GameState* st, int idx);
static void liana_bounds(int idx, float* top, float* bottom);
static void aplicar_colision_plataformas(GameState* st, float old_x, float old_y);

// para el espectador
void game_loop_espectador(Juego* j, SOCKET sock, GameState* st);

#endif //JUEGO_H