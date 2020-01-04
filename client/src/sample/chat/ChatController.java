package sample.chat;

import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
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
import java.util.Timer;
import java.util.TimerTask;

public class ChatController {
    public TextFlow textArea;
    public TextField msg;
    public Button sendBtn;
    public Label error;
    public Button refreshBtn;

    private Socket socket = null;
    private Timer timer = new Timer();

    private boolean isServerOnline = true;

    private String nickname;

    public void joinChat(String nickname) {
        this.nickname = nickname;
        try {
            connect(nickname);
        } catch (IOException e) {
            Alert errorAlert = new Alert(Alert.AlertType.ERROR);
            errorAlert.setHeaderText(null);
            errorAlert.setContentText("The server is not running");
            errorAlert.showAndWait();

            System.exit(0);
        }

        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                try {
                    if (isServerOnline) {
                        Platform.runLater(() -> {
                            error.setVisible(false);
                            refreshBtn.setVisible(false);
                        });
                    }
                    isServerOnline = true;
                    InputStreamReader inputStreamReader = new InputStreamReader(socket.getInputStream());
                    BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
                    processMsg(bufferedReader.readLine());
                } catch (IOException e) {
                    isServerOnline = false;
                    Platform.runLater(() -> {
                        error.setVisible(true);
                        refreshBtn.setVisible(true);
                    });
                }
            }
        }, 0, 1);
    }

    private void connect(String nickname) throws IOException {
        socket = new Socket("127.0.0.1", 3000);

        // send join message to server
        PrintWriter pr = new PrintWriter(socket.getOutputStream());
        pr.print("join|" + nickname);
        pr.flush();
    }

    public void setStageAndSetupListeners(Stage window) {
        window.setOnCloseRequest(e -> System.exit(0));
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

    public void reconnect(ActionEvent actionEvent) {
        System.out.println("refreshed");
        try {
            connect(this.nickname);
            isServerOnline = true;
        } catch (IOException e) {

        }
    }
}
