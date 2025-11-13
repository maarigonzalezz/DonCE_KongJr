package org.servidor.servidor.juego.reglas;

import java.util.ArrayList;
import java.util.List;

import org.servidor.servidor.juego.Level;
import org.servidor.servidor.juego.entidades.*;

public final class GameLoopCollisionsSmoke {
    public static void main(String[] args) throws InterruptedException {
        Level level = new Level();
        GameState state = new GameState();

        // Colocamos al jugador justo donde está la fruta para forzar recogida
        Fruta fruta = new Fruta(4, 16*12, 100);
        DKJr jugador = new DKJr(level.lianas().get(fruta.lianaId()).x(), 16*12);

        // Colocamos un cocodrilo azul más abajo para provocar golpe después de unos ticks
        CocodriloAzul azul = new CocodriloAzul(0, 16*10, 80f);

        List<Entity> entities = new ArrayList<>();
        entities.add(jugador);
        entities.add(fruta);
        entities.add(azul);

        GameLoop loop = new GameLoop(level, entities, state);

        float dt = 1f/30f;
        for (int i=0; i<60; i++) {
            loop.tick(dt);

            if (i % 10 == 0) {
                System.out.printf("t=%.2f  Jugador(y=%.1f)  Ents=%d  [vidas=%d, score=%d]%n",
                        i*dt, jugador.y(), entities.size(), state.vidas(), state.score());
            }
            Thread.sleep(10);
        }
        System.out.println("Collisions smoke OK (deberías ser score>=100 y quizá vidas<2).");
    }
}
