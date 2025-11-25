package org.servidor.servidor;

import javafx.animation.Animation;
import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.image.ImageView;
import javafx.util.Callback;
import javafx.util.Duration;
import org.servidor.servidor.juego.Sala;
import org.servidor.servidor.socket.Servidor;
import org.servidor.servidor.juego.entidades.*;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class AppController {
    @FXML
    private Label confirmacion;

    // --- Estado del servidor ---
    private Timeline refreshTimeline;
    private Servidor servidor;
    private ExecutorService serverThread; // hilo para el bucle accept()
    private final ObservableList<Entity> itemsSalaA = FXCollections.observableArrayList();
    private final ObservableList<Entity> itemsSalaB = FXCollections.observableArrayList();
    private Entity entidadSeleccionada;
    private String salaSeleccionada;

    @FXML
    private void initialize() {
        // Opciones para la liana
        menu_liana_btn.setItems(FXCollections.observableArrayList(
                "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13"
        ));
        menu_liana_btn.getSelectionModel().selectFirst();

        // Opciones para entidades
        menu_entities.setItems(FXCollections.observableArrayList(
                "Fruta",
                "Cocodrilo rojo",
                "Cocodrilo azul"
        ));

        // Opciones para sala
        menu_sala.setItems(FXCollections.observableArrayList("A", "B"));

        alt_lbl.textProperty().bind(
                altura_slider.valueProperty().asString("%.0f")
        );

        // Listas para salas A y B
        lista_A.setItems(itemsSalaA);
        lista_B.setItems(itemsSalaB);

        // Cómo se muestra cada Entity en el ListView
        Callback<ListView<Entity>, ListCell<Entity>> cellFactory =
                new Callback<ListView<Entity>, ListCell<Entity>>() {
                    @Override
                    public ListCell<Entity> call(ListView<Entity> listView) {
                        return new ListCell<Entity>() {
                            @Override
                            protected void updateItem(Entity e, boolean empty) {
                                super.updateItem(e, empty);
                                if (empty || e == null) {
                                    setText(null);
                                } else {
                                    setText(formatEntity(e));
                                }
                            }
                        };
                    }
                };

        lista_A.setCellFactory(cellFactory);
        lista_B.setCellFactory(cellFactory);

        lista_A.getSelectionModel().selectedItemProperty().addListener((obs, oldV, newV) -> {
            if (newV != null) {
                entidadSeleccionada = newV;
                salaSeleccionada = "A";
                System.out.println("Seleccionada en A: " + formatEntity(newV));
            }
        });

        lista_B.getSelectionModel().selectedItemProperty().addListener((obs, oldV, newV) -> {
            if (newV != null) {
                entidadSeleccionada = newV;
                salaSeleccionada = "B";
                System.out.println("Seleccionada en B: " + formatEntity(newV));
            }
        });

        startAutoRefresh();
    }

    private String formatEntity(Entity e) {
        // Ajusta según tus clases reales
        if (e instanceof Fruta f) {
            return "Fruta  | liana=" + f.lianaId() + " | pts=" + f.getPuntos();
        } else if (e instanceof CocodriloRojo c) {
            return "Coc. rojo | liana=" + c.lianaId();
        } else if (e instanceof CocodriloAzul c) {
            return "Coc. azul | liana=" + c.lianaId();
        } else {
            return e.getClass().getSimpleName();
        }
    }
    private void refreshEntityLists() {
        if (servidor == null) return;

        // Usa el servidor que CREASTE en startServer()
        Sala salaA = servidor.getSala("A");
        Sala salaB = servidor.getSala("B");
        if (salaA == null || salaB == null) return;

        Platform.runLater(() -> {
            itemsSalaA.setAll(salaA.getEntitiesSnapshot());
            itemsSalaB.setAll(salaB.getEntitiesSnapshot());
        });
    }


    public void startAutoRefresh() {
        if (refreshTimeline != null) {
            return; // ya está corriendo
        }

        refreshTimeline = new Timeline(
                new KeyFrame(Duration.millis(500), e -> refreshEntityLists())
        );
        refreshTimeline.setCycleCount(Animation.INDEFINITE);
        refreshTimeline.play();
    }

    @FXML
    private void crearEntidad() {
        if (servidor == null) {
            confirmacion.setText("Servidor no está activo");
            return;
        }

        String salaId   = menu_sala.getValue();        // "A" o "B"
        String tipoStr  = menu_entities.getValue();    // "Fruta", "Cocodrilo rojo", "Cocodrilo azul"
        String lianaStr = menu_liana_btn.getValue();   // "1" .. "13"

        if (salaId == null || tipoStr == null || lianaStr == null) {
            confirmacion.setText("Seleccione sala, entidad y liana");
            return;
        }

        // --- liana: de "1..13" en UI a 0..12 en código ---
        int lianaIdx;
        try {
            int lianaNumero = Integer.parseInt(lianaStr) - 1; // 1..13
            lianaIdx = lianaNumero;                   // 0..12
        } catch (NumberFormatException e) {
            confirmacion.setText("Liana inválida");
            return;
        }

        // --- puntos desde el TextField ---
        int puntos = 0;
        if ("Fruta".equals(tipoStr)) {
            String txtPuntos = points_space.getText();
            if (txtPuntos == null || txtPuntos.isBlank()) {
                confirmacion.setText("Ingrese los puntos de la fruta");
                return;
            }
            try {
                puntos = Integer.parseInt(txtPuntos.trim());
            } catch (NumberFormatException e) {
                confirmacion.setText("Puntos inválidos (deben ser un número entero)");
                return;
            }
        }

        float alturaPct = (float) altura_slider.getValue(); // 0..100

        // tu servidor ahora recibe también los puntos
        servidor.crearEntidadAdmin(salaId, tipoStr, lianaIdx, alturaPct, puntos);

        confirmacion.setText("Entidad creada en sala " + salaId);
    }



    @FXML
    protected void server_toggle() {
        if (servidor == null) {
            startServer();
            System.out.println("Conectadooo ");
        } else {
            stopServer();
        }
    }

    @FXML
    private Slider altura_slider;

    @FXML
    private ListView<Entity> lista_A;

    @FXML
    private ImageView imagen;

    @FXML
    private ListView<Entity> lista_B;

    @FXML
    private ChoiceBox<String> menu_liana_btn;

    @FXML
    private ChoiceBox<String> menu_entities;

    @FXML
    private Button btn_crear;

    @FXML
    private Button btn_eliminar;

    @FXML
    private ChoiceBox<String> menu_sala;

    @FXML
    private Label lbl1;

    @FXML
    private Label lbl2;

    @FXML
    private Label lbl3;

    @FXML
    private Label lbl31;

    @FXML
    private TextField points_space;

    @FXML
    private Label alt_lbl;

    @FXML
    private void eliminarFruta() {
        if (entidadSeleccionada == null || salaSeleccionada == null) {
            confirmacion.setText("Seleccione primero una fruta en A o B");
            return;
        }

        if (!(entidadSeleccionada instanceof Fruta fruta)) {
            confirmacion.setText("Solo se pueden eliminar frutas");
            return;
        }

        if (servidor == null) {
            confirmacion.setText("Servidor no está activo");
            return;
        }

        // Pedimos al servidor que elimine esa fruta por id y sala
        servidor.eliminarFruta(salaSeleccionada, fruta.getEntityId());

        // Opcional: limpiar selección local
        entidadSeleccionada = null;
        salaSeleccionada = null;

        confirmacion.setText("Fruta eliminada de sala " + salaSeleccionada);
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
        System.out.println("Servidor detenido1.");
    }
}