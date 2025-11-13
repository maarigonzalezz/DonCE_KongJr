package org.servidor.servidor.juego.entidades;


/**
 * Cocodrilo que desciende por una liana y, al finalizar el segmento, cae por gravedad.
 * Estados: EN_LIANA --> CAYENDO. En CAYENDO no puede volver a agarrarse.
 * Usa MovimientoAzul para transición y actualización de velocidades.
 */


public final class CocodriloAzul extends Cocodrilo {
    public enum Estado { EN_LIANA, CAYENDO }
    private Estado estado = Estado.EN_LIANA;
    private static final float G = 900f / 60f; // gravedad aprox. (px/s^2) adaptada

    public CocodriloAzul(int lianaId, float yInicial, float speed) {
        super(lianaId, yInicial, speed, new BoundingBox(10, 10));
        this.estrategia = new MovimientoAzul();
    }

   public Estado estado() { return estado; }
    void setEstado(Estado e) { estado = e; }
    float gravedad() { return G; }
}

