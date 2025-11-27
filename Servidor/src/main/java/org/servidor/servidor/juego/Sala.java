package org.servidor.servidor.juego;

import com.fasterxml.jackson.databind.JsonNode;
import org.servidor.servidor.juego.entidades.CocodriloAzul;
import org.servidor.servidor.juego.entidades.CocodriloRojo;
import org.servidor.servidor.juego.entidades.Fruta;
import org.servidor.servidor.mensajes.MessageSender;
import org.servidor.servidor.patrones.DefaultEntityFactory;
import org.servidor.servidor.patrones.EntityFactory;
import org.servidor.servidor.patrones.EntityKind;
import org.servidor.servidor.socket.ClienteActivo;

import org.servidor.servidor.juego.reglas.GameLoop;
import org.servidor.servidor.juego.reglas.GameState;
import org.servidor.servidor.juego.reglas.Snapshot;

import org.servidor.servidor.juego.entidades.Entity;

import java.util.*;
import java.util.concurrent.*;

/**
 * Orquestador de la partida a nivel de red/sesión.
 * Mantiene clientes y expone el estado a través de MessageSender.
 * El puntaje y las vidas tienen como única fuente a GameState.
 */
public class Sala {

    // --- Estado de la sala / sesión ---
    private final String partida;
    private boolean salaActiva = false;
    private int observadores = 0;
    public final List<ClienteActivo> clientes = new ArrayList<>(); // conectados
    public final List<ClienteActivo> observers = new ArrayList<>(); // conectados
    private final MessageSender messageSender;
    private final EntityFactory entityFactory;

    // --- Juego
    private GameState gameState;
    private List<Entity> entities;

    private GameLoop loop;
    private ScheduledExecutorService exec;
    private int tickCounter = 0; // para snapshots

    // --- Constructor principal ---
    public Sala(String partida) {
        this.partida = partida;
        this.messageSender = new MessageSender();
        this.gameState = new GameState(); // vidas=2 por defecto
        this.entityFactory = new DefaultEntityFactory();
    }


    public boolean SalaOcupada() { return salaActiva; }

    public boolean tieneCupoParaEspectador() {
        return salaActiva && observadores < 2;
    }

    /** Agrega cliente; si es jugador, activa sala y arranca loop si no está corriendo. */
    public void setCliente(ClienteActivo cliente, String tipo) {
        int control;
        if ("jugador".equalsIgnoreCase(tipo)) {
            salaActiva = true;
            control = 1;
            startLoopIfNeeded();
        } else {
            observadores++;
            control = 0;
            observers.add(cliente);
        }
        clientes.add(cliente);

        // Confirma con score/vidas desde GameState
        // aqui se deben de mandar las entidades también
        messageSender.sendConfirmation(cliente, gameState.score(), gameState.vidas(), control, partida);
        System.out.println("cliente añadido correctamente");
    }

    // Mae en esta parte era que simulaba , ahora solo cuando el cliente mueve las teclas
    /** Arranca el game loop del servidor si aún no está corriendo. */
    private synchronized void startLoopIfNeeded() {
        // Si ya hay un loop activo, no hacemos nada
        if (exec != null) {
            return;
        }

        // Asegurarse de que haya listas inicializadas
        if (entities == null) {
            entities = new ArrayList<>();
        }

        // Crear el GameLoop del servidor (solo lógica de cocodrilos/frutas)
        loop = new GameLoop(entities, gameState);
        // crear frutas y enemigos iniciales
        loop.startEntities();

        // Crear un executor de un solo hilo para esta sala
        exec = Executors.newSingleThreadScheduledExecutor();

        // dt en segundos (~30 FPS)
        final float DT = 1f / 30f;

        // Programar la tarea periódica
        exec.scheduleAtFixedRate(() -> {
            try {
                // 1) avanzar la simulación del servidor
                loop.tick(DT);

                // 2) aumentar contador de tick para el Snapshot
                tickCounter++;

                // 3) construir snapshot y mandarlo a todos los clientes
                Snapshot s = buildSnapshot();
                broadcastSnapshot(s);

            } catch (Exception e) {
                e.printStackTrace();
            }
        }, 0, 33, TimeUnit.MILLISECONDS); // ~30 veces por segundo

        System.out.println("Game loop de la sala " + partida + " iniciado.");
    }

    // ------------ Snapshots ------------

    private Snapshot buildSnapshot() {
        Snapshot s = new Snapshot();
        s.tick = tickCounter;
        s.score = gameState.score();
        s.vidas = gameState.vidas();
        s.speedFactor = gameState.speedFactor();

        if (entities != null) {
            for (Entity e : entities) {
                Snapshot.EntityState entityState = new Snapshot.EntityState();
                entityState.id = e.getEntityId().toString();
                entityState.x = e.x(); entityState.y = e.y();

                if (e instanceof CocodriloRojo) entityState.tipo = "CocodriloRojo";
                else if (e instanceof CocodriloAzul) entityState.tipo = "CocodriloAzul";
                else if (e instanceof Fruta) entityState.tipo = "Fruta";
                else entityState.tipo = e.getClass().getSimpleName();

                s.entidades.add(entityState);
            }
        }
        return s;
    }

    private void broadcastSnapshot(Snapshot s) {
        for (ClienteActivo c : clientes) {
            messageSender.sendSnapshot(c, s);
        }
    }

