<?php

class RouterController extends AbstractController
{
    /** @var AbstractController */
    protected $controller;

    /**
     * Executes routing mechanism.
     * @param $params array First param is URL address to process
     */
    public function execute($params) {
        if (!isset($_SESSION['redirects'])) {
            $_SESSION['redirects'] = array();
        }

        $this->data['base'] = $_SESSION['base'];

        // Parse URL
        $URLParts = $this->parseURL($params[0]);
        // URL is only host
        if (empty($URLParts[0]))
            $this->redirect("intro");

        // Find Controller
        $controllerName = StringUtils::hyphensToCamel($URLParts[0], false) . "Controller";
        if (file_exists('controller/' . $controllerName . '.php'))
            $this->controller = new $controllerName;
        else {
            $this->redirect('not-found');
        }

        // Execute Controller
        $URLPartsForController = $URLParts;
        array_shift($URLPartsForController);
        $this->controller->execute($URLPartsForController);

        // Take header data from child controller for layout
        foreach($this->controller->header as $key => $value) {
            $this->data[$key] = $value;
        }

        // Extend title if needed
        if (strlen($this->data['title']) > 0)
            $this->data['title'] .= ' | ';

        // render messages
        $this->data['messages'] = $this->getMessages();

        // right upper status panel
        $this->data['logged'] = AuthManager::isLoggedIn();
        if (AuthManager::isLoggedIn()) {
            // role
            switch ($_SESSION['auth']['role']) {
                case 'CLIENT':
                    $this->data['role'] = 'Klient';
                    break;
                case 'EMPLOYEE':
                    $this->data['role'] = 'Zaměstnanec';
                    break;
                case 'ADMIN':
                    $this->data['role'] = 'Administrátor';
                    break;
                default:
                    $this->data['role'] = 'Uživatel';
                    break;
            }
            // name
            $this->data['fname'] = $_SESSION['auth']['first_name'];
            $this->data['sname'] = $_SESSION['auth']['second_name'];
            // notifications
            $builder = new HtmlBuilder();
            $notifications = NotificationManager::getAmountOfUnreadNotifications($_SESSION['auth']['name']);
            $builder->startElement('div', array('class' => 'notif_count' . ($notifications > 0 ? ' notif_red' : '')));
            $builder->addValue($notifications);
            $builder->endElement();
            $this->data['notifications'] = $builder->render();
        }

        // main upper menu
        $this->data['main_menu'] = array();
        if (AuthManager::isLoggedIn()) {
            // CLIENT
            if ($_SESSION['auth']['role'] == 'CLIENT') {
                $this->data['main_menu'][] = $this->getMainMenuItem($URLParts, 'ib-client', 'Domů', 'Úvodní strana');
                $accounts = AccountManager::getAllActiveUsersBankAccounts($_SESSION['auth']['name']);
                if ($accounts) {
                    foreach ($accounts as $acc) {
                        $this->data['main_menu'][] = $this->getMainMenuItem($URLParts, 'bank-account/'.$acc['number'],
                            is_null($acc['custom_name']) ? $acc['name'] : $acc['custom_name'], StringUtils::formatMoney($acc['funds']) . ' Kč'
                        );
                    }
                }
                $this->data['main_menu'][] = $this->getMainMenuItem($URLParts, 'new-account', 'Nový účet', 'Založte si nový účet');
            }
            // EMPLOYEE & ADMIN
            if ($_SESSION['auth']['role'] == 'EMPLOYEE' || $_SESSION['auth']['role'] == 'ADMIN') {
                $this->data['main_menu'][] = $this->getMainMenuItem($URLParts, 'ib-privileged', 'Panel', 'Hlavní panel');
                $this->data['main_menu'][] = $this->getMainMenuItem($URLParts, 'pending-requests', 'Žádosti', 'Čekající žádosti');
                $this->data['main_menu'][] = $this->getMainMenuItem($URLParts, 'send-message', 'Zpráva', 'Zaslat zprávu');
            }
            // ADMIN
            // TODO - správa zaměstnanců a poboček adminem
        }

        // sub upper menu
        $this->data['sub_menu'] = array();
        if (AuthManager::isLoggedIn()) {
            // CLIENT
            if ($_SESSION['auth']['role'] == 'CLIENT') {
                if (StringUtils::hyphensToCamel($URLParts[0]) == 'bankAccount') {
                    $this->data['sub_menu'][] = $this->getSubMenuItem($URLParts, 'bank-account/' . $URLParts[1] . '/overview', 'menu_info', 'Přehled');
                    $this->data['sub_menu'][] = $this->getSubMenuItem($URLParts, 'bank-account/' . $URLParts[1] . '/payment', 'menu_payment', 'Platba');
                    $this->data['sub_menu'][] = $this->getSubMenuItem($URLParts, 'bank-account/' . $URLParts[1] . '/cards', 'menu_cards', 'Karty');
                    $this->data['sub_menu'][] = $this->getSubMenuItem($URLParts, 'bank-account/' . $URLParts[1] . '/disponents', 'menu_disponents', 'Disponenti');
                    $this->data['sub_menu'][] = $this->getSubMenuItem($URLParts, 'bank-account/' . $URLParts[1] . '/logs', 'menu_logs', 'Výpisy');
                }
            }
            // EMPLOYEE & ADMIN
            if ($_SESSION['auth']['role'] == 'EMPLOYEE' || $_SESSION['auth']['role'] == 'ADMIN') {
                if (StringUtils::hyphensToCamel($URLParts[0]) == 'pendingRequests') {
                    $this->data['sub_menu'][] = $this->getSubMenuItem($URLParts, 'pending-requests/person', 'menu_request_person', 'Osobní údaje');
                    $this->data['sub_menu'][] = $this->getSubMenuItem($URLParts, 'pending-requests/bank-account', 'menu_request_bank_account', 'Bankovní účty');
                    $this->data['sub_menu'][] = $this->getSubMenuItem($URLParts, 'pending-requests/card', 'menu_request_card', 'Karty');
                    $this->data['sub_menu'][] = $this->getSubMenuItem($URLParts, 'pending-requests/disponent', 'menu_request_disponent', 'Disponenti');
                }
            }
            // ADMIN
        } else {
            // VISITOR
            $this->data['sub_menu'][] = $this->getSubMenuItem($URLParts, 'registration', 'menu_add', 'Registrovat');
            $this->data['sub_menu'][] = $this->getSubMenuItem($URLParts, 'login', 'menu_login', 'Přihlásit');
            $this->data['sub_menu'][] = $this->getSubMenuItem($URLParts, 'about', 'menu_info', 'O nás');
        }

        // redirects
        if ($_SESSION['debug_enabled']) {
            $builder = new HtmlBuilder();
            $builder->startElement('div', array('id' => 'redirects'));
            $builder->startElement('span', array('class' => 'chocolate'));
            $builder->addValue('Redirects: ');
            $builder->endElement();
            $builder->addValue(implode(' -> ', empty($_SESSION['redirects']) ? array('<none>') : $_SESSION['redirects']));
            $builder->endElement();
            $this->data['redirects'] = $builder->render();
        } else {
            $this->data['redirects'] = '';
        }
        unset($_SESSION['redirects']);

        // layout
        $this->view = "layout";
    }

