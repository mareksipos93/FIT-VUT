package cz.iwitrag.cudemo.demonstration;

import javafx.scene.paint.Color;

import java.util.ArrayList;
import java.util.List;

/**
 * Představuje demonstraci, obsahuje objekty a parametry a je určitého vestavěného typu (<i>DemonstrationType</i>)
 * Autor: Marek Šipoš (xsipos03)
 * ITU PROJEKT 2017, FIT VUT
 */
public class Demonstration {

    /** Vestavěný typ demonstrace (typ křivky) */
    private DemonstrationType type;

    /** Seznam bodů obsažených v této simulaci */
    private List<Point> points = new ArrayList<>();


    public static Demonstration getDemonstration(DemonstrationType type, boolean isNew) {
        if (type == null) return null;

        Demonstration demonstration = new Demonstration();
        demonstration.type = type;

        if (isNew) {
            if (type == DemonstrationType.FERGUSON) {
                demonstration.points.add(new Point(50, 50));
                demonstration.points.add(new Point(100, 100));
                demonstration.points.add(new Point(150, 150));
            } else if (type == DemonstrationType.BSPLAJN) {
                demonstration.points.add(new Point(100, 160, Color.RED));
                demonstration.points.add(new Point(120, 170));
                demonstration.points.add(new Point(140, 150));
                demonstration.points.add(new Point(120, 130));
                demonstration.points.add(new Point(100, 110));
                demonstration.points.add(new Point(120, 90));
                demonstration.points.add(new Point(140, 100, Color.RED));
            } else if (type == DemonstrationType.NURBS) {
                demonstration.points.add(new Point(50, 50));
                demonstration.points.add(new Point(100, 50));
                demonstration.points.add(new Point(150, 50));
            }
        }
        return demonstration;
    }

    /**
     * Vrací typ demonstrace (typ křivky)
     */
    public DemonstrationType getType() {
        return type;
    }

    public List<Point> getPoints() {
        return new ArrayList<>(points);
    }

    public void removePoint(Point point) {
        points.remove(point);
    }

    public void addPoint(Point point) {
        if (!points.contains(point))
            points.add(point);
    }

    /**
     * Určuje vestavěný typ demonstrace (typ křivky)
     */
    public enum DemonstrationType {
        FERGUSON,
        BSPLAJN,
        NURBS;

        @Override
        public String toString() {
            if (this == FERGUSON) {
                return "Fergusonova křivka";
            } else if (this == BSPLAJN) {
                return "B-splajn";
            } else if (this == NURBS) {
                return "NURBS křivka";
            } else {
                return "Neznámá";
            }
        }

        public String getDesc() {
            if (this == FERGUSON) {
                return "Fergusonova křivka\n" +
                        "Nějaký popis Fergusonovy křivky";
            } else if (this == BSPLAJN) {
                return "B-splajn\n" +
                        "Nějaký popis B-splajnu";
            } else if (this == NURBS) {
                return "NURBS křivka\n" +
                        "Nějaký popis NURBS křivky";
            } else {
                return "Neznámý typ křivky\n" +
                        "Kde se to tu vzalo?";
            }
        }

        public static String[] getNames() {
            String[] names = new String[DemonstrationType.values().length];
            int i = 0;
            for (DemonstrationType type : DemonstrationType.values()) {
                names[i] = type.toString();
                i++;
            }
            return names;
        }

        public static DemonstrationType getByName(String name) {
            for (DemonstrationType type : DemonstrationType.values()) {
                if (type.toString().equalsIgnoreCase(name)) {
                    return type;
                }
            }
            return null;
        }
    }

    // TODO - parametry klíčových geometrií, interakce
}
