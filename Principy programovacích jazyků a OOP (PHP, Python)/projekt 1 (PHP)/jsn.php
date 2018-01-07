<?php
/* IPP projekt 1 - FIT VUT
 * Zadani: JSON2XML
 * Autor: Marek Sipos (xsipos03)
 * Datum: 15.3.2017
 */
mb_internal_encoding("UTF-8"); // Pracujeme v UTF-8

# -----------------------------------------------
# -------------  Globalni promenne  -------------
# -----------------------------------------------

/* Duvod vyuziti globalnich promennych je jednoduche reseni nepristupnosti promennych ve vlastnich funkcich (viz nize) */
/* V opacnem pripade bych musel pole predavat do kazde funkce jako parametr, v PHP se pole predavaji hodnotou, coz neni efektivni */
global $args_long;   // Pole s dlouhymi parametry --nazev
global $args_short;  // Retezec s kratkymi parametry -n
global $args_haspar; // Pole s parametry, ktere prijimaji nejakou hodnotu
global $args_found;  // Pole obsahujici nactene parametry pomoci getopt
global $arg;         // Pole se zpracovanymi parametry pro vyuziti v kodu

# -----------------------------------------------
# -------------  Nacteni parametru  -------------
# -----------------------------------------------

/* Popis parametru k vyhledani pomoci getopt, long zacinaji --, short zacinaji - */
$args_long = array(
    'help',        // Vytiskne napovedu na stdout a nic nedela. Parametr help musi byt zadan samostatne, jinak vyvola chybu
    'input',       // JSON vstupni soubor (UTF-8, RFC 4627), pokud parametr chybi, bere se stdin, lze zadat absolutne i relativne, pokud je nekde mezera, musi to byt obaleno uvozovkami
    'output',      // XML vystupni soubor (UTF-8), pokud parametr chybi, bere se stdout, pokud soubor jiz existuje, prepise se, jinak se vytvori novy. Dale je vse stejne jako u parametru input
    'array-name',  // Umoznuje prejmenovat element obalujici pole z array na neco jineho. Pokud je zadana nevalidni znacka, vyvola se chyba 50
    'item-name',   // Umoznuje prejmenovat element obalujici prvky pole z item na neco jineho. Pokud je zadana nevalidni znacka, vyvola se chyba 50
    'array-size',  // U pole bude doplnen atribut size s poctem prvku v poli
    'index-items', // Ke kazdemu prvku pole bude pridan atribut index s jeho poradim v poli (cislovani zacina implicitne na 1)
    'start',       // Zmena hodnoty implicitniho citace pro parametr index-items, musi to byt cele cislo v rozsahu >= 0, pokud neni zadan parametr index-items, vyvola se chyba 1
    'types'        // Obalujici elementy skalarnich hodnot budou doplneny o atribut type s datovym typem puvodni hodnoty (integer, real, string, literal)
);

$args_short = ''
    . 'h'  // V ODVOZENYCH jmenech elementu z dvojice jmeno-hodnota se nahradi nepovolene znaky timto retezcem (implicitne se nahrazuji znakem - ). Pokud vznikne nevalidni XML nazev, vyvola se chyba 51
    . 'n'  // Nevygeneruje se XML hlavicka
    . 'r'  // Jmeno paroveho korenoveho elementu. Pokud neni zadan, zadny obalujici element se nevytvori. Pokud je zadana nevalidni znacka, vyvola se chyba 50
    . 's'  // Hodnoty typu string se prevedou na textove elementy misto atributu
    . 'i'  // Hodnoty typu number se prevedou na textove elementy misto atributu
    . 'l'  // Hodnoty literalu (true, false, null) se prevedou na elementy (napr. <true /> ) misto atributu
    . 'c'  // Oproti implicitnimu chovani aktivuje preklad problematickych znaku (UTF-8 kod < 128 ) ve vstupnich retezcich a konvertuje je na zapis pomoci & (napr. &amp )
    . 'a'  // Synonymum pro array-size
    . 't'; // Synonymum pro index-items

/* Seznam parametru, ktere prijimaji nejakou hodnotu (pro pozdejsi kontrolu) */
$args_haspar = array(
    'input', 'output', 'h', 'r', 'array-name', 'item-name', 'start'
);


