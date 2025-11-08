package org.servidor.servidor.mensajes;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.servidor.servidor.socket.Servidor;
import org.servidor.servidor.jsonparser.RegisterParser;
import org.servidor.servidor.socket.RegistroCliente;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.UUID;


public class MessageManagement {
    private final Servidor server;
    private final Socket socket;
    private final UUID clientId;
    private final ObjectMapper objectMapper = new ObjectMapper();
    private final MessageSender messageSender;
    private final RegistroCliente clientRegistration;


    // Constructor de MessageManagement
    public MessageManagement(Servidor server, Socket socket, UUID clientId) {
        this.server = server; // Inicializa la instancia del servidor
        this.socket = socket; // Inicializa el socket del cliente
        this.clientId = clientId; // Inicializa el identificador único del cliente
        this.messageSender = new MessageSender(server, clientId);
        this.clientRegistration = new RegistroCliente(server, socket, clientId);
    }

    /**
     * Maneja los mensajes JSON entrantes del cliente.
     *
     * @param messageJson El mensaje recibido en formato JSON.
     */
    public void handleMessage(String messageJson) {
        // Verifica si el mensaje está vacío
        if (messageJson.isEmpty()) {
            System.out.println("Se recibió un mensaje vacío."); // Notifica que el mensaje está vacío
            return; // Sale del método
        }

        try {
            // Convierte el mensaje JSON en un nodo
            JsonNode jsonNode = objectMapper.readTree(messageJson);
            // Obtiene el tipo de mensaje del nodo
            String messageType = jsonNode.path("type_message").asText();

            // Dirige el mensaje al manejador correcto según el tipo
            switch (messageType) {
                case "register": // Si el tipo es "register"
                    System.out.println("Llegó mensaje: " + messageType);
                    handleRegistration(jsonNode); // Maneja el registro
                    break;
                //case LA IDEA ES IR METIENDO CASOS SEGUN LO QUE RECIBAMOS DEL CLIENTE
                default: // Si el tipo de mensaje es desconocido
                    System.out.println("Tipo de mensaje desconocido: " + messageType);
            }
        } catch (IOException e) {
            // Maneja excepciones al procesar el mensaje
            System.err.println("Error procesando el mensaje: " + e.getMessage());
        }
    }

    private void handleRegistration(JsonNode jsonNode) {
        try {
            // Convierte el nodo JSON en un objeto Register_Data
            RegisterParser registerData = objectMapper.treeToValue(jsonNode, RegisterParser.class);
            System.out.println("ID del cliente: " + clientId); // Muestra el ID del cliente
            // Registra el tipo de cliente (Jugador o espectador)
            clientRegistration.registerClientType(registerData);
        } catch (IOException e) {
            // Maneja excepciones al procesar el registro
            System.err.println("Error procesando el registro: " + e.getMessage());
        }
    }





}
