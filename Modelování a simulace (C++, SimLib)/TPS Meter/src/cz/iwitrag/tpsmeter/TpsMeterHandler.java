package cz.iwitrag.tpsmeter;

import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/** @author Marek Sipos (xsipos03) */
public class TpsMeterHandler {

    private List<TpsMeterListener> listeners = new ArrayList<>();
    private FileWriter writer;

    private String filename;
    private boolean isWorking;
    private int interval;
    private int duration;

    private int elapsedTime;
    private long lastSystemTime;

    private int totalMeasurements;
    private double maxTps;
    private double minTps;
    private double avgTps;

    public TpsMeterHandler() {
        isWorking = false;
        filename = "measurement.txt";
        Main.getMain().getServer().getScheduler().scheduleSyncRepeatingTask(Main.getMain(), () -> {
            if (isWorking) {
                elapsedTime++;

                // Measure
                if (elapsedTime % interval == 0) {
                    double tps = 1000.0 / (((double)(System.currentTimeMillis() - lastSystemTime)) / 20.0);
                    if (tps > 20.0) tps = 20.0;
                    if (tps < 0.0) tps = 0.0;
                    lastSystemTime = System.currentTimeMillis();
                    if (totalMeasurements == 0) {
                        maxTps = tps;
                        minTps = tps;
                        avgTps = tps;
                    } else {
                        if (tps > maxTps) maxTps = tps;
                        if (tps < minTps) minTps = tps;
                        avgTps = ((avgTps * (double)totalMeasurements) + tps) / ((double)totalMeasurements + 1);
                    }
                    totalMeasurements++;

                    try {
                        writer.write(elapsedTime + ":\t" + tps + "\n");
                        writer.flush();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }

                // End
                if (elapsedTime == duration) {
                    List<TpsMeterListener> safeCopy = new ArrayList<>(listeners);
                    for (TpsMeterListener listener : safeCopy) {
                        listener.onFinish();
                    }

                    isWorking = false;
                    try {
                        writer.write("===== END DURATION (MEASUREMENTS:" + totalMeasurements + ", MAX:" + maxTps + ", MIN:" + minTps + ", AVG:" + avgTps + ", TOTALTIME:" + elapsedTime + ") =====\n");
                        writer.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    writer = null;
                }
            }

            lastSystemTime = System.currentTimeMillis();
        }, 0, 20);
    }

    private void resetStats() {
        elapsedTime = 0;
        totalMeasurements = 0;
        maxTps = -1;
        minTps = -1;
        avgTps = -1;
    }

    public boolean stop() {
        if (!isWorking) return false;

        isWorking = false;
        try {
            writer.write("===== END MANUAL (MEASUREMENTS:" + totalMeasurements + ", MAX:" + maxTps + ", MIN:" + minTps + ", AVG:" + avgTps + ", TOTALTIME:" + elapsedTime + ") =====\n");
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        writer = null;
        return true;
    }


    public boolean start(int interval, int duration) throws IOException {
        if (isWorking) return false;

        resetStats();

        this.interval = interval;
        this.duration = duration;
        isWorking = true;

        writer = new FileWriter(filename, true);

        try {
            writer.write("===== START (INTERVAL:" + interval + ", DURATION:" + duration + ") =====\n");
            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return true;
    }

    public String getFilename() {
        return filename;
    }

    public boolean setFilename(String filename) {
        if (isWorking) return false;

        this.filename = filename;
        return true;
    }

    public int getInterval() {
        return interval;
    }

    public int getDuration() {
        return duration;
    }

    public void addListener(TpsMeterListener listener) {
        listeners.add(listener);
    }

    public void removeListener(TpsMeterListener listener) {
        listeners.remove(listener);
    }
}
