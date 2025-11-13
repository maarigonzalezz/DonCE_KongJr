package org.servidor.servidor.juego.entidades;


/**
 * Caja de colisión alineada a ejes (AABB) expresada por ancho y alto.
 * Provee intersección AABB vs AABB usando posiciones de centro de dos entidades.
 * Se usa para detectar golpes (cocodrilo <--> jugador) y recogida (fruta <--> jugador).
 */



public final class BoundingBox {
    public final float w, h; // ancho/alto en píxeles

    public BoundingBox(float w, float h) { this.w = w; this.h = h; }

    /** AABB contra AABB dadas las posiciones centro (x,y) de cada entidad. */
    public boolean intersects(float ax, float ay, BoundingBox b, float bx, float by) {
        return Math.abs(ax - bx) * 2 < (this.w + b.w) &&
                Math.abs(ay - by) * 2 < (this.h + b.h);
    }
}
