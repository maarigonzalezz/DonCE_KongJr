package org.servidor.servidor.juego.reglas;

import java.util.ArrayList;
import java.util.List;

/** Estado mínimo serializable para clientes/espectadores. */
public final class Snapshot {
    public int tick;              // contador de ticks (opcional)
    public int score;
    public int vidas;
    public float speedFactor;

    public final List<EntityState> entidades = new ArrayList<>();

    public static final class EntityState {
        public String tipo;   // "DKJr", "CocodriloRojo", "CocodriloAzul", "Fruta"
        public String id;     // UUID.toString()
        public float x, y;    // centro en píxeles
        public float w, h;    // bbox (para debug/cliente)
    }
}