    // Reinicia sala (sesión y simulación).
    public synchronized void reiniciarSala() {
        salaActiva = false;
        clientes.clear();
        observadores = 0;

        if (exec != null) { exec.shutdownNow(); exec = null; }
        loop = null;
        entities = null;

        if (gameState != null) gameState.reset(); // score=0, vidas=2, speedFactor=1.0, fase=RUNNING
    }

    /** Identificador de esta sala (para socket). */
    public String getPartida() { return partida; }

    /** Exponer GameState (opcional). */
    public GameState getGameState() { return gameState; }

    public synchronized void handleClientMessage( JsonNode msg) {
        String type = msg.path("type_message").asText("");

        switch (type) {
            case "death":
                loop.manejarDeath();
                verificarVidas();
                break;
            case "fruit_destroyed":
                loop.manejarFruitDestroyed(msg);
                break;
            case "win":
                loop.manejarWin();
                break;
            case "jr_pos":
                sendJrtoObservers(msg);
                break;
            default:
                System.out.println("Mensaje de juego desconocido: " + type);
        }
    }

    private void verificarVidas() {
        if (gameState.vidas() <= 0) {
            System.out.println("Partida " + partida + " GAME OVER");
            for (ClienteActivo c : clientes) {
                messageSender.send_GameOver(c);
            }
            reiniciarSala();
            // REINICIAR SALA
        }
    }


    public synchronized void coordinarSalida(JsonNode jsonNode, UUID clientId) {
        String reason = jsonNode.path("reason").asText(null);

        // 1) Buscar el cliente saliente
        ClienteActivo saliente = buscarClientePorId(clientId);
        if (saliente == null) {
            System.out.println("coordinarSalida: cliente " + clientId + " no encontrado en sala " + partida);
            return;
        }

        String tipo = saliente.getClient_type(); // "jugador" o "espectador" (según tu diseño)

        System.out.printf("Salida de cliente %s (tipo=%s, reason=%s) en sala %s%n",
                clientId, tipo, reason, partida);

        // 2) Branch por tipo
        if ("jugador".equalsIgnoreCase(tipo)) {
            manejarSalidaJugador(saliente, reason);
        } else {
            manejarSalidaEspectador(saliente);
        }
    }

    private ClienteActivo buscarClientePorId(UUID clientId) {
        for (ClienteActivo c : clientes) {
            if (Objects.equals(c.getClientId(), clientId)) {
                return c;
            }
        }
        return null;
    }

    private void manejarSalidaEspectador(ClienteActivo espectador) {
        // Quitar de la lista de clientes
        boolean removed = clientes.remove(espectador);
        observers.remove(espectador);

        if (removed) {
            // Decrementar contador de observadores si lo estás usando
            observadores = Math.max(0, observadores - 1);
            System.out.println("Espectador " + espectador.getClientId() +
                    " salió de la sala " + partida + ". Observadores restantes: " + observadores);
        } else {
            System.out.println("No se pudo remover espectador, no estaba en la lista: " + espectador.getClientId());
        }
    }

    private void manejarSalidaJugador(ClienteActivo jugador, String reason) {
        // Por si quieres distinguir entre "se desconectó" y "salió voluntariamente"
        boolean abandonoVoluntario = "jout".equalsIgnoreCase(reason);

        if (abandonoVoluntario) {
            // 1) Notificar a los observadores que la partida terminó
            for (ClienteActivo c : observers){
                messageSender.send_GameOver(c);
            }

            // 2) Limpiar la sala (parar loop, vaciar listas, resetear estado)
            reiniciarSala();
        }
    }

    public List<Entity> getEntitiesSnapshot() {
        synchronized (this) {
            if (entities == null) {
                // Aún no se inicializa la lista: devolvemos una lista vacía
                return Collections.emptyList();
            }
            return new ArrayList<>(entities);
        }
    }

    public void sendJrtoObservers(JsonNode jrmessage){
        for (ClienteActivo c : observers){
            messageSender.send_JrPosition(c, jrmessage);
        }
    }


    public synchronized void eliminarFrutaPorId(UUID entityId) {
        if (entities == null) return;

        Iterator<Entity> it = entities.iterator();
        while (it.hasNext()) {
            Entity e = it.next();
            if (e instanceof Fruta && Objects.equals(e.getEntityId(), entityId)) {
                Fruta f = (Fruta) e;
                System.out.println("Eliminando fruta " + entityId + " (pts=" + f.getPuntos() + ") de sala " + partida);
                it.remove();
                break;
            }
        }
    }

    public synchronized void crearEntidadAdmin(String tipoStr, int lianaId, float alturaPct, int puntos) {
        EntityKind kind = mapTipoToKind(tipoStr);
        if (kind == null) {
            System.out.println("Tipo de entidad desconocido: " + tipoStr);
            return;
        }

        Entity nueva = entityFactory.createEntity(kind, lianaId, alturaPct, puntos);
        if (nueva != null) {
            entities.add(nueva);
            System.out.printf("Creada entidad %s en sala %s (liana=%d, altura=%.1f)%n",
                    kind, partida, lianaId, alturaPct);
        }
    }

    private EntityKind mapTipoToKind(String tipoStr) {
        if (tipoStr == null) return null;

        return switch (tipoStr) {
            case "Fruta" -> EntityKind.FRUTA;
            case "Cocodrilo rojo" -> EntityKind.COCODRILO_ROJO;
            case "Cocodrilo azul" -> EntityKind.COCODRILO_AZUL;
            default -> null;
        };
    }
}
