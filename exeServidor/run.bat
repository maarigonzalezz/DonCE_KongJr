@echo off
set DIR=%~dp0


set "JAVA_HOME=C:\Program Files\Java\jdk-21"


set "PATH=%DIR%bin;%PATH%"

"%JAVA_HOME%\bin\java" ^
  --module-path "%DIR%lib" ^
  --add-modules javafx.controls,javafx.fxml,javafx.graphics ^
  -jar "%DIR%Servidor.jar"

pause