/* Vytvorime temp pole a retezec s pripojenymi :: za parametry, aby getopt propustilo parametr s i bez dodanych parametru */
$args_long_temp = $args_long;
array_walk($args_long_temp, function(&$value) { $value .= "::"; });
$args_short_temp = preg_split( '//u', $args_short, null, PREG_SPLIT_NO_EMPTY );
array_walk($args_short_temp, function(&$value) { $value .= "::"; });
$args_short_temp = implode($args_short_temp);
/* Samotne vyhledani a ulozeni parametru do pole, dale overeni zda nedoslo k chybe PHP */
$args_found = getopt($args_short_temp, $args_long_temp);

if (is_bool($args_found))
    chyba("Nastala systemova chyba pri nacitani parametru (getopt)", 100);

# --------------------------------------------------
# -------------  Zpracovani parametru  -------------
# --------------------------------------------------

$arg = array();

foreach ($args_found as $key => $value) {

    /* Osetreni vicenasobneho zadani tehoz parametru */
    if (is_array($value))
        chyba("Parametr " . (strlen(cely_nazev_parametru($key)) > 1 ? "--" : "-") . cely_nazev_parametru($key) . " byl zadan vicekrat", 1);

    /* Osetreni situaci, kdy hodnota parametru prebyva */
    if (!is_bool($value) && !in_array($key, $args_haspar, true))
        chyba("Parametr " . (strlen(cely_nazev_parametru($key)) > 1 ? "--" : "-") . cely_nazev_parametru($key) . " neprijima zadnou hodnotu", 1);

    /* Osetreni situaci, kdy hodnota parametru chybi */
    if (is_bool($value) && in_array($key, $args_haspar, true))
        chyba("Parametr " . (strlen(cely_nazev_parametru($key)) > 1 ? "--" : "-") . cely_nazev_parametru($key) . " ocekava hodnotu", 1);

    /* Nacteni zadanych hodnot */
    if (!is_bool($value))
        $arg[cely_nazev_parametru($key)] = $value;
    else
        $arg[cely_nazev_parametru($key)] = true;
}

/* Parametr napovedy nelze kombinovat s jinymi parametry */
if (array_key_exists("help", $arg) && sizeof($arg) > 1)
    chyba("Parametr --help musi byt zadan jako jediny", 1);

/* Parametr start lze pouzit jen s parametrem index-items */
if (array_key_exists("start", $arg) && !array_key_exists("index-items", $arg))
    chyba("Parametr --start nelze pouzit bez parametru --index-items", 1);

/* Parametr start musi byt celociselny a >= 0, takze nemuze mit tecku ani minus */
if (array_key_exists("start", $arg) && !ctype_digit($arg["start"]))
    chyba("Parametr --start musi byt cele nezaporne cislo", 1);

/* Neexistujici parametry */
if (sizeof($args_found)+1 != $argc)
    chyba("Nalezeny neexistujici parametry, zkuste zadat --help pro napovedu", 1);

/* Chybne nazvy v parametrech nahrazujicich nazvy elementu */
if (array_key_exists("r", $arg) && !validni_nazev_elementu($arg["r"]))
    chyba("Parametr -r obsahuje neplatnou hodnotu '" . $arg['r'] . "'", 50);
if (array_key_exists("array-name", $arg) && !validni_nazev_elementu($arg["array-name"]))
    chyba("Parametr --array-name obsahuje neplatnou hodnotu '" . $arg['array-name'] . "'", 50);
if (array_key_exists("item-name", $arg) && !validni_nazev_elementu($arg["item-name"]))
    chyba("Parametr --item-name obsahuje neplatnou hodnotu '" . $arg['item-name'] . "'", 50);

# ------------------------------------------
# -------------  Beh programu  -------------
# ------------------------------------------

/* Vytisknuti napovedy a ukonceni behu programu */
if (array_key_exists("help", $arg)) {
    tisk_napovedy();
    exit(0);
}

/* Nacteni zdrojoveho JSON souboru do retezce */
set_error_handler("input_chyba", E_WARNING);
$json_str = file_get_contents((array_key_exists("input", $arg) ? $arg["input"] : "php://stdin"));
restore_error_handler();

