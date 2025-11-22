package org.servidor.servidor.juego;

import org.servidor.servidor.juego.entidades.CocodriloAzul;
import org.servidor.servidor.juego.entidades.CocodriloRojo;
import org.servidor.servidor.juego.entidades.Fruta;
import org.servidor.servidor.mensajes.MessageSender;
import org.servidor.servidor.socket.ClienteActivo;

import org.servidor.servidor.juego.reglas.GameLoop;
import org.servidor.servidor.juego.reglas.GameState;
import org.servidor.servidor.juego.reglas.Snapshot;

import org.servidor.servidor.juego.entidades.Entity;

import java.util.ArrayList;
import java.util.List;
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
    }

    // --- Constructor alternativo (inyección de GameState) ---
    public Sala(String partida, GameState injectedState) {
        this.partida = partida;
        this.messageSender = new MessageSender();
        this.gameState = (injectedState != null) ? injectedState : new GameState();
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

    /** Reinicia sala (sesión y simulación).
    public synchronized void reiniciarSala() {
        salaActiva = false;
        clientes.clear();
        observadores = 0;

        if (exec != null) { exec.shutdownNow(); exec = null; }
        loop = null;
        level = null;
        entities = null;

        if (gameState != null) gameState.reset(); // score=0, vidas=2, speedFactor=1.0, fase=RUNNING
    }

    // ------------ Inputs del cliente ------------
    /** Recibe acciones del cliente y las aplica al primer DKJr hallado.
    public void applyInput(String accion) {
        if (entities == null) return;

        DKJr dk = null;
        for (Entity e : entities) if (e instanceof DKJr) { dk = (DKJr) e; break; }
        if (dk == null) return;

        switch (accion.toLowerCase()) {
            case "climb_up"   -> dk.climbUp(2.0f);
            case "climb_down" -> dk.climbDown(2.0f);
            case "release"    -> dk.release();
            case "move_left"  -> dk.moveLeft();
            case "move_right" -> dk.moveRight();
        }

        //  AVANZAR UN TICK DEL JUEGO DESPUÉS DE CADA INPUT
        if (loop != null) {
            loop.tick(1f/30f); // Un frame de simulación
            broadcastSnapshot(buildSnapshot());
        }
    }*/

    /** Identificador de esta sala (para socket). */
    public String getPartida() { return partida; }

    /** Exponer GameState (opcional). */
    public GameState getGameState() { return gameState; }
}
