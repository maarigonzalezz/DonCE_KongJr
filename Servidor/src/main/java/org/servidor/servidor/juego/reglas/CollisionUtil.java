package org.servidor.servidor.juego.reglas;

import org.servidor.servidor.juego.entidades.BoundingBox;
import org.servidor.servidor.juego.entidades.Entity;

/**
 * Utilidades de colisión AABB entre entidades.
 */
public final class CollisionUtil {
    private CollisionUtil() {}

    public static boolean intersects(Entity a, Entity b) {
        BoundingBox ab = a.bbox();
        BoundingBox bb = b.bbox();
        return ab.intersects(a.x(), a.y(), bb, b.x(), b.y());
    }
}