/* Dekodovani JSON retezce do PHP struktury */
$json = @json_decode($json_str, true);
if ($json === null && json_last_error() !== JSON_ERROR_NONE)
    chyba("Chyba formatu JSON souboru: " . json_last_error_msg(), 4);

if ($json === null) {
    chyba("Nebyl zadan JSON soubor nebo nebyl EOF na vlastnim radku", 100); // FIXME - co to vlastne zpusobuje? A nejake zotaveni? Je treba to vubec resit?
}

/* Zpracovani PHP struktury do XML retezce podle zadanych moznosti */
if (array_key_exists("r", $arg))
    $root = '<' . $arg["r"] . '/>';
else
    $root = '<deleteme/>';
$xml =  new SimpleXMLElement('<?xml version="1.0" encoding="UTF-8"?>' . $root);

/* Samotne rekurzivni zpracovani */
if (array_key_exists(0, $json))
    xml_zpracuj_pole($xml, $json); // Je to globalni pole
else
    xml_zpracuj_objekt($xml, $json); // Je to globalni objekt

/* Odsazeni */
$dom = dom_import_simplexml($xml);
$dom->ownerDocument->formatOutput = true;

/* Prevod na retezec */
$result = $dom->ownerDocument->saveXML();

/* Pripadne odstraneni hlavicky a root elementu, uprava odsazeni */
$exploded = explode("\n", $result);

$unindent = false;
$result = "";
for ($i = 0; $i < sizeof($exploded); $i++) {
    if ($i == 0 && array_key_exists("n", $arg))
        continue;
    if (($i == 1 || $i >= sizeof($exploded)-2) && !array_key_exists("r", $arg)) {
        $unindent = true;
        continue;
    }
    if ($unindent)
        $exploded[$i] = substr($exploded[$i], 2, strlen($exploded[$i]));

    $result .= $exploded[$i] . PHP_EOL;
}

/* Pripadny undo htmlspecialchars ktere provadi simpleXML sam od sebe */
if (!array_key_exists("c", $arg))
    $result = htmlspecialchars_decode($result); // FIXME - vubec se nekoduji uvozovky do &quot; (apostrofy se neresi, nejsou v JSON povolene vubec) jExam ukazuje identical O_o

/* Zapis vysledneho XML do souboru */
set_error_handler("output_chyba", E_WARNING);
$json_str = file_put_contents((array_key_exists("output", $arg) ? $arg["output"] : "php://stdout"), $result);
restore_error_handler();

/* Konec programu */

exit(0);

# --------------------------------------------
# -------------  Pomocne funkce  -------------
# --------------------------------------------

/**
 * Ukoncit beh skriptu s navratovym kodem a vypsat chybovou hlasku
 * @param string $zprava Chybova hlaska k vypsani na chybovy vystup
 * @param int $cislo_chyby Kod chyby pro ukonceni behu skriptu
 */
function chyba($zprava, $cislo_chyby) {
    if (!empty($zprava)) fwrite(STDERR, "CHYBA #$cislo_chyby: " . $zprava);
    exit($cislo_chyby);
}

/**
 * Tato funkce slouzi k rychlemu ziskani celeho nazvu parametru bez pomlcek
 * @param $param string Existujici cely nebo zkraceny nazev parametru bez pomlcek
 * @return string Cely nazev parametru bez pomlcek (pokud takovy neexistuje, vraci prazdny retezec)
 */
function cely_nazev_parametru($param) {
    global $args_long, $args_short;

    /* Synonyma */
    if (strcmp($param, "a") == 0)
        return "array-size";
    if (strcmp($param, "t") == 0)
        return "index-items";
    /* Dlouhe parametry se vrati */
    foreach($args_long as $value) {
        if (strcmp($param, $value) == 0)
            return $param;
    }
    /* Kratke parametry bez delsich verzi se vrati */
    $shorts = preg_split( '//u', $args_short, null, PREG_SPLIT_NO_EMPTY ); // Rozbiti retezce na jednotlive znaky, verze UTF-8
    foreach($shorts as $value) {
        if (strcmp($param, $value) == 0)
            return $param;
    }
    return ""; // Takovy parametr neexistuje
}

