package org.servidor.servidor.juego.reglas;

/**
 * Estado global de la partida (servidor).
 * score, vidas y speedFactor son la “fuente de verdad” para Sala y GameLoop.
 */
public final class GameState {
    private int score = 0;
    private int vidas = 2;          // <-- inicia en 2 para igualar la Sala actual
    private float speedFactor = 6.0f;
    public int score() { return score; }
    public int vidas() { return vidas; }
    public float speedFactor() { return speedFactor; }

    public void victoryBoost(float factor) {
        speedFactor *= factor;
        vidas += 1;
    }

    public void perderVida(){
        vidas -= 1;
    }

    public void updateScore(int points){
        score += points;
    }


    /** Reinicia marcadores sin recrear GameState (útil para “reiniciarSala”). */
    public void reset() {
        score = 0;
        vidas = 2;
        speedFactor = 3.0f;
    }
}
