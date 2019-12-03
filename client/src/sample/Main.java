package sample;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.io.*;
import java.net.Socket;

public class Main extends Application {

    @Override
    public void start(Stage primaryStage) throws Exception{
//        Parent root = FXMLLoader.load(getClass().getResource("sample.fxml"));
//        primaryStage.setTitle("Hello World");
//        primaryStage.setScene(new Scene(root, 300, 275));
//        primaryStage.show();

    }


    public static void main(String[] args) throws IOException {
//        launch(args);

        Socket socket = null;
        try {
            socket = new Socket("127.0.0.1", 3000);
        } catch (IOException e) {
            e.printStackTrace();
        }

        PrintWriter pr = null;
        try {
            assert socket != null;
            pr = new PrintWriter(socket.getOutputStream());
        } catch (IOException e) {
            e.printStackTrace();
        }
        assert pr != null;
        String str_in = "hello world";
        pr.println(str_in);
        pr.flush();

        InputStreamReader inputStreamReader = new InputStreamReader(socket.getInputStream());
        BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
        String str = bufferedReader.readLine();
        System.out.println(str);
    }
}
