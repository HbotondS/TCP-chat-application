package sample.login;

import com.sun.istack.internal.NotNull;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.control.TextField;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.AnchorPane;
import javafx.stage.Stage;
import sample.chat.ChatController;

import java.io.IOException;

public class LoginController {
    public TextField nickname;
    public AnchorPane ap;
    private Stage stage;

    public void join() throws IOException {
        if (!nickname.getText().isEmpty()) {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("../chat/ChatUI.fxml"));
            stage.setScene(new Scene(loader.load()));
            ChatController controller = loader.getController();
            controller.setStageAndSetupListeners(stage);
            controller.joinChat(nickname.getText());
        }
    }

    public void setStage(Stage window) {
        this.stage = window;
    }

    public void sendOnEnter(KeyEvent keyEvent) throws IOException {
        if (keyEvent.getCode() == KeyCode.ENTER) {
            join();
        }
    }
}
