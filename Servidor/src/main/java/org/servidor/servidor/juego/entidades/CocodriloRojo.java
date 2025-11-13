package org.servidor.servidor.juego.entidades;

/**
 * Cocodrilo que sube y baja dentro de un segmento de liana sin caerse.
 * Estados implícitos: dirección UP/DOWN; invierte sentido en los extremos.
 * Usa MovimientoRojo para calcular su delta-y y límites.
 */


public final class CocodriloRojo extends Cocodrilo {
    public enum Dir { UP, DOWN }
    Dir dir = Dir.UP;

    public CocodriloRojo(int lianaId, float yInicial, float speed) {
        super(lianaId, yInicial, speed, new BoundingBox(10, 10));
        this.estrategia = new MovimientoRojo();
    }

    // Accesor para estrategia
    Dir dir() { return dir; }
    void setDir(Dir d) { dir = d; }
}
