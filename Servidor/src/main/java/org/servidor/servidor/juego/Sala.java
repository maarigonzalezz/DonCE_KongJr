package org.servidor.servidor.juego;

import org.servidor.servidor.mensajes.MessageSender;
import org.servidor.servidor.socket.ClienteActivo;

import org.servidor.servidor.juego.reglas.GameLoop;
import org.servidor.servidor.juego.reglas.GameState;
import org.servidor.servidor.juego.reglas.Snapshot;

import org.servidor.servidor.juego.entidades.Entity;
import org.servidor.servidor.juego.entidades.DKJr;
import org.servidor.servidor.juego.entidades.CocodriloRojo;
import org.servidor.servidor.juego.entidades.CocodriloAzul;
import org.servidor.servidor.juego.entidades.Fruta;

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
    private final MessageSender messageSender;

    // --- Juego
    private Level level;
    private GameState gameState;
    private List<Entity> entities;
    private GameLoop loop;
    private ScheduledExecutorService exec;

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
        if ("jugador".equalsIgnoreCase(tipo)) {
            salaActiva = true;
            startLoopIfNeeded();
        } else {
            observadores++;
        }
        clientes.add(cliente);

        // Confirma con score/vidas desde GameState
        messageSender.sendConfirmation(cliente, gameState.score(), gameState.vidas());
        System.out.println("cliente añadido correctamente");
    }

    /** Mae en esta parte era que simulaba , ahora solo cuando el cliente mueve las teclas  */
    private synchronized void startLoopIfNeeded() {
        if (loop != null) return;

        level = new Level();
        entities = new ArrayList<>();

        // Solo el jugador
        int t = level.map().tileSize();
        entities.add(new DKJr(t * 7.5f, t * 2.0f));

        loop = new GameLoop(level, entities, gameState);


        // EL JUEGO SOLO AVANZARÁ CUANDO LLEGUEN INPUTS
        System.out.println("🎮 Juego listo - esperando inputs del jugador");
    }

    /** Reinicia sala (sesión y simulación). */
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

    // ------------ Snapshots ------------
    private int tickCounter = 0; // para snapshots

    private Snapshot buildSnapshot() {
        Snapshot s = new Snapshot();
        s.tick = tickCounter;
        s.score = gameState.score();
        s.vidas = gameState.vidas();
        s.speedFactor = gameState.speedFactor();

        if (entities != null) {
            for (Entity e : entities) {
                Snapshot.EntityState es = new Snapshot.EntityState();
                es.id = e.id.toString();
                es.x = e.x(); es.y = e.y();
                es.w = e.bbox().w; es.h = e.bbox().h;

                if (e instanceof DKJr) es.tipo = "DKJr";
                else if (e instanceof CocodriloRojo) es.tipo = "CocodriloRojo";
                else if (e instanceof CocodriloAzul) es.tipo = "CocodriloAzul";
                else if (e instanceof Fruta) es.tipo = "Fruta";
                else es.tipo = e.getClass().getSimpleName();

                s.entidades.add(es);
            }
        }
        return s;
    }

    private void broadcastSnapshot(Snapshot s) {
        for (ClienteActivo c : clientes) {
            messageSender.sendSnapshot(c, s);
        }
    }

    // ------------ Inputs del cliente ------------
    /** Recibe acciones del cliente y las aplica al primer DKJr hallado. */
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
    }

    /** Identificador de esta sala (para socket). */
    public String getPartida() { return partida; }

    /** Exponer GameState (opcional). */
    public GameState getGameState() { return gameState; }
}
