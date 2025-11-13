package org.servidor.servidor.juego.reglas;

import java.util.ArrayList;
import java.util.List;

import org.servidor.servidor.juego.Level;
import org.servidor.servidor.juego.entidades.*;

public final class VictorySpeedFactorSmoke {
    public static void main(String[] args) throws InterruptedException {
        Level lvl = new Level();
        GameState st = new GameState();

        // Enemigos para observar su velocidad (se escalará al ganar)
        CocodriloRojo rojo = new CocodriloRojo(2, 16*12, 60f);
        CocodriloAzul azul = new CocodriloAzul(0, 16*10, 80f);

        // Jugador: lo colocamos justo SOBRE la banda META (col 3-4, fila 19).
        // META en Level: cols 2..5, row 19 -> ponemos al jugador en col 3.5:
        float xMeta = (3 * lvl.map().tileSize()) + lvl.map().tileSize()/2f;
        float yMeta = (19 * lvl.map().tileSize()) + 8f;   // un poco por encima para que el pie caiga en META
        DKJr dk = new DKJr(xMeta, yMeta);

        List<Entity> ents = new ArrayList<>();
        ents.add(dk);
        ents.add(rojo);
        ents.add(azul);

        GameLoop loop = new GameLoop(lvl, ents, st);

        float dt = 1f/30f;
        for (int i=0; i<5; i++) loop.tick(dt); // unos ticks para que dispare la victoria

        System.out.printf("Tras victoria: speedFactor=%.2f, vidas=%d, score=%d%n",
                st.speedFactor(), st.vidas(), st.score());

        // Ahora corre un poco para ver que los cocodrilos van más rápido (por el factor aplicado)
        for (int i=0; i<30; i++) { loop.tick(dt); Thread.sleep(5); }

        System.out.println("Victory smoke OK.");
    }
}
