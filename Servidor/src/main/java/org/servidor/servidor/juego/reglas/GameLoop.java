package org.servidor.servidor.juego.reglas;

import java.util.ArrayList;
import java.util.List;

import org.servidor.servidor.juego.LianasConfig;
import org.servidor.servidor.juego.entidades.*;

public final class GameLoop {
    private final List<Entity> entities;
    private final GameState gameState;

    private final LianasConfig lianasConfig;

    public GameLoop(List<Entity> entidades, GameState state) {
        this.entities = entidades;
        this.gameState = state;
        this.lianasConfig = new LianasConfig();
    }

    public void tick(float dt) {
        float speedFactor = gameState.speedFactor();

        // Si algún día quieres eliminar azules que se caen,
        // puedes ir marcándolos aquí y borrarlos luego
        List<Entity> porEliminar = new ArrayList<>();

        for (Entity e : entities) {
            // Aplicar factor de velocidad global a cocodrilos
            if (e instanceof Cocodrilo croc) {
                croc.setSpeedFactor(speedFactor);
            }

            // Delegar la lógica de movimiento a cada entidad
            e.update(lianasConfig, dt);

            // Si quieres, aquí podrías chequear:
            // - azules muy abajo -> porEliminar.add(e);
        }

        if (!porEliminar.isEmpty()) {
            entities.removeAll(porEliminar);
        }
    }

    //
    public void startEntities() {
        entities.add(new Fruta(5, 300, 90f));
        entities.add(new Fruta(1, 300, 50f));
        entities.add(new CocodriloAzul(1, 2f));
    }

    /** Aplica boost de victoria y reposiciona al jugador.
    private void applyVictoryReset(DKJr jugador) {
        // 1) Boost lógico (+10% de dificultad)
        float factor = 1.10f;
        gameState.victoryBoost(factor);

        // 2) Aqui se puede multiplicar  baseSpeed*speedFactor

    }*/
}
