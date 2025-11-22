package org.servidor.servidor.juego.reglas;

/**
 * Estado global de la partida (servidor).
 * score, vidas y speedFactor son la “fuente de verdad” para Sala y GameLoop.
 */
public final class GameState {
    private int score = 0;
    private int vidas = 2;          // <-- inicia en 2 para igualar la Sala actual
    private float speedFactor = 4.0f;
    public int score() { return score; }
    public int vidas() { return vidas; }
    public float speedFactor() { return speedFactor; }


    public void addScore(int pts) { score += pts; }

    public void loseLife() {
       // vidas = Math.max(0, vidas - 1);
        //if (vidas == 0) fase = Fase.GAME_OVER;
    }

    public void victoryBoost(float factor) {
        speedFactor *= factor;
    }


    /** Reinicia marcadores sin recrear GameState (útil para “reiniciarSala”). */
    public void reset() {
        score = 0;
        vidas = 2;
        speedFactor = 1.0f;
    }
}
