package org.servidor.servidor.juego.reglas;

import java.util.ArrayList;
import java.util.List;

import org.servidor.servidor.juego.Level;
import org.servidor.servidor.juego.entidades.*;

public final class DKJrPlatformLianaSmoke {
    public static void main(String[] args) throws InterruptedException {
        Level lvl = new Level();
        GameState state = new GameState();

        // Coloca al jugador sobre una liana (id=1, col≈4) y arriba de una plataforma para ver ambos casos
        int xLiana = lvl.lianas().get(1).x();     // liana id=1 (col 4)
        float startX = 16*5 + 8; // x=88
        float startY = 16*18;    // y=288
        DKJr dk = new DKJr(startX, startY);


        List<Entity> ents = new ArrayList<>();
        ents.add(dk);

        GameLoop loop = new GameLoop(lvl, ents, state);

        float dt = 1f/30f;
        for (int i=0; i<120; i++) {
            loop.tick(dt);
            if (i % 10 == 0) {
                System.out.printf("t=%.2f  DK(x=%.1f,y=%.1f)  estado=%s%n", i*dt, dk.x(), dk.y(), dk.estado());
            }
            Thread.sleep(10);
        }
        System.out.println("Smoke DKJr plataformas/lianas OK.");
    }
}
