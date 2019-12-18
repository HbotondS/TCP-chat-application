package sample;

import javafx.application.Platform;
import javafx.fxml.Initializable;
import javafx.scene.control.*;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.GridPane;
import javafx.stage.Stage;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.URL;
import java.util.ResourceBundle;

public class Controller implements Initializable {
    public TextArea textArea;
    public TextField msg;
    public Button sendBtn;
    public Label error;

    private Socket socket = null;

    private boolean isConnected  = false;

    @Override
    public void initialize(URL location, ResourceBundle resources) {
        try {
            socket = new Socket("127.0.0.1", 3000);
        } catch (IOException e) {
            Alert errorAlert = new Alert(Alert.AlertType.ERROR);
            errorAlert.setHeaderText(null);
            errorAlert.setContentText("The server is not running");
            errorAlert.showAndWait();

            System.exit(0);
        }

        Thread thread = new Thread(new RecvThread());
        thread.start();
    }

    public void setStageAndSetupListeners(Stage window) {
        window.setOnCloseRequest(e -> {
            System.exit(0);
        });
    }

    public void sendMsg() {
        try {
            PrintWriter pr = new PrintWriter(socket.getOutputStream());
            String str_in = msg.getText();
            if (!str_in.equals("")) {
                pr.println(str_in);
                pr.flush();

                msg.clear();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void sendOnEnter(KeyEvent keyEvent) {
        if (keyEvent.getCode() == KeyCode.ENTER) {
            sendMsg();
        }
    }

    private class RecvThread implements Runnable {

        private boolean isServerOnline = true;

        @Override
        public void run() {
            while (true) {
                try {
                    if (isServerOnline) {
                        Platform.runLater(() -> error.setText(""));
                    }
                    isServerOnline = true;
                    InputStreamReader inputStreamReader = new InputStreamReader(socket.getInputStream());
                    BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
                    String msg = bufferedReader.readLine();
                    textArea.appendText(msg + "\n");
                } catch (IOException e) {
                    isServerOnline = false;
                    isConnected = false;
                    Platform.runLater(() -> error.setText("Server is offline"));
                }
            }
        }
    }
}
