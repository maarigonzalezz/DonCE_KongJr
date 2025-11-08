module org.servidor.servidor {
    requires javafx.controls;
    requires javafx.fxml;
    requires com.fasterxml.jackson.databind;


    opens org.servidor.servidor to javafx.fxml;
    exports org.servidor.servidor;

    opens org.servidor.servidor.jsonparser to com.fasterxml.jackson.databind;
}