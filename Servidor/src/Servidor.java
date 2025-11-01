import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.*;

public class Servidor {
    static final int PORT = 5050;

    public static void main(String[] args) throws IOException {
        var pool = Executors.newCachedThreadPool();
        try (ServerSocket server = new ServerSocket(PORT)) {
            System.out.println("Servidor escuchando en puerto " + PORT);
            while (true) {
                Socket s = server.accept();
                pool.execute(new ClientHandler(s));
            }
        }
    }

    static class ClientHandler implements Runnable {
        private final Socket socket;
        ClientHandler(Socket s) { this.socket = s; }

        @Override public void run() {
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
                    out.write("ECHO " + line + "\n");
                    out.flush();
                }
            } catch (IOException e) {
                System.out.println("Cliente cerrado: " + who + " (" + e.getMessage() + ")");
            } finally {
                try { socket.close(); } catch (IOException ignore) {}
            }
        }
    }

}
