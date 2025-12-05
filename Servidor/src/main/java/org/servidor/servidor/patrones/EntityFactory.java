package org.servidor.servidor.patrones;

import org.servidor.servidor.juego.entidades.Entity;

public interface EntityFactory {
    /**
     * Crea una entidad para una liana dada.
     *
     * @param kind      tipo de entidad (fruta, croc rojo, croc azul)
     * @param lianaId   índice de liana
     * @param alturaPct solo aplica a frutas: 0–100 (0 = bottom, 100 = top).
     */
    Entity createEntity(EntityKind kind, int lianaId, float alturaPct, int puntos);
}
