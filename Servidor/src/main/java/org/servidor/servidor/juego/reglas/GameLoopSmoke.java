package org.servidor.servidor.juego.reglas;

import java.util.ArrayList;
import java.util.List;

import org.servidor.servidor.juego.Level;
import org.servidor.servidor.juego.entidades.*;

public final class GameLoopSmoke {
    public static void main(String[] args) throws InterruptedException {
        Level level = new Level();
        GameState state = new GameState();

        // Entidades de prueba
        CocodriloRojo rojo = new CocodriloRojo(2, 16*12, 60f);
        CocodriloAzul azul = new CocodriloAzul(0, 16*10, 80f);
        Fruta fruta = new Fruta(4, 16*12, 100);

        List<Entity> entities = new ArrayList<>();
        entities.add(rojo);
        entities.add(azul);
        entities.add(fruta);

        GameLoop loop = new GameLoop(level, entities, state);

        float dt = 1f/30f;
        for (int i=0; i<60; i++) {
            loop.tick(dt);
            if (i % 10 == 0) {
                System.out.printf("t=%.2f  Rojo(y=%.1f)  Azul(y=%.1f, vy=%.1f, est=%s)  Fruta(y=%.1f)  [vidas=%d, score=%d]%n",
                        i*dt, rojo.y(), azul.y(), azul.vy(), azul.estado(), fruta.y(), state.vidas(), state.score());
            }
            Thread.sleep(10);
        }
        System.out.println("GameLoop smoke OK.");
    }
}
