package org.servidor.servidor.mensajes;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.servidor.servidor.socket.Servidor;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.UUID;

public class MessageSender {

    private final Servidor server; // El servidor que gestiona las conexiones de los clientes
    private final UUID clientId; // El ID único del cliente al que se envían los mensajes
    private final ObjectMapper objectMapper = new ObjectMapper(); // El ObjectMapper para convertir objetos a JSON

    /**
     * Constructor de la clase MessageSender.
     *
     * @param server El servidor que se utilizará para enviar mensajes.
     * @param clientId El ID del cliente al que se enviarán los mensajes.
     */
    public MessageSender(Servidor server, UUID clientId) {
        this.server = server; // Inicializa el servidor
        this.clientId = clientId; // Inicializa el ID del cliente
    }

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

}
