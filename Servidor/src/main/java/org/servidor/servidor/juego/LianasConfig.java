package org.servidor.servidor.juego;

import java.util.ArrayList;
import java.util.List;

public class LianasConfig {
    private final List<LianaInfo> lianas = new ArrayList<>();

    public LianasConfig() {
        // OJO: estos valores tienen que coincidir con los de C
        lianas.add(new LianaInfo( 1,  60.0f, 160.0f, 500.0f));
        lianas.add(new LianaInfo( 2, 140.0f, 160.0f, 480.0f));
        lianas.add(new LianaInfo( 3, 350.0f, 160.0f, 460.0f));
        lianas.add(new LianaInfo( 4, 430.0f, 160.0f, 360.0f));
        lianas.add(new LianaInfo( 5, 220.0f, 300.0f, 380.0f));
        lianas.add(new LianaInfo( 6, 220.0f, 400.0f, 500.0f));
        lianas.add(new LianaInfo( 7, 500.0f, 180.0f, 470.0f));
        lianas.add(new LianaInfo( 8, 560.0f, 180.0f, 400.0f));
        lianas.add(new LianaInfo( 9, 680.0f, 100.0f, 340.0f));
        lianas.add(new LianaInfo(10, 740.0f, 100.0f, 340.0f));
        lianas.add(new LianaInfo(11, 680.0f, 360.0f, 420.0f));
        lianas.add(new LianaInfo(12, 740.0f, 360.0f, 420.0f));
        lianas.add(new LianaInfo(13, 300.0f,  35.0f,  95.0f));
    }

    public LianaInfo getById(int id) {
        return lianas.get(id);
    }

    public List<LianaInfo> all() {
        return lianas;
    }

    public int count(){
        return lianas.size();
    }
}
