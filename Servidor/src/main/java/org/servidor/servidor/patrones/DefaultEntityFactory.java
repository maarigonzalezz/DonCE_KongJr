package org.servidor.servidor.patrones;

import org.servidor.servidor.juego.entidades.CocodriloAzul;
import org.servidor.servidor.juego.entidades.CocodriloRojo;
import org.servidor.servidor.juego.entidades.Fruta;
import org.servidor.servidor.juego.entidades.Entity;


public final class DefaultEntityFactory implements EntityFactory {

    // Si necesitas la config de lianas, la puedes recibir por el constructor
    // pero con tu diseño actual, no es obligatorio
    // private final LianasConfig lianasConfig;
    //
    // public DefaultEntityFactory(LianasConfig cfg) {
    //     this.lianasConfig = cfg;
    // }

    @Override
    public Entity createEntity(EntityKind kind, int lianaId, float alturaPct, int puntos) {
        return switch (kind) {
            case FRUTA -> crearFruta(lianaId, alturaPct, puntos);
            case COCODRILO_ROJO -> crearCocodriloRojo(lianaId);
            case COCODRILO_AZUL -> crearCocodriloAzul(lianaId);
        };
    }

    // ---------- Implementaciones concretas ----------

    private Fruta crearFruta(int lianaId, float alturaPct, int puntos) {
        return new Fruta(lianaId, puntos, alturaPct);
    }

    private CocodriloRojo crearCocodriloRojo(int lianaId) {
        float yInicial = 0f;      // la estrategia/Fruta se encargan luego de alinear con la liana
        float speed = 6.0f;       // ajusta a tu gusto
        return new CocodriloRojo(lianaId, speed);
    }

    private CocodriloAzul crearCocodriloAzul(int lianaId) {
        float yInicial = 0f;
        float speed = 6.0f;       // un poco más rápido, si quieres
        return new CocodriloAzul(lianaId, speed);
    }
}

