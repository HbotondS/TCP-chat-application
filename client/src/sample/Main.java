package sample;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import sample.login.LoginController;

public class Main extends Application {

    @Override
    public void start(Stage window) throws Exception{
        FXMLLoader loader = new FXMLLoader(getClass().getResource("login/LoginUI.fxml"));
        Parent root = loader.load();
        LoginController controller = loader.getController();
        controller.setStage(window);

        window.setTitle("TCP chat application");
        window.setScene(new Scene(root));
        window.show();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
