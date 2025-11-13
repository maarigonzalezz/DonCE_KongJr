package org.servidor.servidor.juego.entidades;

import java.util.UUID;
import org.servidor.servidor.juego.Level;





public abstract class Entity {
    public final UUID id = UUID.randomUUID();
    protected float x, y;      // posición centro en píxeles
    protected float vx, vy;    // velocidad
    protected final BoundingBox bbox;

    protected Entity(float x, float y, BoundingBox bbox) {
        this.x = x; this.y = y; this.bbox = bbox;
    }

    public float x() { return x; }
    public float y() { return y; }
    public float vy() { return vy; }     // <-- útil para logs de smoke test
    public BoundingBox bbox() { return bbox; }

    /** Actualización por tick; las subclases definen su comportamiento. */
    public abstract void update(Level level, float dt);
}