/**
 * Funkce, ktera zpracuje chybu pri otevirani vstupniho souboru
 * @param $errno int Uroven chyby PHP
 * @param $errstr string Zprava o chybe PHP
 */
function input_chyba($errno, $errstr) {
    chyba("Pri otevirani vstupniho souboru doslo k chybe: $errstr [$errno]", 2);
}

/**
 * Funkce, ktera zpracuje chybu pri otevirani vystupniho souboru
 * @param $errno int Uroven chyby PHP
 * @param $errstr string Zprava o chybe PHP
 */
function output_chyba($errno, $errstr) {
    chyba("Pri otevirani vystupniho souboru doslo k chybe: $errstr [$errno]", 3);
}

/**
 * Vytiskne napovedu na standardni vystup
 */
function tisk_napovedy() {
    echo "-------------------------------" . PHP_EOL;
    echo "JSON2XML - autor: Marek Sipos" . PHP_EOL;
    echo "Skript prevadi JSON format (RFC 4627) do XML." . PHP_EOL;
    echo "Parametry:" . PHP_EOL;
    echo "  --help\t\t\tTato napoveda" . PHP_EOL;
    echo "  --input=<nazev souboru>\tVstupni JSON soubor (UTF-8) (vynech pro stdin)" . PHP_EOL;
    echo "  --output=<nazev souboru>\tVystupni XML soubor (UTF-8) (vynech pro stdout)" . PHP_EOL;
    echo "  -h=<retezec>\t\t\tNahradit ve jmene odvozeneho elementu nepovolene znaky retezcem (vychozi pomlcka)" . PHP_EOL;
    echo "  -n\t\t\t\tNegenerovat XML hlavicku ve vystupnim souboru" . PHP_EOL;
    echo "  -r=<nazev korene>\t\tNazev paroveho korenoveho elementu obalujiciho vysledek" . PHP_EOL;
    echo "  --array-name=<novy nazev>\tNahrazeni implicitniho jmena tagu pole (array) na jine" . PHP_EOL;
    echo "  --item-name=<novy nazev>\tNahrazeni implicitniho jmena tagu prvku pole (item) na jine" . PHP_EOL;
    echo "  -s\t\t\t\tNahradit hodnoty typu string na textove elementy misto atributu" . PHP_EOL;
    echo "  -i\t\t\t\tNahradit hodnoty typu number na textove elementy misto atributu" . PHP_EOL;
    echo "  -l\t\t\t\tNahradit hodnoty literalu (true, false, null) na textovou reprezentaci (napr. <true/>) misto atributu" . PHP_EOL;
    echo "  -c\t\t\t\tKonvertovat problematicke znaky na reprezentaci pomoci metaznaku & (napr. &lt;)" . PHP_EOL;
    echo "  -a, --array-size\t\tDoplnit u poli atribut size s poctem prvku" . PHP_EOL;
    echo "  -t, --index-items\t\tDoplnit k prvkum pole atribut index s poradim (od 1, lze zmenit pomoci --start)" . PHP_EOL;
    echo "  --start=<cislo>\t\tKterym cislem zacina indexovani pomoci --index-items" . PHP_EOL;
    echo "  --types\t\t\tDoplnit obalujici elementy hodnot atributem type obsahujici typ obalene puvodni hodnoty prvku" . PHP_EOL;
    echo "-------------------------------" . PHP_EOL;
}

/**
 * Upravi nazev elementu podle parametru -h
 * @param $str string Nazev elementu k upraveni
 * @return mixed string Upraveny nazev elementu
 */
function upraveny_nazev_elementu($str) {
    global $arg;
    if (array_key_exists("h", $arg))
        $r = $arg["h"];
    else
        $r = "-";
    $str_old = $str;
    $str = str_replace(str_split('<>&"\'/'), $r, $str);
    if (!validni_nazev_elementu($str))
        chyba("Element '$str_old' neni ani po uprave na '$str' validni", 51);

    return $str;
}

/**
 * Overi nazev elementu na jeho validitu
 * @param $str string Nazev elementu k overeni
 * @return bool Zda je nazev validni
 */
