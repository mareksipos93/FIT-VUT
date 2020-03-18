<?php

require_once('settings.php');

////////////////////////////////

require_once('model/StringUtils.php');

function autoload($class) {
    if (StringUtils::endsWith($class, 'Controller', true))
        require("controller/" . $class . ".php");
    else if (StringUtils::endsWith($class, 'FFW', true))
        require("model/formframework/" . $class . ".php");
    else
        require("model/" . $class . ".php");
}

mb_internal_encoding("UTF-8");
ini_set("default_charset", "UTF-8");
spl_autoload_register("autoload");

session_start(array(
    'cookie_lifetime' => 1800, // 30 min
));

$_SESSION['base'] = $base;

                /* DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG */
                /* DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG */
                /* DEBUG DEBUG */    $_SESSION['debug_enabled'] = false;     /* DEBUG DEBUG */
                /* DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG */
                /* DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG */

if ($_SESSION['debug_enabled']) {
    set_error_handler(function($severity, $message, $file, $line) {
        if (error_reporting() & $severity) {
            throw new ErrorException($message, 0, $severity, $file, $line);
        }
    });
    error_reporting(E_ALL);
    ini_set('display_errors', 1);
} else {
    error_reporting(0);
    ini_set('display_errors', 0);
}

if (!Db::connect($db['host'], $db['user'], $db['password'], $db['database'])) {
    die(<<<'TAG'
<!DOCTYPE html>
<html lang="cs-cz">
    <head>
        <meta charset="UTF-8" />
        <title>Chyba</title>
    </head>
    <body>
        <p>
            Nepodařilo se navázat připojení s databází. Nejsou špatně údaje?<br />Pokud jsou údaje správné - byla provedena instalace skriptem?
        </p>
    </body>
</html>
TAG
);
}

$router = new RouterController();
$router->execute(array($_SERVER['REQUEST_URI']));

$router->printView();

