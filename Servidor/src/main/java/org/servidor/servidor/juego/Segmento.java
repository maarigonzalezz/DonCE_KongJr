package org.servidor.servidor.juego;

public record Segmento(int rowStart, int rowEnd) {
    public boolean contieneFila(int row) {
        return row >= rowStart && row <= rowEnd;
    }
}
