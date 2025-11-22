package org.servidor.servidor.juego.reglas;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

/** Estado mínimo serializable para clientes/espectadores. */
public final class Snapshot {
    public int tick;              // contador de ticks (opcional)
    public int score;
    public int vidas;
    public float speedFactor;

    public final List<EntityState> entidades = new ArrayList<>();

    public static final class EntityState {
        public String tipo;
        public String id;
        public float x, y;    // centro en píxeles
    }
}
