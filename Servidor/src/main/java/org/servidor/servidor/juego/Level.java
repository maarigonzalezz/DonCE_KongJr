package org.servidor.servidor.juego;

import java.util.ArrayList;
import java.util.List;

public final class Level {
    public static final int TILE_SIZE = 16;
    public static final int COLS = 15;
    public static final int ROWS = 20;

    private final TileMap map = new TileMap(TILE_SIZE, COLS, ROWS);
    private final List<Liana> lianas = new ArrayList<>();

    public Level() {
        // --- plataformas (colInicio, colFin, fila) ---
        map.fillPlatformRange(1, 15, 1);
        map.fillPlatformRange(3, 5, 4);    map.fillPlatformRange(10, 12, 4);
        map.fillPlatformRange(2, 3, 7);    map.fillPlatformRange(6, 8, 7);   map.fillPlatformRange(12, 13, 7);
        map.fillPlatformRange(5, 6, 10);   map.fillPlatformRange(9, 11, 10);
        map.fillPlatformRange(2, 4, 13);   map.fillPlatformRange(7, 9, 13);  map.fillPlatformRange(12, 14, 13);
        map.fillPlatformRange(4, 6, 16);   map.fillPlatformRange(10, 12, 16);
        map.fillRange(TileType.META, 2, 5, 19); // esta usa la META, ose la zona de llegada


        // --- lianas (id, col, segmentos) ---
        addLiana(0, 2,  new Segmento(2,18));
        addLiana(1, 4,  new Segmento(5,19));
        addLiana(2, 7,  new Segmento(8,16), new Segmento(4,7));
        addLiana(3, 9,  new Segmento(11,16), new Segmento(7,10));
        addLiana(4, 12, new Segmento(8,19));
        addLiana(5, 14, new Segmento(2,18));

        // Marca visual de lianas en el grid (opcional)
        for (Liana li : lianas) {
            for (Segmento s : li.segments())
                for (int r = s.rowStart(); r <= s.rowEnd(); r++)
                    map.setTile(li.col(), r, TileType.LIANA);
        }
    }

    private void addLiana(int id, int col, Segmento... segs) {
        lianas.add(new Liana(id, col, List.of(segs), TILE_SIZE));
    }

    public TileMap map() { return map; }
    public List<Liana> lianas() { return List.copyOf(lianas); }
}
