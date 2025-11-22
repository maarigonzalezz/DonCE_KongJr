package org.servidor.servidor.juego.entidades;

import java.util.UUID;
import org.servidor.servidor.juego.LianaInfo;
import org.servidor.servidor.juego.LianasConfig;


public abstract class Entity {
    private final UUID entityId;
    protected float x, y;      // posición centro en píxeles
    protected float vx, vy;    // velocidad
    protected int lianaId;
    protected LianaInfo liana;

    protected Entity(int lianaId) {
        this.lianaId = lianaId;
        LianasConfig lianasConfig = new LianasConfig();
        this.liana = lianasConfig.getById(lianaId);
        this.y = liana.getTopY();
        this.x = liana.getX();
        this.entityId = UUID.randomUUID();
    }

    public float x() { return x; }
    public float y() { return y; }

    public UUID getEntityId() {
        return entityId;
    }

    /** Actualización por tick; las subclases definen su comportamiento. */
    public abstract void update(LianasConfig lianasConfig, float dt);
}
