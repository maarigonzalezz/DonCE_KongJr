package org.servidor.servidor.socket;

import java.net.Socket;
import java.util.UUID;

public class ClienteActivo {
    private Socket socket;             // Socket de la conexión del cliente
    private UUID clientId;             // ID del cliente en formato GUID
    private String client_type;        // Tipo de cliente: "jugador", "espectador" o "Client"
    private String partida;        // Tipo de cliente: "jugador", "espectador" o "Client"

    public ClienteActivo(Socket socket, UUID clientId, String client_type){
        this.socket = socket;
        this.clientId = clientId;
        this.client_type = client_type;
    }

    public void setPartida(String partida) {
        this.partida = partida;
    }

    public String getPartida() {
        return partida;
    }

    public Socket getSocket() {
        return socket;
    }

    public UUID getClientId() {
        return clientId;
    }
}
