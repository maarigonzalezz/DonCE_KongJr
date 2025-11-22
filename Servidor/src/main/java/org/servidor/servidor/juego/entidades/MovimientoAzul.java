package org.servidor.servidor.juego.entidades;

import org.servidor.servidor.juego.LianasConfig;
import org.servidor.servidor.juego.LianaInfo;

/**
 * Estrategia de movimiento para el cocodrilo azul.
 *
 * EN_LIANA:
 *  - Baja pegado a la liana hasta el bottomY de esa liana.
 *  - Cuando llega al límite inferior, cambia a CAYENDO.
 *
 * CAYENDO:
 *  - Aplica gravedad (vy y G) hasta que el GameLoop decida eliminarlo
 *    (por ejemplo, cuando pasa cierto límite de Y).
 */
public final class MovimientoAzul implements MovementStrategy {

    @Override
    public void avanzar(Cocodrilo base, LianasConfig lianasConfig, float dt) {
        // Esta estrategia sólo aplica a CocodriloAzul
        if (!(base instanceof CocodriloAzul azul)) {
            return;
        }

        // Obtener info de la liana correspondiente
        int lianaId = azul.getLianaId();
        LianaInfo liana = lianasConfig.getById(lianaId);
        if (liana == null) {
            return; // defensivo
        }

        // (Opcional) Alinear X a la liana, si quieres que se pegue al centro:
        // azul.setX(liana.getX());

        // Decidir según el estado actual
        switch (azul.getEstado()) {
            case EN_LIANA -> actualizarEnLiana(azul, liana, dt);
            case CAYENDO -> actualizarCayendo(azul, dt);
        }
    }

    /**
     * Fase EN_LIANA: baja a velocidad constante por la liana hasta bottomY.
     */
    private void actualizarEnLiana(CocodriloAzul azul, LianaInfo liana, float dt) {
        // Usar la velocidad EFECTIVA (baseSpeed * speedFactor)
        float v = azul.effectiveSpeed();   // <-- usa el factor global

        float ny = azul.getY() + v * dt;

        // Cuando llega al límite inferior, se suelta
        if (ny >= liana.getBottomY()) {
            ny = liana.getBottomY();
            azul.setEstado(CocodriloAzul.Estado.CAYENDO);
            azul.setVy(0f); // arranca caída desde velocidad 0
        }

        azul.setY(ny);
    }

    /**
     * Fase CAYENDO: caída libre con gravedad.
     * Escalamos la gravedad con el mismo factor de velocidad
     * para que al subir la dificultad caiga más rápido.
     */
    private void actualizarCayendo(CocodriloAzul azul, float dt) {
        // Factor de velocidad global que le puso el GameLoop
        float factor = azul.effectiveSpeed();  // getter en Cocodrilo

        // Gravedad efectiva
        float g = azul.gravedad() * factor;

        float vy = azul.getVy() + g * dt;
        float ny = azul.getY() + vy * dt;

        azul.setVy(vy);
        azul.setY(ny);

        // Aquí podrías marcarlo para eliminar si cae demasiado
        // (el borrado real lo hace el GameLoop):
        //
        // if (ny > algunLimite) {
        //     azul.setMuerto(true);
        // }
    }
}
