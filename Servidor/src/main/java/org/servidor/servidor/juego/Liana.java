package org.servidor.servidor.juego;

import java.util.List;

public final class Liana {
    private final int id;
    private final int col;                 // columna de la grilla (0..cols-1)
    private final List<Segmento> segments;
    private final int tileSize;

    public Liana(int id, int col, List<Segmento> segments, int tileSize) {
        this.id = id;
        this.col = col;
        this.segments = List.copyOf(segments);
        this.tileSize = tileSize;
    }

    public int id() { return id; }
    public int col() { return col; }
    public List<Segmento> segments() { return segments; }

    /** x en píxeles del centro de agarre */
    public int x() { return (col * tileSize) + tileSize / 2; }

    /** ¿La fila dada pertenece a algún segmento de esta liana? */
    public boolean admiteFila(int row) {
        for (Segmento s : segments) if (s.contieneFila(row)) return true;
        return false;
    }
}
