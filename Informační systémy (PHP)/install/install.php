<?php

require_once('../settings.php');

mb_internal_encoding("UTF-8");
ini_set("default_charset", "UTF-8");

function connect($host, $user, $password, $db) {

    $settings = array(
        PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
        PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8",
        PDO::ATTR_EMULATE_PREPARES => false,
    );

    $pdo = null;
    try {
        $pdo = new PDO(
            "mysql:host=$host;dbname=$db",
            $user,
            $password,
            $settings
        );
    } catch (Exception $ex) {
        return false;
    }
    return $pdo;
}

$msg = '';
$show_form = true;

/** @noinspection PhpUndefinedVariableInspection */
$connection = connect($db['host'], $db['user'], $db['password'], $db['database']);
if ($connection === false) {
    $msg = 'Nepodařilo se navázat připojení s databází. Nejsou špatně údaje?';
    $show_form = false;
}

if (isset($_POST['install_type'])) {

    $sql = '';
    if ($_POST['install_type'] == 'empty')
        $sql = file_get_contents('empty_tables.sql');
    if ($_POST['install_type'] == 'filled')
        $sql = file_get_contents('full_tables.sql');

    if (!empty($sql)) {
        try {
            $connection->exec($sql);
            $show_form = false;
            $msg = 'Úspěšně provedeno!';
        } catch (PDOException $e) {
            $msg = 'Chyba při zpracování SQL skriptu!<br />' . $e->getMessage();
        }
    } else
        $msg = 'Nepodařilo se načíst databázový SQL soubor.';
}

?>
<!DOCTYPE html>
    <html>
    <head>
        <style>
            p {margin: 0 0 20px 0; display: inline-block; padding: 3px; border: black 1px solid;}
        </style>
        <meta charset="UTF-8" />
        <title>Instalační skript</title>
    </head>
    <body>

        <h2>Instalace databáze</h2>

<?php if (!empty($msg)): ?>
        <p>
            <?= $msg ?>
        </p>
<?php endif ?>

<?php if ($show_form): ?>
        <form method="POST" id="install">
            <label>
                <input type="radio" name="install_type" value="empty">
                Vytvořit pouze prázdné tabulky
            </label><br/>
            <label>
                <input type="radio" name="install_type" value="filled">
                Vytvořit tabulky naplněné ukázkovými daty
            </label><br/><br/>
            <input type="submit" name="submit" value="Odeslat"/>
        </form>
<?php endif ?>
    </body>
</html>
