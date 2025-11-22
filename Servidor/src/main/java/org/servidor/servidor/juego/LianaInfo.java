package org.servidor.servidor.juego;

public class LianaInfo {
    private final int id;       // 0 a 12, por ejemplo
    private final float x;      // posición horizontal en tu sistema de coordenadas
    private final float topY;   // y mínima (parte alta donde empiezan los bichos)
    private final float bottomY;// y máxima (hasta dónde puede bajar sin caerse)
    private final float fallY;  // (para azules) altura donde se sueltan de la liana

    public LianaInfo(int id, float x, float topY, float bottomY){
        this.id = id;
        this.x = x;
        this.topY = topY;
        this.bottomY = bottomY;
        this.fallY = bottomY - 60f;
    }

    public int getId() {
        return id;
    }

    public float getX() {
        return x;
    }

    public float getTopY() {
        return topY;
    }

    public float getBottomY() {
        return bottomY;
    }

    public float getFallY() {
        return fallY;
    }
}