function validni_nazev_elementu($str) {
    if (strlen($str) == 0) return false;
    if (!ctype_alpha($str[0]) && $str[0] != "_" && $str[0] != ":") return false;
    if (strpos($str, '<') !== false || strpos($str, '>') !== false || strpos($str, '&') !== false ||
        strpos($str, '"') !== false || strpos($str, '\'') !== false || strpos($str, '/') !== false ||
        strpos($str, ' ') !== false) return false;
    return true;
}

/**
 * Zpracuje JSON pole ve tvaru [ neco ]
 * @param $xml SimpleXMLElement XML koren ke zpracovani
 * @param $array array Pole s JSON daty ke zpracovani
 */
function xml_zpracuj_pole($xml, $array) {
    global $arg;
    if (array_key_exists("array-name", $arg))
        $child = $xml->addChild($arg["array-name"]);
    else
        $child = $xml->addChild("array");
    if (array_key_exists("array-size", $arg))
        $child->addAttribute("size", (string)sizeof($array));
    if (array_key_exists("start", $arg))
        $index = $arg["start"];
    else
        $index = 1;
    foreach($array as $key => $value) {
        if (array_key_exists("item-name", $arg))
            $item = $arg["item-name"];
        else
            $item = "item";
        if (!is_array($value))
            $newitem = xml_zpracuj_hodnotu($child, $item, $value);
        else if (array_key_exists(0, $value)) {
            $newitem = $child->addChild($item);
            xml_zpracuj_pole($newitem, $array[$key]);
        }
        else {
            $newitem = $child->addChild($item);
            xml_zpracuj_objekt($newitem, $array[$key]);
        }
        if (array_key_exists("index-items", $arg))
            $newitem->addAttribute("index", (string)$index);
        $index++;
    }
}

/**
 * Zpracuje JSON objekt ve tvaru { neco }
 * @param $xml SimpleXMLElement XML koren ke zpracovani
 * @param $array array Pole s JSON daty ke zpracovani
 */
function xml_zpracuj_objekt($xml, $array) {
    foreach($array as $key => $value) {
        if (!is_array($value))
            xml_zpracuj_hodnotu($xml, $key, $value);
        else if (array_key_exists(0, $value)) {
            $child = $xml->addChild(upraveny_nazev_elementu($key));
            xml_zpracuj_pole($child, $array[$key]);
        }
        else {
            $child = $xml->addChild(upraveny_nazev_elementu($key));
            xml_zpracuj_objekt($child, $array[$key]);
        }
    }
}

/**
 * Zpracuje JSON hodnotu typu integer, real, string, literal (true, false, null)
 * @param $xml SimpleXMLElement XML koren ke zpracovani
 * @param $key string Nazev elementu k pridani
 * @param $value mixed Hodnota elementu k pridani
 * @return SimpleXMLElement Vytvoreny XML element
 */
function xml_zpracuj_hodnotu($xml, $key, $value) {
    global $arg;
    $child = $xml->addChild(upraveny_nazev_elementu($key));
    if (array_key_exists("l", $arg)) {
        if ($value === true)
            $child->addChild("true");
        else if ($value === false)
            $child->addChild("false");
        else if ($value === null)
            $child->addChild("null");
    }
    else {
        if ($value === true)
            $child->addAttribute("value", "true");
        else if ($value === false)
            $child->addAttribute("value", "false");
        else if ($value === null)
            $child->addAttribute("value", "null");
        if (array_key_exists("types", $arg) && ($value === true || $value === false || $value === null))
            $child->addAttribute("type", "literal");
    }
    if (is_int($value) || is_float($value)) {
        if (array_key_exists("i", $arg))
            $child->{0} = (string)floor($value);
        else {
            $child->addAttribute("value", (string)floor($value));
            if (array_key_exists("types", $arg)) {
                if (is_int($value))
                    $child->addAttribute("type", "integer");
                else if (is_float($value))
                    $child->addAttribute("type", "real");
            }
        }
    }
    if (is_string($value)) {
        if (array_key_exists("s", $arg))
            $child->{0} = "$value";
        else {
            $child->addAttribute("value", "$value");
            if (array_key_exists("types", $arg)) {
                $child->addAttribute("type", "string");
            }
        }
    }
    return $child;
}
