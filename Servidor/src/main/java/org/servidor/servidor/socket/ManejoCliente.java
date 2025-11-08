package org.servidor.servidor.socket;

import java.io.*;
import java.net.Socket;
import java.util.UUID;

import org.servidor.servidor.mensajes.MessageManagement;

class ManejoCliente implements Runnable {
    private final Socket socket;
    private final UUID clientId;         // Identificador único del cliente
    private final Servidor server;         // Instancia del servidor que maneja la conexión


    ManejoCliente(Socket s, Servidor server) {
        this.socket = s;
        this.server = server;
        this.clientId = UUID.randomUUID(); // Generación de un ID único para el cliente
    }

    @Override
    public void run() {
        String who = socket.getRemoteSocketAddress().toString();
        System.out.println("Conectado: " + who);
        try (BufferedReader input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
             PrintWriter output = new PrintWriter(socket.getOutputStream(), true)) {

            String messageJson;  // Variable para almacenar mensajes en formato JSON
            MessageManagement messageManagement = new MessageManagement(server, socket, clientId); // Manejador de mensajes

            // Escucha continuamente los mensajes del cliente
            while ((messageJson = input.readLine()) != null) {
                messageManagement.handleMessage(messageJson); // Procesa el mensaje
            }
        } catch (IOException e) {
            System.out.println("Cliente cerrado: " + who + " (" + e.getMessage() + ")");
        } finally {
            try { socket.close(); } catch (IOException ignore) {}
        }
    }
}
