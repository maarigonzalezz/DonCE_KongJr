
#ifndef CONSTANTS_H
#define CONSTANTS_H

// constantes para conexion cliente servidor
#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT "5050"
#define RECV_BUF    1024

// ventana principal
#define WINDOW_WIDTH   800
#define WINDOW_HEIGHT  600
#define WINDOW_TITLE   "DonCEy Kong Jr"

// botones del menú (posición y tamaño)
#define BTN1_X 300.0f
#define BTN1_Y 220.0f
#define BTN1_W 300.0f
#define BTN1_H 60.0f

#define BTN2_X 300.0f
#define BTN2_Y 320.0f
#define BTN2_W 300.0f
#define BTN2_H 60.0f

#define WHICH_BTN_A_X 300
#define WHICH_BTN_A_Y 220
#define WHICH_BTN_A_W 300
#define WHICH_BTN_A_H 60

#define WHICH_BTN_B_X 300
#define WHICH_BTN_B_Y 320
#define WHICH_BTN_B_W 300
#define WHICH_BTN_B_H 60

// --- Ventana principal ---
#define WINDOW_WIDTH   800
#define WINDOW_HEIGHT  600

// --- Stage fijo (valores aproximados, luego los ajustas a gusto) ---

// Agua
#define WATER_Y        570
#define WATER_H        30

#define NUM_PLATFORMS 12
// TIERRA 1 DONDE EMPIEZA JR
#define PLAT_TOP_X     0
#define PLAT_TOP_Y     550
#define PLAT_TOP_W     250
#define PLAT_TOP_H     20

// TIERRA 2
#define TIERRA_MID2_X    320
#define TIERRA_MID2_Y    510
#define TIERRA_MID2_W    60
#define TIERRA_MID2_H    60

// TIERRA 3
#define TIERRA_MID3_X    410
#define TIERRA_MID3_Y    530
#define TIERRA_MID3_W    50
#define TIERRA_MID3_H    40


// TIERRA 4
#define TIERRA_MID4_X    500
#define TIERRA_MID4_Y    510
#define TIERRA_MID4_W    60
#define TIERRA_MID4_H    60

// TIERRA 5
#define TIERRA_MID5_X    590
#define TIERRA_MID5_Y    495
#define TIERRA_MID5_W    60
#define TIERRA_MID5_H    75

// TIERRA 6
#define TIERRA_MID6_X    680
#define TIERRA_MID6_Y    495
#define TIERRA_MID6_W    60
#define TIERRA_MID6_H    75


// PLATAFORMA 1
#define PLAT_MID1_X    170
#define PLAT_MID1_Y    380
#define PLAT_MID1_W    150
#define PLAT_MID1_H    20

// PLATAFORMA 2
#define PLAT_MID2_X    170
#define PLAT_MID2_Y    280
#define PLAT_MID2_W    90
#define PLAT_MID2_H    20

// PLATAFORMA 3
#define PLAT_MID3_X    580
#define PLAT_MID3_Y    340
#define PLAT_MID3_W    200
#define PLAT_MID3_H    20

// PLATAFORMA 4
#define PLAT_MID4_X    450
#define PLAT_MID4_Y    160
#define PLAT_MID4_W    200
#define PLAT_MID4_H    20

// PLATAFORMA 5
#define PLAT_MID5_X    0
#define PLAT_MID5_Y    140
#define PLAT_MID5_W    470
#define PLAT_MID5_H    20

// PLATAFORMA 6
#define PLAT_MID6_X    200
#define PLAT_MID6_Y    60
#define PLAT_MID6_W    60
#define PLAT_MID6_H    20

// Jaula de Donkey
#define DK_CAGE_X      80
#define DK_CAGE_Y      60
#define DK_CAGE_W      95
#define DK_CAGE_H      80

// Mario
#define MARIO_X      195
#define MARIO_Y      90
#define MARIO_W      50
#define MARIO_H      50

#define NUM_LIANAS 13
// Ancho de una liana
#define LIANA_W  8.0f

// Liana 1
#define LIANA1_X      60
#define LIANA1_TOP_Y  (PLAT_MID5_Y + PLAT_MID5_H)
#define LIANA1_BOT_Y  500.0f

// Liana 2
#define LIANA2_X      140
#define LIANA2_BOT_Y  480.0f

// Liana 3
#define LIANA3_X      350.0f
#define LIANA3_BOT_Y  460.0f

// Liana 4
#define LIANA4_X      430.0f
#define LIANA4_BOT_Y  360.0f

// Liana 5
#define LIANA5_X      220
#define LIANA5_TOP_Y  (PLAT_MID2_Y + PLAT_MID2_H)
#define LIANA5_BOT_Y  80.0f

// Liana 6
#define LIANA6_X      220
#define LIANA6_TOP_Y  (PLAT_MID1_Y + PLAT_MID1_H)
#define LIANA6_BOT_Y  100.0f

// Liana 7
#define LIANA7_X      500
#define LIANA7_TOP_Y  (PLAT_MID4_Y + PLAT_MID4_H)
#define LIANA7_BOT_Y  290

// Liana 8
#define LIANA8_X      560
#define LIANA8_BOT_Y  220

// Liana 9
#define LIANA9_X      680 // para l9 y 11
#define LIANA9_TOP_Y  100 // para l9 y 10
#define LIANA9_BOT_Y  240 //para l9 y 10

// Liana 10
#define LIANA10_X      740 // para l10 y 12

// Liana 11
#define LIANA11_TOP_Y  360 // para l11 y 12
#define LIANA11_BOT_Y  60 //para l11 y 12

// Liana 13
#define LIANA13_X      300
#define LIANA13_TOP_Y  35
#define LIANA13_BOT_Y  60


// Posición inicial de Jr (jugador)
#define JR_START_X     (PLAT_TOP_X + 50)
#define JR_START_Y     (PLAT_TOP_Y-35)

// Tamaño de Jr
#define JR_WIDTH       35
#define JR_HEIGHT      35

// configuraciones generales
#define JR_SPEED 180.0f  // píxeles por segundo
#define HOOK_RADIUS 20.0f  // qué tan cerca debe estar en X para engancharse
#define VINE_SPEED  140.0f // velocidad subir/bajar
#define JR_JUMP_VY   -320.0f   // velocidad inicial del salto (negativa = hacia arriba)
#define JR_GRAVITY    900.0f   // gravedad



//#define _WIN32_WINNT 0x0601
#define SDL_MAIN_HANDLED

#endif //CONSTANTS_H