    /**
     * <p>
     * Parse URL and return components of its path.<br />
     * For example if the path is http://my_site.com/first/second?var=val#anchor<br />
     * then this method returns array of ["first", "second"].
     * </p>
     * @param string $url URL to parse, you can get current document's URL with $_SERVER['REQUEST_URI']
     * @return string[] Returns components (words) of its path previously delimited by /
     */
    private function parseURL($url) {
        $path = parse_url($url, PHP_URL_PATH);
        $path = ltrim($path, "/");
        $path = trim($path);
        return explode("/", $path);
    }

    private function isMenuSelected($URLParts, $href) {
        $href = $this->parseURL($href);

        for($i = 0; $i < sizeof($href); $i++) {
            if (!isset($URLParts[$i]) || StringUtils::hyphensToCamel($URLParts[$i]) != StringUtils::hyphensToCamel($href[$i]))
                return '';
        }

        return 'selected';
    }

    private function getMainMenuItem($URLParts, $href, $first, $second) {
        return array('href' => $href, 'selected' => $this->isMenuSelected($URLParts, $href), 'first' => $first, 'second' => $second);
    }

    private function getSubMenuItem($URLParts, $href, $image, $text) {
        return array('href' => $href, 'selected' => $this->isMenuSelected($URLParts, $href), 'image' => $image, 'text' => $text);
    }
}
