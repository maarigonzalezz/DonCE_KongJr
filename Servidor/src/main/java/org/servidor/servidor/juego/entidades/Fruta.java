package org.servidor.servidor.juego.entidades;


import org.servidor.servidor.juego.LianaInfo;
import org.servidor.servidor.juego.LianasConfig;

/**
 * Fruta estática ligada a una liana.
 *
 * - lianaId: en qué liana vive.
 * - alturaPct: 0–100, donde:
 *      0  => bottomY de la liana
 *      100 => topY de la liana
 *
 * El servidor solo necesita la posición lógica (x, y) y los puntos;
 * el cliente C dibuja y maneja tamaños.
 */
public final class Fruta extends Entity {

    private final int puntos;
    private final float alturaPct;  // 0 a 100

    public Fruta(int lianaId, int puntos, float alturaPct) {
        super(lianaId);
        this.puntos = puntos;
        this.alturaPct = alturaPct;
    }

    public int getPuntos() {
        return puntos;
    }

    public float getAlturaPct() {
        return alturaPct;
    }

    @Override
    public void update(LianasConfig lianasConfig, float dt) {
        // La fruta no se mueve con el tiempo, pero en cada tick
        // nos aseguramos de que su x/y estén sincronizados con la liana.
        LianaInfo liana = lianasConfig.getById(lianaId);
        if (liana == null) {
            return; // defensivo
        }

        // Posición horizontal: centro de la liana
        this.x = liana.getX() + 5;

        // Posición vertical según porcentaje
        float top = liana.getTopY();
        float bottom = liana.getBottomY();
        float diff = bottom - top;          // altura total de la liana

        float ratio = alturaPct / 100.0f;   // 0.0 .. 1.0

        // 0% -> bottom, 100% -> top
        this.y = bottom - diff * ratio;
    }
}
