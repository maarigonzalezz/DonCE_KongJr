package org.servidor.servidor.socket;

import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.concurrent.*;

import org.servidor.servidor.juego.Sala;

public class Servidor {
    private final int PORT;
    private final ExecutorService pool = Executors.newCachedThreadPool();
    private volatile boolean running = false;
    private ServerSocket serverSocket;

    public static final List<Sala> Salas = new ArrayList<>(); // Lista de Salas

    public Servidor(int port) {
        this.PORT = port;
        crearsalas();
    }

    public void start() {
        if (running) return;
        running = true;

        try (ServerSocket ss = new ServerSocket(PORT)) {
            this.serverSocket = ss;
            System.out.println("Servidor escuchando en puerto " + PORT);

            while (running) {
                try {
                    Socket s = ss.accept();
                    pool.execute(new ManejoCliente(s, this));
                } catch (IOException e) {
                    if (running) System.err.println("Error aceptando conexión: " + e.getMessage());
                    // si no está running, es porque estamos apagando: salir del loop
                }
            }
        } catch (IOException e) {
            System.err.println("No se pudo abrir el puerto " + PORT + ": " + e.getMessage());
        } finally {
            stop(); // asegura liberar recursos
        }
    }

    public void stop() {
        if (!running) return;
        running = false;
        try {
            if (serverSocket != null && !serverSocket.isClosed()) {
                serverSocket.close(); // desbloquea accept()
            }
        } catch (IOException ignore) { }
        pool.shutdownNow();
        System.out.println("Servidor detenido.");
    }

    private void crearsalas(){
        Sala salaA = new Sala("A");
        Sala salaB = new Sala("B");
        Salas.add(salaA);
        Salas.add(salaB);
    }

    /**
     * Busca la primera sala NO ocupada. Si existe, devuelve el identificador 'partida'.
     * Si no hay disponibles, devuelve Optional.empty().
     */
    public Optional<String> getEstadoSalas() {
        for (Sala s : Salas) {
            if (!s.SalaOcupada()) { // disponible
                return Optional.of(s.getPartida());
            }
        }
        return Optional.empty();
    }

    public List<String> getEspectadoresSalas() {
        List<String> disponibles = new ArrayList<>();
        for (Sala s : Salas) {
            if (s.tieneCupoParaEspectador()) {
                disponibles.add(s.getPartida());
            }
        }
        return disponibles; // si está vacío, no hay
    }


    public void addClienteaSala(ClienteActivo cliente, String partida, String jugador){
        for (Sala s : Salas){
            if (Objects.equals(s.getPartida(), partida)){
                s.setCliente(cliente, jugador);
                cliente.setPartida(partida);
                System.out.println("se agregó cliente de tipo " + jugador + " a la partida " + partida);
            }
        }

    }

}
