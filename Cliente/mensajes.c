#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include "juego.h"
#include "mensajes.h"


void enviar_muerte(SOCKET sock, DeathReason r) {
    const char* reason_str = "unknown";
    if (r == DEATH_WATER) reason_str = "water";
    else if (r == DEATH_CROC) reason_str = "croc";

    char json[128];
    snprintf(json, sizeof json,
        "{\"type_message\":\"death\",\"reason\":\"%s\"}", reason_str);

    net_send_line(sock, json);
}


// RECIBIR MENSAJES
void parse_start_message(const char* line, GameState* state)
{
    const char* p;
    int val;

    state->speed = 1.0f;
    state->jr_x = JR_START_X;
    state->jr_y = JR_START_Y;
    state->jr_mode   = JR_MODE_GROUND;
    state->jr_facing = JR_FACE_RIGHT;
    state->jr_vx = 0;
    state->jr_vy = 0;
    state->vine_idx = -1;
    state->on_ground = 1;
    state->pending_death = DEATH_NONE;

    state->num_frutas = 0;
    state->num_cocodrilos = 0;
    for (int i = 0; i < MAX_FRUTAS; ++i) state->frutas[i].activa = 0;
    for (int i = 0; i < MAX_COCODRILOS; ++i) state->cocodrilos[i].activo = 0;

    // --- score ---
    p = strstr(line, "\"score\":");
    if (p && sscanf(p + 8, "%d", &val) == 1) {
        state->score = val;
    }

    // --- lifes ---
    p = strstr(line, "\"lifes\":");
    if (p && sscanf(p + 8, "%d", &val) == 1) {
        state->vidas = val;
    }

    // --- partida (A o B) ---
    p = strstr(line, "\"partida\":");
    if (p) {
        // nos movemos justo después de "partida":
        p += 10;

        // saltar espacios
        while (*p == ' ' || *p == '\t')
            p++;

        char buf[8] = {0};

        if (*p == '"') {
            // caso por si algún día viene "A" con comillas
            p++;
            sscanf(p, "%7[^\"]", buf);
        } else {
            // caso actual: partida:A  (sin comillas)
            sscanf(p, "%7[^,} ]", buf); // hasta coma, llave o espacio
        }

        // guardar en GameState
        strncpy(state->partida, buf, sizeof(state->partida) - 1);
        state->partida[sizeof(state->partida) - 1] = '\0';
    }
}

void parse_snapshot(GameState* st, const char* line) {
    const char* p;

    // 1) score
    p = strstr(line, "\"score\":");
    if (p) {
        int score;
        if (sscanf(p + 8, "%d", &score) == 1) {
            st->score = score;
        }
    }

    // 2) lifes
    p = strstr(line, "\"lifes\":");
    if (p) {
        int lifes;
        if (sscanf(p + 8, "%d", &lifes) == 1) {
            st->vidas = lifes;
        }
    }

    // 3) speedFactor
    p = strstr(line, "\"speedFactor\":");
    if (p) {
        float sf;
        if (sscanf(p + 13, "%f", &sf) == 1) {
            st->speed = sf;
        }
    }

    // 4) Resetear listas de frutas y cocodrilos
    st->num_frutas = 0;
    st->num_cocodrilos = 0;
    for (int i = 0; i < MAX_FRUTAS; ++i) st->frutas[i].activa = 0;
    for (int i = 0; i < MAX_COCODRILOS; ++i) st->cocodrilos[i].activo = 0;

    // 5) Entrar al arreglo "entidades"
    p = strstr(line, "\"entidades\":[");
    if (!p) return;

    const char* cursor = strchr(p, '[');
    if (!cursor) return;
    cursor++;  // después de '['

    // Recorremos cada objeto { ... } dentro del array
    while (*cursor && *cursor != ']') {
        const char* tipo_p = strstr(cursor, "\"tipo\":\"");
        if (!tipo_p) break;
        tipo_p += 8; // avanzar después de "tipo":"

        char tipo[32] = {0};
        sscanf(tipo_p, "%31[^\"]", tipo);

        const char* id_p = strstr(cursor, "\"id\":\"");
        char id[64] = {0};
        if (id_p) {
            id_p += 6; // después de "id":"
            sscanf(id_p, "%63[^\"]", id);
        }

        float x = 0.0f, y = 0.0f;
        const char* x_p = strstr(cursor, "\"x\":");
        const char* y_p = strstr(cursor, "\"y\":");
        if (x_p) sscanf(x_p + 4, "%f", &x);
        if (y_p) sscanf(y_p + 4, "%f", &y);

        // Clasificar por tipo
        if (strcmp(tipo, "Fruta") == 0) {
            if (st->num_frutas < MAX_FRUTAS) {
                Fruta* f = &st->frutas[st->num_frutas++];
                strncpy(f->id, id, sizeof(f->id)-1);
                f->x = x;
                f->y = y;
                f->activa = 1;
            }
        } else if (strcmp(tipo, "CocodriloAzul") == 0 ||
                   strcmp(tipo, "CocodriloRojo") == 0) {

            if (st->num_cocodrilos < MAX_COCODRILOS) {
                Cocodrilo* c = &st->cocodrilos[st->num_cocodrilos++];
                strncpy(c->id, id, sizeof(c->id)-1);
                c->x = x;
                c->y = y;
                c->activo = 1;
                c->tipo = (strcmp(tipo, "CocodriloAzul") == 0)
                          ? CROC_AZUL : CROC_ROJO;
            }
        }

        // Avanzar al siguiente objeto
        const char* end_obj = strchr(cursor, '}');
        if (!end_obj) break;
        cursor = end_obj + 1;

        // saltar comas y espacios
        while (*cursor == ',' || *cursor == ' ') cursor++;
        // si el siguiente char es ']', salimos del while
    }
}