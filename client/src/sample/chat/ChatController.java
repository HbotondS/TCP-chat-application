package sample.chat;

import javafx.application.Platform;
import javafx.scene.control.*;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.paint.Color;
import javafx.scene.text.Text;
import javafx.scene.text.TextFlow;
import javafx.stage.Stage;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class ChatController {
    public TextFlow textArea;
    public TextField msg;
    public Button sendBtn;
    public Label error;

    private Socket socket = null;

    public void joinChat(String nickname) {
        try {
            socket = new Socket("127.0.0.1", 3000);

            // send join message to server
            PrintWriter pr = new PrintWriter(socket.getOutputStream());
            pr.print("join|" + nickname);
            pr.flush();
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

    private void processMsg(String msg) {
        String[] splittedMsg = msg.split("\\|");
        switch (splittedMsg[0]) {
            case "join": {
                Text text = new Text(splittedMsg[1] + " joined the chat.\n");
                text.setStyle("-fx-font-weight: bold");
                Platform.runLater(() -> textArea.getChildren().add(text));
                break;
            }
            case "leave": {
                Text text = new Text(splittedMsg[1] + " left the chat.\n");
                text.setStyle("-fx-font-weight: bold");
                text.setFill(Color.RED);
                Platform.runLater(() -> textArea.getChildren().add(text));
                break;
            }
            case "public": {
                Text text = new Text(splittedMsg[1] + ": " + splittedMsg[2] + "\n");
                Platform.runLater(() -> textArea.getChildren().add(text));
                break;
            }
            default: {
                Platform.runLater(() -> textArea.getChildren().add(new Text(msg + "\n")));
            }
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
                    processMsg(bufferedReader.readLine());
                } catch (IOException e) {
                    isServerOnline = false;
                    Platform.runLater(() -> error.setText("Server is offline"));
                }
            }
        }
    }
}
