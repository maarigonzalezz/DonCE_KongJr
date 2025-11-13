package org.servidor.servidor.juego;

// Esta vara es solo para verificar que todo se hace bien , despues hay que quitarlo


public final class LevelSmoke {

    private static char glyph(TileType t) {
        return switch (t) {
            case VACIO      -> '.';
            case PLATAFORMA -> '_';
            case LIANA      -> '|';
            case ABISMO     -> 'X';
            case META       -> 'M';
        };
    }

    /** Dibuja el mapa en la consola (fila superior a inferior). */
    private static void printMap(Level lvl) {
        TileMap map = lvl.map();
        for (int r = map.rows() - 1; r >= 0; r--) {
            StringBuilder sb = new StringBuilder();
            for (int c = 0; c < map.cols(); c++) {
                sb.append(glyph(map.getTile(c, r)));
            }
            System.out.println(sb);
        }
    }

    public static void main(String[] args) {
        Level lvl = new Level();

        System.out.println("OK: Level creado. tile=" + Level.TILE_SIZE +
                " cols=" + Level.COLS + " rows=" + Level.ROWS);

        // --- Validaciones rápidas ---
        System.out.println("Lianas: " + lvl.lianas().size());
        System.out.println("Liana0 col=" + lvl.lianas().get(0).col()
                + " x(px)=" + lvl.lianas().get(0).x()
                + " admite fila 10? " + lvl.lianas().get(0).admiteFila(10));

        // Leer algunos tiles
        System.out.println("Tile(2,19) = " + lvl.map().getTile(2,19));
        System.out.println("Tile(7,13) = " + lvl.map().getTile(7,13));
        System.out.println("Tile(1,1)  = " + lvl.map().getTile(1,1));

        // Dibujo ASCII del mapa
        System.out.println("\n=== MAPA (ASCII) ===");
        printMap(lvl);

        System.out.println("\nSmoke test terminado.");
    }
}
