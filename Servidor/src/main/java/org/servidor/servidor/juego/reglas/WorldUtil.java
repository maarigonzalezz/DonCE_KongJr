package org.servidor.servidor.juego.reglas;

import org.servidor.servidor.juego.Level;
import org.servidor.servidor.juego.Liana;
import org.servidor.servidor.juego.Segmento;

public final class WorldUtil {
    private WorldUtil(){}

    public static int tile(Level lvl){ return lvl.map().tileSize(); }

    /** Convierte una coordenada en píxeles (centro) a columna/fila del grid. */
    public static int toCol(Level lvl, float x){ return Math.max(0, Math.min(lvl.map().cols()-1, (int)(x / tile(lvl)))); }
    public static int toRow(Level lvl, float y){ return Math.max(0, Math.min(lvl.map().rows()-1, (int)(y / tile(lvl)))); }

    /** Y (en px) del tope de la fila. */
    public static float rowTopY(Level lvl, int row){ return row * tile(lvl); }

    /** ¿Hay plataforma con cara superior sólida en (col,row)? */
    public static boolean isPlatformTop(Level lvl, int col, int row){
        return lvl.map().isSolidTop(col, row);
    }

    /** Busca liana cercana por X (umbral en píxeles) y que contenga la fila dada. Devuelve índice o -1. */
    public static int findLianaAt(Level lvl, float x, int row, float umbralPx){
        for (int i=0; i<lvl.lianas().size(); i++){
            Liana li = lvl.lianas().get(i);
            if (Math.abs(x - li.x()) <= umbralPx){
                // ¿algún segmento incluye la fila?
                for (Segmento s : li.segments()){
                    if (row >= s.rowStart() && row <= s.rowEnd()) return i;
                }
            }
        }
        return -1;
    }
}
