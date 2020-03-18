package cz.iwitrag.cudemo;

import cz.iwitrag.cudemo.demonstration.Demonstration;
import cz.iwitrag.cudemo.demonstration.KeyGeometry;
import cz.iwitrag.cudemo.demonstration.Point;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.collections.ObservableMap;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.stage.FileChooser;
import javafx.stage.Modality;
import javafx.stage.Stage;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.*;

/**
 * Controller pro hlavní okno aplikace
 * Autor: Marek Šipoš (xsipos03)
 * ITU PROJEKT 2017, FIT VUT
 */
public class MainController {

    public TabPane mainMenu;
    public TabPane demonstrations;
    public Tab emptyDemonstrationTab;
    public Tab optionsTab;
    public Tab demoOptionsTab;
    public Button newButton;
    public Button loadButton;
    public Button saveButton;
    public Button saveAsButton;
    public Label demoOptionsLabel;

    private List<OpenedDemo> openedDemos = new ArrayList<>();

    private static MainController controller;

    public MainController() {
        MainController.controller = this;
    }

    public static MainController getController() {
        return controller;
    }

    @FXML
    private void initialize() {
        // Kartu s možnostmi demonstrace na začátku nechceme
        mainMenu.getTabs().remove(demoOptionsTab);

        // Změna velikosti okna (překreslí aktivní kartu)
        ChangeListener<Number> stageSizeListener = (observable, oldValue, newValue) -> {
            if (!openedDemos.isEmpty()) {
                Tab selectedTab = demonstrations.getSelectionModel().getSelectedItem();
                for (OpenedDemo itOpenedDemo : openedDemos) {
                    if (itOpenedDemo.getDemoTab().equals(selectedTab)) {
                        itOpenedDemo.repaint();
                        break;
                    }
                }
            }
        };
        Main.getStage().widthProperty().addListener(stageSizeListener);
        Main.getStage().heightProperty().addListener(stageSizeListener);

        // Přepínání karet s demonstracemi
        demonstrations.getSelectionModel().selectedItemProperty().addListener((ov, oldTab, newTab) -> {
            if (oldTab != newTab) {
                for (OpenedDemo itOpenedDemo : openedDemos) {
                    if (itOpenedDemo.getDemoTab().equals(newTab)) {
                        itOpenedDemo.repaint();
                        demoOptionsLabel.setText("Možnosti demonstrace\n" + itOpenedDemo.getDemoTabName());
                    }
                }
            }
        });
    }

    public void registerShortcuts() {
        // Přiřazení klávesových zkratek ke tlačítkům, volá se z Mainu po inicializaci scény
        ObservableMap<KeyCombination, Runnable> acc = Main.getStage().getScene().getAccelerators();
        acc.put(new KeyCodeCombination(KeyCode.N, KeyCombination.CONTROL_DOWN), this::newButtonClicked);
        acc.put(new KeyCodeCombination(KeyCode.L, KeyCombination.CONTROL_DOWN), this::loadButtonClicked);
        acc.put(new KeyCodeCombination(KeyCode.S, KeyCombination.CONTROL_DOWN), this::saveButtonClicked);
    }

    @FXML
    private void newButtonClicked() {
        // Otevře okno pro výběr demonstrace
        Parent root;
        try {
            root = FXMLLoader.load(getClass().getResource("newdemo.fxml"));
            Stage stage = new Stage();
            stage.setTitle("Nová demonstrace");
            stage.setScene(new Scene(root, 400, 250));
            stage.setResizable(false);
            stage.initModality(Modality.WINDOW_MODAL);
            stage.initOwner(Main.getStage());
            stage.show();
        }
        catch (IOException e) {
            e.printStackTrace();
        }
    }

