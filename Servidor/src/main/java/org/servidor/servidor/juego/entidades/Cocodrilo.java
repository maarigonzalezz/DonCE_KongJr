package org.servidor.servidor.juego.entidades;

import org.servidor.servidor.juego.Level;

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
    protected final int lianaId;

    //modelo de velocidad
    protected float baseSpeed;          // velocidad base (px/s)
    private float speedFactor = 1.0f;   // factor inyectado por GameLoop

    protected MovementStrategy estrategia;

    protected Cocodrilo(int lianaId, float y, float baseSpeed, BoundingBox bbox) {
        super(0, y, bbox);
        this.lianaId = lianaId;
        this.baseSpeed = baseSpeed;
    }

    public int lianaId() { return lianaId; }

    // Setter para que el GameLoop inyecte el factor global
    public void setSpeedFactor(float f) { this.speedFactor = f; }

    // Velocidad efectiva = baseSpeed * factor actual
    public float effectiveSpeed() { return baseSpeed * speedFactor; }

    // (Opcional) getters/setters por si se quiere ajustar en runtime:
    public float baseSpeed() { return baseSpeed; }
    public void setBaseSpeed(float s) { this.baseSpeed = s; }

    @Override
    public void update(Level level, float dt) {
        if (estrategia == null) return;
        if (lianaId < 0 || lianaId >= level.lianas().size()) return;

        // Alinear X al eje de la liana
        this.x = level.lianas().get(lianaId).x();

        // Delegar movimiento específico
        estrategia.avanzar(this, level, dt);
    }
}