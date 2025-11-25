package org.servidor.servidor.juego.reglas;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Objects;

import com.fasterxml.jackson.databind.JsonNode;
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
        entities.add(new Fruta(1, 100, 50f));
        entities.add(new CocodriloAzul(1, 2f));
    }

    public void manejarDeath() {
        gameState.perderVida();
    }

    public void manejarFruitDestroyed(JsonNode msg) {
        String id = msg.path("id").asText(null);
        if (id == null || id.isEmpty()) {
            System.out.println("Mensaje fruit_destroyed sin id válido");
            return;
        }

        // Usar un Iterator para poder eliminar mientras iteramos
        Iterator<Entity> it = entities.iterator();
        while (it.hasNext()) {
            Entity e = it.next();

            // Comparamos el id de la entidad con el id que vino en el mensaje
            if (Objects.equals(e.getEntityId().toString(), id)) {

                // Verificamos que realmente sea una fruta
                if (e instanceof Fruta fruta) {
                    int puntos = fruta.getPuntos();
                    gameState.updateScore(puntos);  // suma al puntaje oficial
                    System.out.println("Fruta " + id + " destruida, +" + puntos + " puntos");
                } else {
                    System.out.println("Entity con id " + id + " no es una Fruta");
                }

                // Sacamos la entidad (fruta) de la lista
                it.remove();
                break; // ya encontramos la fruta, salimos del while
            }
        }
    }

    public void manejarWin() {
        gameState.victoryBoost(1.5f);
    }

    /** Aplica boost de victoria y reposiciona al jugador.
    private void applyVictoryReset(DKJr jugador) {
        // 1) Boost lógico (+10% de dificultad)
        float factor = 1.10f;
        gameState.victoryBoost(factor);

        // 2) Aqui se puede multiplicar  baseSpeed*speedFactor

    }*/
}
