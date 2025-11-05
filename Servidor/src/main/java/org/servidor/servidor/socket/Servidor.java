package org.servidor.servidor.socket;

import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.*;

public class Servidor {
    private final int PORT;
    private volatile int jugadores = 0;

    private final ExecutorService pool = Executors.newCachedThreadPool();
    private volatile boolean running = false;
    private ServerSocket serverSocket;

    public Servidor(int port) {
        this.PORT = port;
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
                    pool.execute(new ClientHandler(s));
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

    // (Opcional) getters/setters para jugadores si los necesitas
}
