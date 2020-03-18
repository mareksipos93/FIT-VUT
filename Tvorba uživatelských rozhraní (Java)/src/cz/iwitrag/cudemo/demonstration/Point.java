package cz.iwitrag.cudemo.demonstration;

import javafx.scene.paint.Color;

/**
 * Představuje bod v demonstraci
 * Autor: Marek Šipoš (xsipos03)
 * ITU PROJEKT 2017, FIT VUT
 */
public class Point extends KeyGeometry {

    private Color color;

    public Point(double x, double y) {
        this.x = x;
        this.y = y;
        this.color = new Color(0.2, 0.3, 0.4, 1.0);
    }

    public Point(double x, double y, Color color) {
        this.x = x;
        this.y = y;
        this.color = color;
    }

    public Color getColor() {
        return color;
    }

    public void setColor(Color color) {
        this.color = color;
    }

}
