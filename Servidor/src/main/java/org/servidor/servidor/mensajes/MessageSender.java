package org.servidor.servidor.mensajes;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.servidor.servidor.socket.ClienteActivo;

import java.io.BufferedWriter;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.Objects;
import java.util.UUID;
import java.util.stream.Collectors;

public class MessageSender {

    private final ObjectMapper objectMapper = new ObjectMapper(); // El ObjectMapper para convertir objetos a JSON

    /**
     * Constructor de la clase MessageSender.
     */
    public MessageSender() {}

    /*private void sendMessageToClient(ClientInfo client, String message) {
        try {
            PrintWriter out = new PrintWriter(client.getSocket().getOutputStream(), true); // Prepara el flujo de salida
            out.println(message); // Envía el mensaje
            //System.out.println("Mensaje enviado al cliente " + client.getClientId() + ": " + message); // Registra el mensaje enviado
        } catch (IOException e) {
            System.err.println("Error al enviar mensaje al cliente " + client.getClientId() + ": " + e.getMessage()); // Registra el error durante el envío
        }
    }*/


    private String createJson(Object data) {
        try {
            // Convierte el objeto a JSON usando el ObjectMapper
            return objectMapper.writeValueAsString(data);
        } catch (JsonProcessingException e) {
            // Imprime el stack trace del error y retorna null en caso de fallo
            e.printStackTrace();
            return null;
        }
    }

    public void sendConfirmation(ClienteActivo cliente, int puntaje, int vidas, int control, String partida) {
        Socket socket = cliente.getSocket();
        String json = String.format(
                "{\"type_message\":\"start\",\"score\":%d,\"lifes\":%d, \"control\":%d, \"partida\":%s}",
                puntaje, vidas, control, partida
        );
        try {
            BufferedWriter w = new BufferedWriter(
                    new OutputStreamWriter(socket.getOutputStream(), StandardCharsets.UTF_8)
            );
            w.write(json);
            w.write("\n");  // un mensaje por línea
            w.flush();      // no cerramos el writer para no cerrar el socket
        } catch (Exception e) {
            System.err.println("Error enviando confirmación a " + cliente.getClientId() + ": " + e.getMessage());
        }
    }

    public void sendFullServer(Socket socket, UUID clientId) {
        final String json = "{\"type_message\":\"reach\",\"exp\":\"NoSpace\"}";
        try {
            BufferedWriter w = new BufferedWriter(
                    new OutputStreamWriter(socket.getOutputStream(), StandardCharsets.UTF_8)
            );
            w.write(json);
            w.write("\n"); // framing por línea
            w.flush();
        } catch (Exception e) {
            System.err.println("Error enviando 'reach/NoSpace' a " + clientId + ": " + e.getMessage());
        }
    }


    public void sendOptionstoS(Socket socket, UUID clientId, List<String> disponibles){
        try {
            // Normaliza: quita null/blank, máximo 2
            List<String> ops = (disponibles == null ? List.<String>of() : disponibles).stream()
                    .filter(Objects::nonNull)
                    .map(String::trim)
                    .filter(s -> !s.isEmpty())
                    .limit(2)
                    .collect(Collectors.toList());

            // "A"  /  "B"  /  "A, B"  /  "" (si no hay)
            String which = String.join(", ", ops);

            var msg = new java.util.HashMap<String, Object>();
            msg.put("type_message", "options");
            msg.put("which", which);

            String json = new ObjectMapper().writeValueAsString(msg);

            BufferedWriter w = new BufferedWriter(
                    new OutputStreamWriter(socket.getOutputStream(), StandardCharsets.UTF_8)
            );
            w.write(json);
            w.write("\n"); // framing por línea
            w.flush();
        } catch (Exception e) {
            System.err.println("Error enviando opciones a " + clientId + ": " + e.getMessage());
        }
    }

    // Para enviar el snapshot
    public void sendSnapshot(org.servidor.servidor.socket.ClienteActivo cliente,
                             org.servidor.servidor.juego.reglas.Snapshot snap) {
        try {
            var msg = new java.util.HashMap<String, Object>();
            msg.put("type_message", "snapshot");
            msg.put("tick", snap.tick);
            msg.put("score", snap.score);
            msg.put("lifes", snap.vidas);
            msg.put("speedFactor", snap.speedFactor);
            msg.put("entidades", snap.entidades);

            String json = objectMapper.writeValueAsString(msg);
            BufferedWriter w = new BufferedWriter(
                    new OutputStreamWriter(cliente.getSocket().getOutputStream(), StandardCharsets.UTF_8)
            );
            w.write(json);
            w.write("\n"); // NDJSON
            w.flush();
        } catch (Exception e) {
            System.err.println("Error enviando snapshot a " + cliente.getClientId() + ": " + e.getMessage());
        }
    }

    //  anunciar fin de partida
    public void sendGameOver(org.servidor.servidor.socket.ClienteActivo cliente, int scoreFinal) {
        try {
            var msg = new java.util.HashMap<String, Object>();
            msg.put("type_message", "game_over");
            msg.put("score", scoreFinal);
            String json = objectMapper.writeValueAsString(msg);

            BufferedWriter w = new BufferedWriter(
                    new OutputStreamWriter(cliente.getSocket().getOutputStream(), StandardCharsets.UTF_8)
            );
            w.write(json);
            w.write("\n");
            w.flush();
        } catch (Exception e) {
            System.err.println("Error enviando GAME_OVER a " + cliente.getClientId() + ": " + e.getMessage());
        }
    }


}
