package org.servidor.servidor.socket;

import java.io.*;
import java.net.Socket;
import java.nio.charset.StandardCharsets;

class ClientHandler implements Runnable {
    private final Socket socket;

    ClientHandler(Socket s) { this.socket = s; }

    @Override
    public void run() {
        String who = socket.getRemoteSocketAddress().toString();
        System.out.println("Conectado: " + who);
        try (var in = new BufferedReader(
                new InputStreamReader(socket.getInputStream(), StandardCharsets.UTF_8));
             var out = new BufferedWriter(
                     new OutputStreamWriter(socket.getOutputStream(), StandardCharsets.UTF_8))) {

            out.write("WELCOME\n"); out.flush();
            String line;
            while ((line = in.readLine()) != null) {
                System.out.println("[" + who + "] " + line);
                out.write("Se recibio: " + line + "\n");
                out.flush();
            }
        } catch (IOException e) {
            System.out.println("Cliente cerrado: " + who + " (" + e.getMessage() + ")");
        } finally {
            try { socket.close(); } catch (IOException ignore) {}
        }
    }
}
