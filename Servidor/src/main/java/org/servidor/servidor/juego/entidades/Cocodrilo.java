package org.servidor.servidor.juego.entidades;

import org.servidor.servidor.juego.LianasConfig;

/**
 * Base abstracta para cocodrilos.
 * - lianaId: índice de liana a la que está “anclado” (x se alinea al eje de la liana).
 * - speed: velocidad vertical base (px/s). La estrategia decide cómo aplicarla.
 * - estrategia: comportamiento (Rojo/Azul).
 *
 * Notas:
 * - Incluye defensivos si el lianaId está fuera de rango.
 * - Provee getters/setters de speed para ajustar la velocidad
 */


public abstract class Cocodrilo extends Entity {
    protected float baseSpeed;
    protected float speedFactor = 1.0f;
    protected MovementStrategy estrategia;

    public Cocodrilo(int lianaId, float baseSpeed) {
        super(lianaId);
        this.baseSpeed = baseSpeed;
    }

    public void setSpeedFactor(float sf) {
        this.speedFactor = sf;
    }

    public float effectiveSpeed() {
        return baseSpeed * speedFactor;
    }

    public int getLianaId() { return lianaId; }
    public float getY() { return y; }
    public void setY(float y) { this.y = y; }

    public float getX() {
        return x;
    }

    public float getBaseSpeed() {
        return baseSpeed;
    }

    public void update(LianasConfig lianasConfig, float dt) {
        if (estrategia == null) return;
        if (lianaId < 0 || lianaId >= lianasConfig.count()) return;

        // La estrategia se encarga de usar la info de la liana
        estrategia.avanzar(this, lianasConfig, dt);
    }
}