    @FXML
    private void loadButtonClicked() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Otevřít soubor s demonstrací");
        fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("JSON demonstrace (*.json)", "*.json"));
        File selectedFile = fileChooser.showOpenDialog(Main.getStage());
        if (selectedFile != null) {
            loadNewDemonstration(selectedFile);
        }
    }

    @FXML
    private void saveButtonClicked() {
        Tab selectedTab = demonstrations.getSelectionModel().getSelectedItem();
        for (OpenedDemo itOpenedDemo : openedDemos) {
            if (itOpenedDemo.getDemoTab().equals(selectedTab)) {
                itOpenedDemo.save();
                break;
            }
        }
    }

    // FIXME - pri uzavreni programu se musi zeptat na ulozeni vsech neulozenych demonstraci

    @FXML
    private void saveAsButtonClicked() {
        Tab selectedTab = demonstrations.getSelectionModel().getSelectedItem();
        for (OpenedDemo itOpenedDemo : openedDemos) {
            if (itOpenedDemo.getDemoTab().equals(selectedTab)) {
                itOpenedDemo.saveAs();
                break;
            }
        }
    }

    public List<OpenedDemo> getOpenedDemos() {
        return new ArrayList<>(openedDemos);
    }

    // TODO - případná nastavení demonstrace nebo programu?

    public void createNewDemonstration(Demonstration.DemonstrationType demonstrationType) {

        // Tvorba nové karty pro demo
        Tab newDemoTab = new Tab();
        // Název karty
        String tabName = "*" + demonstrationType.toString();
        if (tabName.length() > Main.MAX_DELKA_NAZVU_KARTY) {
            tabName = tabName.substring(0, Main.MAX_DELKA_NAZVU_KARTY);
        }
        newDemoTab.setText(tabName);
        // AnchorPane a vnitřní Pane
        AnchorPane newAnchorPane = new AnchorPane();
        newAnchorPane.setStyle("-fx-background-color: #6666ff;");
        Pane newPane = new Pane();
        newAnchorPane.setStyle("-fx-background-color: #fff;");
        newAnchorPane.getChildren().add(newPane);
        AnchorPane.setTopAnchor(newPane, 0.0);
        AnchorPane.setBottomAnchor(newPane, 0.0);
        AnchorPane.setLeftAnchor(newPane, 0.0);
        AnchorPane.setRightAnchor(newPane, 0.0);
        newDemoTab.setContent(newAnchorPane);

        // Případné otevření a nastavení karty pro nastavení demonstrace
        if (mainMenu.getTabs().size() < 2) {
            mainMenu.getTabs().add(demoOptionsTab);
            demoOptionsLabel.setText("Možnosti demonstrace\n" + tabName.substring(1));
        }

        // Tvorba nové demonstrace
        Demonstration newDemo = Demonstration.getDemonstration(demonstrationType, true);
        OpenedDemo newOpenedDemo = new OpenedDemo(false);
        newOpenedDemo.setDemoTab(newDemoTab);
        newOpenedDemo.setDemo(newDemo);
        openedDemos.add(newOpenedDemo);
        demonstrations.getTabs().remove(emptyDemonstrationTab);
        demonstrations.getTabs().add(newDemoTab);
        demonstrations.getSelectionModel().select(newDemoTab);
        saveButton.setDisable(false);
        saveAsButton.setDisable(false);

        // Chování při zavření karty
        setTabOnCloseBehavior(newDemoTab);

        // Vykreslení prvků UI
        newOpenedDemo.repaint();
    }

    public void loadNewDemonstration(File file) {

        // Naparsování souboru
        JSONObject jsonObject;
        try {
            jsonObject = (JSONObject)new JSONParser().parse(new FileReader(file));
        } catch (IOException|ParseException e) {
            e.printStackTrace();
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("Chyba");
            alert.setHeaderText("Chyba při otevírání demonstrace");
            alert.setContentText("Při otevírání demonstrace '" + file.getName() + "' došlo k chybě!\n" + e.getLocalizedMessage());
            alert.showAndWait();
            return;
        }

        // Naparsování typu demonstrace
        String type = (String)jsonObject.get("type");
        if (type == null) {
            showDamaged(file);
            return;
        }
        Demonstration.DemonstrationType demonstrationType;
        try {
            demonstrationType = Demonstration.DemonstrationType.valueOf(type);
        } catch (IllegalArgumentException e) {
            showDamaged(file);
            return;
        }

        List<Point> points = new ArrayList<>();
        JSONArray pointsArray = (JSONArray)jsonObject.get("points");
        if (pointsArray != null) {
            for (Object o : pointsArray) {
                JSONObject jsonPoint = (JSONObject) o;
                // Souřadnice bodu, pokud jsou neplatné, tak bod přeskočíme
                Object x = jsonPoint.get("x");
                Object y = jsonPoint.get("y");
                if (x == null || !(x instanceof Double) || y == null || !(y instanceof Double)) {
                    continue;
                }
                // Barva bodu, pokud je neplatná, použije se výchozí
                Color color;
                JSONArray jsonColor = (JSONArray)jsonPoint.get("color");
                if (jsonColor == null || jsonColor.size() != 3) {
                    color = new Color(0.2, 0.3, 0.4, 1.0);
                } else {
                    Object r = jsonColor.get(0);
                    Object g = jsonColor.get(1);
                    Object b = jsonColor.get(2);
                    if (!(r instanceof Double) || !(g instanceof Double) || !(b instanceof Double)) {
                        color = new Color(0.2, 0.3, 0.4, 1.0);
                    } else {
                        color = new Color((double)r, (double)g, (double)b, 1.0);
                    }
                }
                // Tvorba bodu
                points.add(new Point((double)x, (double)y, color));
            }
        }


        // Tvorba nové karty pro demo
        Tab newDemoTab = new Tab();
        // Název karty
        String tabName = file.getName();
        if (tabName.length() > Main.MAX_DELKA_NAZVU_KARTY) {
            tabName = tabName.substring(0, Main.MAX_DELKA_NAZVU_KARTY);
        }
        newDemoTab.setText(tabName);
        // AnchorPane a vnitřní Pane
        AnchorPane newAnchorPane = new AnchorPane();
        newAnchorPane.setStyle("-fx-background-color: #6666ff;");
        Pane newPane = new Pane();
        newAnchorPane.setStyle("-fx-background-color: #fff;");
        newAnchorPane.getChildren().add(newPane);
        AnchorPane.setTopAnchor(newPane, 0.0);
        AnchorPane.setBottomAnchor(newPane, 0.0);
        AnchorPane.setLeftAnchor(newPane, 0.0);
        AnchorPane.setRightAnchor(newPane, 0.0);
        newDemoTab.setContent(newAnchorPane);

        // Případné otevření a nastavení karty pro nastavení demonstrace
        if (mainMenu.getTabs().size() < 2) {
            mainMenu.getTabs().add(demoOptionsTab);
            demoOptionsLabel.setText("Možnosti demonstrace\n" + tabName);
        }

        // Tvorba nové demonstrace
        Demonstration newDemo = Demonstration.getDemonstration(demonstrationType, true);
        OpenedDemo newOpenedDemo = new OpenedDemo(true);
        newOpenedDemo.setDemoTab(newDemoTab);
        newOpenedDemo.setDemo(newDemo);
        newOpenedDemo.setFile(file);
        openedDemos.add(newOpenedDemo);
        demonstrations.getTabs().remove(emptyDemonstrationTab);
        demonstrations.getTabs().add(newDemoTab);
        demonstrations.getSelectionModel().select(newDemoTab);
        saveButton.setDisable(false);
        saveAsButton.setDisable(false);

        // Načtení prvků demonstrace
        for (Point point : points) {
            newDemo.addPoint(point);
        }

        // Chování při zavření karty
        setTabOnCloseBehavior(newDemoTab);

        // Vykreslení prvků UI
        newOpenedDemo.repaint();
    }

    private void showDamaged(File file) {
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle("Chyba");
        alert.setHeaderText("Chyba při otevírání demonstrace");
        alert.setContentText("Při otevírání demonstrace '" + file.getName() + "' došlo k chybě!\n" + "Soubor je poškozen");
        alert.showAndWait();
    }

    public void closeOpenedDemo(OpenedDemo demo) {
        if (openedDemos.contains(demo)) {
            openedDemos.remove(demo);
            demonstrations.getTabs().remove(demo.getDemoTab());
            if (demonstrations.getTabs().isEmpty()) {
                saveButton.setDisable(true);
                saveAsButton.setDisable(true);
                demonstrations.getTabs().add(emptyDemonstrationTab);
                for (Tab itTab : mainMenu.getTabs()) {
                    if (itTab.equals(demoOptionsTab)) {
                        mainMenu.getTabs().remove(itTab);
                        break;
                    }
                }
            }
        }
    }

    private void setTabOnCloseBehavior(Tab tab) {
        tab.setOnCloseRequest(event -> {
            for (OpenedDemo itDemo : openedDemos) {
                if (itDemo.getDemoTab().equals(tab)) {
                    if (!itDemo.isSaved()) {
                        Alert alert = new Alert(Alert.AlertType.WARNING);
                        alert.setTitle("Neuložená demonstrace");
                        alert.setHeaderText("Demonstrace '" + itDemo.getDemoTab().getText().substring(1) + "' není uložena!");
                        alert.setContentText("Opravdu chcete demonstraci zavřít?\nNeuložené změny budou ztraceny!");

                        ButtonType buttonTypeClose = new ButtonType("Zavřít");
                        ButtonType buttonTypeCancel = new ButtonType("Storno");

                        alert.getButtonTypes().setAll(buttonTypeClose, buttonTypeCancel);

                        Optional<ButtonType> result = alert.showAndWait();
                        if (result.get() == buttonTypeClose) {
                            MainController.getController().closeOpenedDemo(itDemo);
                        }
                    } else {
                        MainController.getController().closeOpenedDemo(itDemo);
                    }
                    // Nechceme, aby se karta zavřela sama... o to se postará Controller sám
                    event.consume();
                    break;
                }
            }
        });
    }

    /**
     * Představuje spojení mezi logikou demonstrace, UI a souborem na disku
     */
    private class OpenedDemo {

        private Demonstration demo;
        private Tab demoTab;
        private File file;
        private boolean saved;
        private Map<Node, KeyGeometry> pairs = new HashMap<>();
        private KeyGeometry selectedGeometry;

        public OpenedDemo(boolean saved) {
            this.saved = saved;
        }

        public Demonstration getDemo() {
            return demo;
        }

        public Tab getDemoTab() {
            return demoTab;
        }

        public String getDemoTabName() {
            if (saved)
                return demoTab.getText();
            else
                return demoTab.getText().substring(1);
        }

        public File getFile() {
            return file;
        }

        public void setFile(File file) {
            this.file = file;
        }

        public void setDemoTab(Tab demoTab) {
            this.demoTab = demoTab;
        }

        public void setDemo(Demonstration demo) {
            this.demo = demo;
        }

        public boolean isSaved() {
            return saved;
        }

        public void addPair(Node node, KeyGeometry keyGeometry) {
            pairs.put(node, keyGeometry);
        }

        public void removePair(Node node) {
            pairs.remove(node);
        }

        public void removePair(KeyGeometry keyGeometry) {
            pairs.entrySet().removeIf(entry -> entry.getValue().equals(keyGeometry));
        }

        public HashMap<Node, KeyGeometry> getPairs() {
            return new HashMap<>(pairs);
        }

        public KeyGeometry getSelectedGeometry() {
            return selectedGeometry;
        }

        public void setSelectedGeometry(KeyGeometry selectedGeometry) {
            this.selectedGeometry = selectedGeometry;
        }

        public void save() {
            if (file != null) {

                // Uložení do JSON
                JSONObject jsonObject = new JSONObject();
                jsonObject.put("type", demo.getType().name());
                JSONArray jsonPointArray = new JSONArray();
                for (Point itPoint : demo.getPoints()) {
                    JSONObject jsonPoint = new JSONObject();
                    jsonPoint.put("x", itPoint.getX());
                    jsonPoint.put("y", itPoint.getY());
                    JSONArray jsonColorArray = new JSONArray();
                    jsonColorArray.add(itPoint.getColor().getRed());
                    jsonColorArray.add(itPoint.getColor().getGreen());
                    jsonColorArray.add(itPoint.getColor().getBlue());
                    jsonPoint.put("color", jsonColorArray);
                    jsonPointArray.add(jsonPoint);
                }
                jsonObject.put("points", jsonPointArray);

                try (FileWriter fileWriter = new FileWriter(file)) {
                    fileWriter.write(jsonObject.toJSONString());
                    fileWriter.flush();
                } catch (IOException e) {
                    e.printStackTrace();
                    Alert alert = new Alert(Alert.AlertType.ERROR);
                    alert.setTitle("Chyba");
                    alert.setHeaderText("Chyba při ukládání demonstrace");
                    alert.setContentText("Při ukládání demonstrace '" + getDemoTabName() + "' došlo k chybě!\n" + e.getLocalizedMessage());
                    alert.showAndWait();
                    return;
                }

                saved = true;
                String tabName = file.getName();
                if (tabName.length() > Main.MAX_DELKA_NAZVU_KARTY) {
                    tabName = tabName.substring(0, Main.MAX_DELKA_NAZVU_KARTY);
                }
                demoTab.setText(tabName);
                demoOptionsLabel.setText("Možnosti demonstrace\n" + tabName);

            } else saveAs();
        }

        public void saveAs() {
            FileChooser fileChooser = new FileChooser();
            fileChooser.setTitle("Uložit demonstraci jako");
            fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("JSON demonstrace (*.json)", "*.json"));
            File selectedFile = fileChooser.showSaveDialog(Main.getStage());
            if (selectedFile != null) {
                file = selectedFile;
                save();
            }
        }

        /**
         * Smaže veškeré UI a vykreslí ho na kartu znovu
         */
        public void repaint() {
                Pane contentPane = (Pane)((AnchorPane)demoTab.getContent()).getChildren().get(0);
                Runnable runnable = new Runnable() {
                    @Override
                    public void run() {
                        if (contentPane.getWidth() == 0.0) {
                            Platform.runLater(this);
                        } else {
                            // Smazat veškeré UI
                            pairs.clear();
                            contentPane.getChildren().clear();
                            // Vytvořit UI znovu a spárovat
                            for (Point itPoint : demo.getPoints()) {
                                Rectangle newRect = new Rectangle();
                                newRect.setStroke(Color.TRANSPARENT);
                                newRect.setFill(itPoint.getColor());

                                double newSize = Math.sqrt(contentPane.getWidth()*contentPane.getHeight())/80;
                                newRect.setWidth(newSize);
                                newRect.setHeight(newSize);
                                newRect.setX(contentPane.getWidth()/(double)Main.INIT_WIDTH*itPoint.getX());
                                newRect.setY(contentPane.getHeight()/(double)Main.INIT_HEIGHT*itPoint.getY());

                                contentPane.getChildren().add(newRect);
                                pairs.put(newRect, itPoint);
                            }
                        }
                    }
                };
                Platform.runLater(runnable);
        }
    }
}

