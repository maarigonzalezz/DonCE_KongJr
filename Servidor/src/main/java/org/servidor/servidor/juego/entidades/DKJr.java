package org.servidor.servidor.juego.entidades;

import org.servidor.servidor.juego.Level;
import org.servidor.servidor.juego.reglas.WorldUtil;

/**
 * Entidad del jugador con físicas mínimas:
 * - Gravedad
 * - Aterrizaje en plataformas (one-way) por la cara superior
 * - Agarre simple a lianas y “trepar” automático (para tests)
 */
public final class DKJr extends Entity {

    public enum Estado { DE_PIE, SALTANDO, CAYENDO, COLGADO }

    private Estado estado = Estado.CAYENDO;

    // Físicas/umbrales
    private static final float G = 900f / 60f;      // px/s^2
    private static final float GRAB_X_UMBRAL = 6f;  // distancia X para agarrarse a liana (en píxeles)
    private static final float CLIMB_SPEED   = 40f; // px/s para subir/bajar (solo para pruebas)

    // Dirección de trepada en liana: +1 subir, -1 bajar
    private int climbDir = +1;

    public DKJr(float x, float y) {
        super(x, y, new BoundingBox(12, 12));
    }

    public Estado estado() { return estado; }

    /** Helper para el loop (reset de posición). */
    public void setPosition(float x, float y) { this.x = x; this.y = y; }

    /** Helper para “soltar” y separarse un poco del eje de liana durante tests. */
    public void forceReleaseAndNudgeX(float dx) { this.estado = Estado.CAYENDO; this.x += dx; }


    //Metodos de control
    /** Ordena subir en liana (paso pequeño por input). */
    public void climbUp(float step) {
        if (estado == Estado.COLGADO) {
            climbDir = +1;   // la actualización seguirá subiendo
            y += step;       // pequeño avance inmediato; el update clamp-ea a segmento
        }
    }

    /** Ordena bajar en liana (paso pequeño por input). */
    public void climbDown(float step) {
        if (estado == Estado.COLGADO) {
            climbDir = -1;
            y -= step;
        }
    }

    /** Mover a la izquierda */
    public void moveLeft() {
        if (estado == Estado.DE_PIE || estado == Estado.CAYENDO) {
            this.x -= 4.0f; // se puede ajustar la velocidad
        }
    }

    /** Mover a la derecha */
    public void moveRight() {
        if (estado == Estado.DE_PIE || estado == Estado.CAYENDO) {
            this.x += 4.0f; // se puede ajustar la velocidad
        }
    }

    /** Suelta la liana y pasa a caer. */
    public void release() {
        estado = Estado.CAYENDO;
    }


    @Override
    public void update(Level level, float dt) {
        final int tile = level.map().tileSize();

        // --- Estado: COLGADO (trepar auto para test) ---
        if (estado == Estado.COLGADO) {
            int row = WorldUtil.toRow(level, y);
            int liIndex = WorldUtil.findLianaAt(level, x, row, GRAB_X_UMBRAL);
            if (liIndex >= 0) {
                // Alinear al eje X de la liana
                this.x = level.lianas().get(liIndex).x();

                // Trepar/bajar en Y dentro de los segmentos de la liana
                float ny = y + CLIMB_SPEED * climbDir * dt;

                // Determinar segmento válido alrededor de la nueva Y
                int rNy = WorldUtil.toRow(level, ny);
                var liana = level.lianas().get(liIndex);
                var seg = liana.segments().stream()
                        .filter(s -> rNy >= s.rowStart() && rNy <= s.rowEnd())
                        .findFirst().orElse(null);

                if (seg == null) {
                    // Si salimos del tramo, invertir dirección
                    climbDir *= -1;
                } else {
                    float minY = seg.rowStart() * tile;
                    float maxY = seg.rowEnd()   * tile;
                    if (ny < minY) { ny = minY; climbDir = +1; }
                    if (ny > maxY) { ny = maxY; climbDir = -1; }
                    y = ny;
                }

                vy = 0;
                return; // seguimos colgados
            } else {
                // Si no hay una liana valida el mae se cae
                estado = Estado.CAYENDO;
            }
        }

        // --- Gravedad / caída ---
        vy += G * dt;
        float prevY = y;
        float newY  = y - vy * dt; // y hacia abajo

        // --- Aterrizaje one-way sobre plataformas ---
        float halfH = bbox.h * 0.5f;
        float prevFoot = prevY - halfH;
        float newFoot  = newY  - halfH;

        int col = WorldUtil.toCol(level, x);
        int rowAtNewFoot = WorldUtil.toRow(level, newFoot);

        if (WorldUtil.isPlatformTop(level, col, rowAtNewFoot)) {
            float topY = WorldUtil.rowTopY(level, rowAtNewFoot);
            // venía de arriba y cruzó el tope de la plataforma
            if (prevFoot >= topY && newFoot < topY) {
                newY = topY + halfH;  // coloca de pie sobre la plataforma
                vy = 0;
                estado = Estado.DE_PIE;
                y = newY;

                // Agarre automático si hay liana muy cerca en esta fila
                int li = WorldUtil.findLianaAt(level, x, rowAtNewFoot, GRAB_X_UMBRAL);
                if (li >= 0) {
                    this.x = level.lianas().get(li).x();
                    estado = Estado.COLGADO;
                }
                return;
            }
        }

        // --- Agarre en caída si pasa cerca del eje de una liana ---
        int rowMid = WorldUtil.toRow(level, newY);
        int lianaId = WorldUtil.findLianaAt(level, x, rowMid, GRAB_X_UMBRAL);
        if (lianaId >= 0) {
            this.x = level.lianas().get(lianaId).x();
            vy = 0;
            estado = Estado.COLGADO;
            y = newY;
            return;
        }

        // Si no aterrizó ni se colgó, sigue cayendo
        estado = Estado.CAYENDO;
        y = newY;
    }
}
