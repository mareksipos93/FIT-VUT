package cz.iwitrag.tpsmeter;

import org.bukkit.plugin.java.JavaPlugin;

/** @author Marek Sipos (xsipos03) */
public class Main extends JavaPlugin {

    private static Main main;
    private TpsMeterHandler tpsMeter;

    public Main() {
        Main.main = this;
    }

    public static Main getMain() {
        return main;
    }

    @Override
    public void onEnable(){
        this.getCommand("tpsmeter").setExecutor(new TpsMeterCommand());

        this.tpsMeter = new TpsMeterHandler();
    }

    @Override
    public void onDisable(){
        tpsMeter.stop();
    }

    public TpsMeterHandler getTpsMeter() {
        return tpsMeter;
    }
}
