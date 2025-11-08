package org.servidor.servidor.socket;

import java.net.Socket;
import java.util.List;
import java.util.Optional;
import java.util.UUID;

import org.servidor.servidor.mensajes.MessageSender;
import org.servidor.servidor.jsonparser.RegisterParser;
import org.servidor.servidor.juego.Sala;

public class RegistroCliente {
    private final Servidor server;     // Referencia al servidor
    private final Socket socket;      // Socket del cliente
    private final UUID clientId;      // ID único del cliente
    private final MessageSender messageSender;

    public RegistroCliente(Servidor server, Socket socket, UUID clientId) {
        this.server = server;           // Asignación del servidor
        this.socket = socket;           // Asignación del socket
        this.clientId = clientId;       // Asignación del ID del cliente
        messageSender = new MessageSender(server, clientId);

    }

    public void registerClientType(RegisterParser registerData) {
        String type = registerData.getType(); // Obtiene el tipo de cliente
        // Si es un jugador, crea un nuevo juego
        if ("jugador".equals(type)) {
            manejarJugador(type);
        } else { // Si es un espectador
            manejarEspectador(type);
        }

    }

    private void manejarJugador(String type) {
        Optional<String> salaDisponible = server.getEstadoSalas();
        if (salaDisponible.isPresent()) {
            String partida = salaDisponible.get();
            System.out.println("Sala disponible: " + partida);
            ClienteActivo clienteActivo = new ClienteActivo();
            server.addClienteaSala(clienteActivo, partida, type);
        } else {
            System.out.println("No hay salas disponibles.");
        }
    }

    private void manejarEspectador(String type){
        List<String> disponibles = server.getEspectadoresSalas();
        if (!disponibles.isEmpty()) {
            System.out.println("Salas con cupo: " + disponibles);
        } else {
            System.out.println("No hay salas disponibles para espectadores.");
        }
    }


}
