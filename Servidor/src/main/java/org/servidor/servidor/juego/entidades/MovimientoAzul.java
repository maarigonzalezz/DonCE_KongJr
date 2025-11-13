package org.servidor.servidor.juego.entidades;

import org.servidor.servidor.juego.Level;
import org.servidor.servidor.juego.Liana;
import org.servidor.servidor.juego.Segmento;

/**
 * Estrategia del cocodrilo azul.
 * EN_LIANA: desciende por la liana hasta el límite inferior del segmento actual.
 * Al tocar el final del tramo: cambia a CAYENDO y aplica gravedad.
 *
 * Mejoras:
 * - Epsilon relativo al tamaño del tile para evitar “temblores” en el borde.
 * - Búsqueda de segmento “seguro”: si no está justo dentro de uno, usa el
 *   tramo más cercano por debajo; si no hay, cae.
 * - Defensivos si la liana no tiene segmentos.
 */
public final class MovimientoAzul implements MovementStrategy {
    @Override
    public void avanzar(Cocodrilo c, Level level, float dt) {
        CocodriloAzul azul = (CocodriloAzul) c;

        // Defensivo: rango de lianas
        if (azul.lianaId() < 0 || azul.lianaId() >= level.lianas().size()) {
            // si la liana es inválida, que caiga
            azul.setEstado(CocodriloAzul.Estado.CAYENDO);
        }

        Liana l = level.lianas().get(azul.lianaId());
        int tile = level.map().tileSize();
        float eps = Math.max(0.1f, tile * 0.05f); // tolerancia 5% del tile

        if (azul.estado() == CocodriloAzul.Estado.EN_LIANA) {
            // --- localizar tramo “seguro” ---
            int row = Math.round(azul.y() / tile);
            Segmento seg = l.segments().stream()
                    .filter(s -> s.contieneFila(row))
                    .findFirst()
                    .orElseGet(() -> {
                        // Elegir el tramo con rowStart más alto que no supere 'row'
                        Segmento best = null;
                        for (Segmento s : l.segments()) {
                            if (s.rowStart() <= row) {
                                if (best == null || s.rowStart() > best.rowStart()) best = s;
                            }
                        }
                        return best;
                    });

            if (seg == null) {
                // Sin tramos válidos: pasar a caída
                azul.setEstado(CocodriloAzul.Estado.CAYENDO);
                // No return: que procese la rama de CAYENDO abajo
            } else {
                float minY = seg.rowStart() * tile;

                // Desciende
                float ny = azul.y - azul.effectiveSpeed() * dt;
                if (ny < minY) ny = minY;

                // ¿tocó el final del tramo?
                if (ny <= minY + eps) {
                    azul.setEstado(CocodriloAzul.Estado.CAYENDO);
                    azul.vy = 0;
                }
                azul.y = ny;
                return;
            }
        }

        // --- CAYENDO: gravedad ---
        azul.vy += azul.gravedad() * dt;
        azul.y -= azul.vy * dt; // hacia abajo
    }
}
