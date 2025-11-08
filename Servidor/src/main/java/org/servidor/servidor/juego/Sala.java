package org.servidor.servidor.juego;

import org.servidor.servidor.mensajes.MessageSender;
import org.servidor.servidor.socket.ClienteActivo;

import java.util.ArrayList;
import java.util.List;

public class Sala {
    private int puntaje = 0;
    private int vidas = 2;

    private int observadores = 0;
    private String partida;
    private boolean salaActiva = false;

    public List<ClienteActivo> clientes = new ArrayList<>(); // Lista de clientes conectados
    private final MessageSender messageSender;


    public Sala(String partida){

        this.partida = partida;
        this.messageSender = new MessageSender();
    }

    public boolean SalaOcupada(){
        return salaActiva;
    }

    public boolean tieneCupoParaEspectador() {
        return salaActiva && observadores < 2;
    }

    public void setCliente(ClienteActivo cliente, String tipo){
        if ("jugador".equals(tipo)) {
            salaActiva = true;
        } else {
            observadores ++;
        }
        clientes.add(cliente);
        messageSender.sendConfirmation(cliente, puntaje, vidas);
        System.out.println("cliente añadido correctamente");
    }

    public void reiniciarSala(){
        salaActiva = false;
        clientes.clear();
        puntaje = 0;
        vidas = 2;
        observadores = 0;
    }

    public String getPartida() {
        return partida;
    }





}
