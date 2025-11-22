package org.servidor.servidor.juego.entidades;


/**
 * Cocodrilo que desciende por una liana y, al finalizar el segmento, cae por gravedad.
 * Estados: EN_LIANA --> CAYENDO. En CAYENDO no puede volver a agarrarse.
 * Usa MovimientoAzul para transición y actualización de velocidades.
 */


public final class CocodriloAzul extends Cocodrilo {

    public enum Estado { EN_LIANA, CAYENDO }

    // Estado actual del cocodrilo
    private Estado estado = Estado.EN_LIANA;

    // Velocidad vertical sólo mientras está en caída libre (CAYENDO)
    private float vy = 0f;

    // Gravedad en px/s^2 (ajusta al gusto)
    private static final float G = 900f;

    public CocodriloAzul(int lianaId, float speed) {
        super(lianaId, speed);
        this.estrategia = new MovimientoAzul();
    }

    // --- Accesores usados por la estrategia y el snapshot ---

    public Estado getEstado() {
        return estado;
    }

    public void setEstado(Estado estado) {
        this.estado = estado;
    }

    /**
     * Velocidad vertical durante la fase CAYENDO.
     */
    public float getVy() {
        return vy;
    }

    public void setVy(float vy) {
        this.vy = vy;
    }

    /**
     * Aceleración de la gravedad usada en MovimientoAzul.
     */
    public float gravedad() {
        return G;
    }

    @Override
    public int getLianaId() {
        return super.getLianaId();
    }


}

