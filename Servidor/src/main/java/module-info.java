module org.servidor.servidor {
    requires javafx.controls;
    requires javafx.fxml;


    opens org.servidor.servidor to javafx.fxml;
    exports org.servidor.servidor;
}