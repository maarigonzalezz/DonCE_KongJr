package org.servidor.servidor.juego.entidades;

import org.servidor.servidor.juego.Level;

public interface MovementStrategy {
    void avanzar(Cocodrilo c, Level level, float dt);
}
