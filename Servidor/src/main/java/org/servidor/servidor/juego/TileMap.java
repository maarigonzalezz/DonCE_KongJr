package org.servidor.servidor.juego;

import java.util.Arrays;

public final class TileMap {
    private final int tileSize;
    private final int cols;
    private final int rows;
    private final TileType[][] grid;

    public TileMap(int tileSize, int cols, int rows) {
        this.tileSize = tileSize;
        this.cols = cols;
        this.rows = rows;
        this.grid = new TileType[rows][cols];
        for (TileType[] r : grid) Arrays.fill(r, TileType.VACIO);
    }

    public int tileSize() { return tileSize; }
    public int cols() { return cols; }
    public int rows() { return rows; }

    public TileType getTile(int col, int row) {
        if (col < 0 || col >= cols || row < 0 || row >= rows) return TileType.ABISMO;
        return grid[row][col];
    }

    public void setTile(int col, int row, TileType t) {
        if (col < 0 || col >= cols || row < 0 || row >= rows) return;
        grid[row][col] = t;
    }

    // Esto es para rellenar con cualquier tipo
    public void fillRange(TileType type, int colStart, int colEnd, int row) {
        for (int c = colStart; c <= colEnd; c++) setTile(c, row, type);
    }


    /** ¿colisión por cara superior (one-way) en plataformas? */
    public boolean isSolidTop(int col, int row) {
        return getTile(col, row) == TileType.PLATAFORMA;
    }
    public boolean isLiana(int col, int row) {
        return getTile(col, row) == TileType.LIANA;
    }

    /** Utilidad para rellenar rangos horizontales de plataformas. */
    public void fillPlatformRange(int colStart, int colEnd, int row) {
        for (int c = colStart; c <= colEnd; c++) setTile(c, row, TileType.PLATAFORMA);
    }
}
