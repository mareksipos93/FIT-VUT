package cz.iwitrag.tpsmeter;

import org.bukkit.command.Command;
import org.bukkit.command.CommandExecutor;
import org.bukkit.command.CommandSender;

import java.io.IOException;

/** @author Marek Sipos (xsipos03) */
public class TpsMeterCommand implements CommandExecutor, TpsMeterListener {

    @Override
    public boolean onCommand(CommandSender sender, Command command, String label, String[] args) {

        if (args.length > 0) {
            if (args[0].equalsIgnoreCase("start")) {
                int interval = 1;
                int duration = -1;

                // parse interval
                if (args.length > 1) {
                    try {
                        interval = Integer.parseInt(args[1]);
                    } catch (NumberFormatException e) {
                        sender.sendMessage("§cZadany interval je neplatny");
                        return true;
                    }
                    if (interval < 1) {
                        sender.sendMessage("§cZadany interval je mensi nez 1");
                        return true;
                    }
                }

                // parse duration
                if (args.length > 2) {
                    try {
                        duration = Integer.parseInt(args[2]);
                    } catch (NumberFormatException e) {
                        sender.sendMessage("§cZadana delka je neplatna");
                        return true;
                    }
                    if (duration < 1) {
                        sender.sendMessage("§cZadana delka je mensi nez 1");
                        return true;
                    }
                }

                // start
                try {
                    if (Main.getMain().getTpsMeter().start(interval, duration)) {
                        TpsMeterHandler meter = Main.getMain().getTpsMeter();
                        if (meter.getDuration() == -1) {
                            sender.sendMessage("§aZacina mereni: kazdych §2" + meter.getInterval() + "s §aaz do vypnuti §ado souboru §2" + meter.getFilename());
                        } else {
                            sender.sendMessage("§aZacina mereni: kazdych §2" + meter.getInterval() + "s §apo dobu §2" + meter.getDuration() + "s §ado souboru §2" + meter.getFilename());
                        }
                        meter.addListener(this);
                        return true;
                    } else {
                        sender.sendMessage("§cMereni nelze zacit, prave probiha");
                        return true;
                    }
                } catch (IOException e) {
                    sender.sendMessage("§cNepodarilo se spustit mereni, chyba souboroveho systemu");
                }
            }

            if (args[0].equalsIgnoreCase("file")) {
                TpsMeterHandler meter = Main.getMain().getTpsMeter();
                if (args.length == 1) {
                    sender.sendMessage("§bMomentalne je nastaven soubor §3" + meter.getFilename());
                    return true;
                } else {
                    if (meter.setFilename(args[1])) {
                        sender.sendMessage("§aSoubor uspesne zmenen na §2" + args[1]);
                    } else {
                        sender.sendMessage("§cSoubor nelze nastavit, prave probiha mereni");
                    }
                    return true;
                }
            }

            if (args[0].equalsIgnoreCase("stop")) {
                TpsMeterHandler meter = Main.getMain().getTpsMeter();
                if (meter.stop()) {
                    sender.sendMessage("§aMereni bylo manualne dokonceno a ulozeno do souboru §2" + meter.getFilename());
                    meter.removeListener(this);
                    return true;
                } else {
                    sender.sendMessage("§cZadne mereni neprobiha");
                    return true;
                }
            }
        }

        sender.sendMessage("§b/" + label + " start §f- zacne mereni po sekundach az do vypnuti");
        sender.sendMessage("§b/" + label + " start interval §f- zacne mereni po INTERVAL sekundach az do vypnuti");
        sender.sendMessage("§b/" + label + " start interval delka §f- zacne mereni po INTERVAL sekundach po dobu DELKA sekund");
        sender.sendMessage("§b/" + label + " file soubor §f- nastavi soubor, kam se ma mereni ukladat");
        sender.sendMessage("§b/" + label + " stop §f- vypne probihajici mereni");

        return true;
    }

    @Override
    public void onFinish() {
        Main.getMain().getServer().broadcastMessage("§aMereni bylo dokonceno a ulozeno do souboru §2" + Main.getMain().getTpsMeter().getFilename());
        Main.getMain().getTpsMeter().removeListener(this);
    }
}
