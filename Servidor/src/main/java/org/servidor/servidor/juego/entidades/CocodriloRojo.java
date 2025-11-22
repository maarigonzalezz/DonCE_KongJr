package org.servidor.servidor.juego.entidades;

/**
 * Cocodrilo que sube y baja dentro de un segmento de liana sin caerse.
 * Estados implícitos: dirección UP/DOWN; invierte sentido en los extremos.
 * Usa MovimientoRojo para calcular su delta-y y límites.
 */


public final class CocodriloRojo extends Cocodrilo {
    public enum Dir { UP, DOWN }
    private Dir dir = Dir.DOWN;

    public CocodriloRojo(int lianaId, float speed) {
        super(lianaId, speed);
        this.estrategia = new MovimientoRojo();
    }

    public Dir getDir() { return dir; }
    public void setDir(Dir d) { dir = d; }

    @Override
    public int getLianaId() {
        return super.getLianaId();
    }
}