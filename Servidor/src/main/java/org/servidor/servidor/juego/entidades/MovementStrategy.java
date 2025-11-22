package org.servidor.servidor.juego.entidades;

import org.servidor.servidor.juego.LianasConfig;

public interface MovementStrategy {
    void avanzar(Cocodrilo c, LianasConfig lianasConfig, float dt);
}
