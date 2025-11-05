package org.servidor.servidor;

import javafx.fxml.FXML;
import javafx.scene.control.Label;
import org.servidor.servidor.socket.Servidor;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class AppController {
    @FXML
    private Label confirmacion;

    // --- Estado del servidor ---
    private Servidor servidor;
    private ExecutorService serverThread; // hilo para el bucle accept()

    @FXML
    protected void server_toggle() {
        if (servidor == null) {
            startServer();
            System.out.println("Conectadooo ");
        } else {
            stopServer();
        }
    }

    private void startServer() {
        // Evita dobles arranques
        if (servidor != null) return;

        servidor = new Servidor(5050);

        // Cierra ordenado cuando se cierre la JVM
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            try { servidor.stop(); } catch (Exception ignored) {}
        }));

        // Hilo único para el servidor (el método start() bloquea)
        serverThread = Executors.newSingleThreadExecutor(r -> {
            Thread t = new Thread(r, "servidor-thread");
            t.setDaemon(true); // no impide que la app cierre
            return t;
        });

        // Arrancar en background
        serverThread.submit(() -> {
            servidor.start(); // bucle accept() corre aquí
        });

        confirmacion.setText("ACTIVADO en puerto 5050");
        System.out.println("Servidor arrancando…");
    }

    private void stopServer() {
        if (servidor == null) return;

        try {
            servidor.stop();           // cierra ServerSocket y desbloquea accept()
            if (serverThread != null) {
                serverThread.shutdownNow();
            }
        } finally {
            servidor = null;
            serverThread = null;
        }

        confirmacion.setText("DETENIDO");
        System.out.println("Servidor detenido.");
    }
}