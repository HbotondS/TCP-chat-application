package sample;

import javafx.event.ActionEvent;
import javafx.event.EventType;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.URL;
import java.util.ResourceBundle;

public class Controller implements Initializable {
    public TextArea textArea;
    public TextField msg;
    public Button sendBtn;

    Socket socket = null;

    @Override
    public void initialize(URL location, ResourceBundle resources) {
        try {
            socket = new Socket("127.0.0.1", 3000);
        } catch (IOException e) {
            e.printStackTrace();
        }
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
}
