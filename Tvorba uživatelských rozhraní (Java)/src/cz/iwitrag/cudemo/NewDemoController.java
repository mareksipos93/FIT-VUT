package cz.iwitrag.cudemo;

import cz.iwitrag.cudemo.demonstration.Demonstration;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ListView;
import javafx.scene.control.TextArea;
import javafx.stage.Stage;

/**
 * Controller pro okno pro výběr typu nové demonstrace
 * Autor: Marek Šipoš (xsipos03)
 * ITU PROJEKT 2017, FIT VUT
 */
public class NewDemoController {

    public ListView<String> presetDemos;
    public TextArea demoDesc;
    public Button newDemoButton;

    // Slouží k uchování poslední vybrané demonstrace, pomocí CTRL lze totiž výběr jakoby odkliknout... :/
    private Demonstration.DemonstrationType lastSelectedDemonstration;

    private static NewDemoController controller;

    public NewDemoController() {
        NewDemoController.controller = this;
    }

    public static NewDemoController getController() {
        return controller;
    }

    @FXML
    private void initialize() {
        ObservableList<String> list = presetDemos.getItems();
        list.addAll(Demonstration.DemonstrationType.getNames());
    }

    @FXML
    public void handleMouseClick() {
        String selected = presetDemos.getSelectionModel().getSelectedItem();
        if (selected != null && !selected.isEmpty()) {
            lastSelectedDemonstration = Demonstration.DemonstrationType.getByName(selected);
            if (lastSelectedDemonstration != null) {
                demoDesc.setText(lastSelectedDemonstration.getDesc());
                newDemoButton.setDisable(false);
            }
        }
    }

    @FXML
    public void newDemoButtonClicked() {
        if (lastSelectedDemonstration != null) {
            ((Stage)newDemoButton.getScene().getWindow()).close();
            MainController.getController().createNewDemonstration(lastSelectedDemonstration);
        }
    }
}
