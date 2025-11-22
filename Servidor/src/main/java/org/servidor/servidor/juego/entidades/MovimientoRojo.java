package org.servidor.servidor.juego.entidades;

import org.servidor.servidor.juego.LianaInfo;
import org.servidor.servidor.juego.LianasConfig;

/**
 * Estrategia de movimiento del cocodrilo rojo.
 * Calcula los límites (minY/maxY) del segmento actual y mueve al rojo,
 * invirtiendo la dirección al tocar un extremo para “rebotar” dentro del tramo.
 */
public class MovimientoRojo implements MovementStrategy {

    @Override
    public void avanzar(Cocodrilo c, LianasConfig lianasConfig, float dt) {
        // 1. Obtener info de la liana donde está el cocodrilo
        LianaInfo liana = lianasConfig.getById(c.getLianaId());

        // 3. Movimiento vertical entre top/bottom de esa liana
        float v = c.effectiveSpeed(); // baseSpeed * speedFactor

        float y = c.getY();
        CocodriloRojo rojo = (CocodriloRojo) c; // si necesitas Dir.UP/DOWN

        switch (rojo.getDir()) {
            case DOWN -> y += v * dt;
            case UP   -> y -= v * dt;
        }

        // 4. Rebotar entre los límites de la liana
        if (y > liana.getBottomY()) {
            y = liana.getBottomY();
            rojo.setDir(CocodriloRojo.Dir.UP);
        } else if (y < liana.getTopY()) {
            y = liana.getTopY();
            rojo.setDir(CocodriloRojo.Dir.DOWN);
        }

        c.setY(y);
    }
}
