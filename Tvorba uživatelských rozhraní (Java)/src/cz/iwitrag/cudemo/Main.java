package cz.iwitrag.cudemo;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.stage.Stage;

/**
 * Vstupní bod aplikace
 * Autor: Marek Šipoš (xsipos03)
 * ITU PROJEKT 2017, FIT VUT
 */
public class Main extends Application {

    public static final int INIT_WIDTH = 500;
    public static final int INIT_HEIGHT = 300;
    public static final int MAX_DELKA_NAZVU_KARTY = 20;

    private static Stage stage;

    public static Stage getStage() {
        return stage;
    }

    @Override
    public void start(Stage stage) throws Exception {
        Main.stage = stage;
        stage.setTitle("CUDEMO");
        stage.setScene(new Scene(FXMLLoader.load(getClass().getResource("main.fxml"))));
        stage.show();
        stage.setMinWidth(INIT_WIDTH);
        stage.setMinHeight(INIT_HEIGHT);
        MainController.getController().registerShortcuts();
    }

    public static void main(String[] args) {
        launch(args);
    }

}
