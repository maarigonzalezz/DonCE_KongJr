package org.servidor.servidor.juego.entidades;

import org.servidor.servidor.juego.Level;
import org.servidor.servidor.juego.Liana;
import org.servidor.servidor.juego.Segmento;

/**
 * Estrategia de movimiento del cocodrilo rojo.
 * Calcula los límites (minY/maxY) del segmento actual y mueve al rojo,
 * invirtiendo la dirección al tocar un extremo para “rebotar” dentro del tramo.
 */
public final class MovimientoRojo implements MovementStrategy {
    @Override
    public void avanzar(Cocodrilo c, Level level, float dt) {
        CocodriloRojo rojo = (CocodriloRojo) c;

        // Defensivo: rango de lianas
        if (rojo.lianaId() < 0 || rojo.lianaId() >= level.lianas().size()) return;

        Liana l = level.lianas().get(rojo.lianaId());
        if (l.segments().isEmpty()) return; // sin tramos: nada que hacer

        int tile = level.map().tileSize();

        // Hallar el segmento donde está (o el más cercano por debajo)
        int row = Math.round(rojo.y() / tile);
        Segmento seg = l.segments().stream()
                .filter(s -> s.contieneFila(row))
                .findFirst()
                .orElseGet(() -> {
                    Segmento best = null;
                    for (Segmento s : l.segments()) {
                        if (s.rowStart() <= row) {
                            if (best == null || s.rowStart() > best.rowStart()) best = s;
                        }
                    }
                    return best != null ? best : l.segments().get(0);
                });

        float minY = seg.rowStart() * tile;
        float maxY = seg.rowEnd()   * tile;

        // Usa la speed actual del cocodrilo
        float spd = rojo.effectiveSpeed(); // depende del factor global de velocidad
        float dy  = spd * dt * (rojo.dir() == CocodriloRojo.Dir.UP ? +1f : -1f);
        float ny  = rojo.y + dy;

        // Rebote en extremos
        if (ny > maxY) { ny = maxY; rojo.setDir(CocodriloRojo.Dir.DOWN); }
        if (ny < minY) { ny = minY; rojo.setDir(CocodriloRojo.Dir.UP);   }

        rojo.y = ny;
    }
}
