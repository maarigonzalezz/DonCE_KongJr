package org.servidor.servidor.juego.entidades;

import org.servidor.servidor.juego.Level;


/**
 * Objeto de puntuación estático asociado a una liana y una altura.
 * - lianaId, y, puntos.
 * En cada tick solo alinea su x al eje de la liana. Se elimina al ser recogida.
 */


public final class Fruta extends Entity {
    private final int lianaId;
    private final int puntos;

    public Fruta(int lianaId, float y, int puntos) {
        super(0, y, new BoundingBox(8, 8));
        this.lianaId = lianaId;
        this.puntos = puntos;
    }

    public int lianaId() { return lianaId; }
    public int puntos() { return puntos; }

    @Override
    public void update(Level level, float dt) {
        this.x = level.lianas().get(lianaId).x(); // estática, solo alinea X
    }
}
