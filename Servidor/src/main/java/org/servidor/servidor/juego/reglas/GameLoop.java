package org.servidor.servidor.juego.reglas;

import java.util.Iterator;
import java.util.List;

import org.servidor.servidor.juego.Level;
import org.servidor.servidor.juego.TileType;
import org.servidor.servidor.juego.entidades.*;

public final class GameLoop {
    private final Level level;
    private final List<Entity> entidades;
    private final GameState state;

    public GameLoop(Level level, List<Entity> entidades, GameState state) {
        this.level = level;
        this.entidades = entidades;
        this.state = state;
    }

    public void tick(float dt) {
        if (state.fase() != GameState.Fase.RUNNING && state.fase() != GameState.Fase.VICTORY_RESET) return;

        // Si venimos de reset de victoria, volvemos a RUNNING en el siguiente tick
        if (state.fase() == GameState.Fase.VICTORY_RESET) {
            state.setFase(GameState.Fase.RUNNING);
        }

        // 1) Avanzar entidades
        for (Entity e : entidades) {
            if (e instanceof Cocodrilo c) c.setSpeedFactor(state.speedFactor());
            e.update(level, dt);
        }

        // 2) Buscar jugador
        DKJr jugador = null;
        for (Entity e : entidades) {
            if (e instanceof DKJr j) { jugador = j; break; }
        }
        if (jugador == null) return;

        // 3) Jugador <--> Fruta (score++)
        for (Iterator<Entity> it = entidades.iterator(); it.hasNext(); ) {
            Entity e = it.next();
            if (e instanceof Fruta f) {
                if (CollisionUtil.intersects(jugador, e)) {
                    state.addScore(f.puntos());
                    it.remove(); // fruta recogida
                }
            }
        }

        // 4) Jugador <--> Cocodrilos (pierde vida)
        for (Entity e : entidades) {
            if (e instanceof Cocodrilo && CollisionUtil.intersects(jugador, e)) {
                state.loseLife();
                if (state.fase() != GameState.Fase.GAME_OVER) {
                    respawnDKJr(jugador);
                } else {
                    // Aquí se puede tirar el fin de la partida
                }
                break;
            }
        }

        // 5) Victoria: tocar META (tile bajo el pie)
        int col = WorldUtil.toCol(level, jugador.x());
        int row = WorldUtil.toRow(level, jugador.y() - jugador.bbox().h * 0.5f);
        if (level.map().getTile(col, row) == TileType.META) {
            applyVictoryReset(jugador);
        }
    }

    /** Reubica al jugador tras perder una vida. */
    private void respawnDKJr(DKJr j) {
        int t = level.map().tileSize();
        j.forceReleaseAndNudgeX(0);
        j.setPosition(t * 2 + t/2f, t * 2 + j.bbox().h/2f); // spawn bajo-izq
        // Si se añade un reset de velocidades/estado en DKJr, se puede llamar aca
    }

    /** Aplica boost de victoria y reposiciona al jugador. */
    private void applyVictoryReset(DKJr jugador) {
        // 1) Boost lógico (+10% de dificultad)
        float factor = 1.10f;
        state.victoryBoost(factor);

        // 2) Aqui se puede multiplicar  baseSpeed*speedFactor


        // 3) Reset de posición del jugador
        int tile = level.map().tileSize();
        jugador.forceReleaseAndNudgeX(0);
        jugador.setPosition(tile * 2 + tile/2f, tile * 2 + jugador.bbox().h/2f);
    }
}
