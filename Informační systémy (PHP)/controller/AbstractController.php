<?php

abstract class AbstractController {
    protected $data = array();
    protected $view = "";
    protected $header = array('title' => '', 'keywords' => '', 'description' => '');

    public function __construct() {
        // Apply roles for use in templates
        $this->data['is_visitor'] = false;
        $this->data['is_client'] = false;
        $this->data['is_employee'] = false;
        $this->data['is_admin'] = false;
        $this->data['is_privileged'] = false;
        if (!AuthManager::isLoggedIn()) {
            $this->data['is_visitor'] = true;
        } else if ($_SESSION['auth']['role'] == 'CLIENT') {
            $this->data['is_client'] = true;
        } else if ($_SESSION['auth']['role'] == 'EMPLOYEE') {
            $this->data['is_employee'] = true;
            $this->data['is_privileged'] = true;
        } else if ($_SESSION['auth']['role'] == 'ADMIN') {
            $this->data['is_admin'] = true;
            $this->data['is_privileged'] = true;
        }
    }

    abstract public function execute($params);

    /**
     * Prints template providing all data as variables defined by direct names for use.<br />
     * All variables are secured from XSS. Unsecured vars are available with underscore prefix.
     */
    public function printView() {
        if ($this->view) {
            extract($this->secureChars($this->data));
            extract($this->data, EXTR_PREFIX_ALL, "");
            require("view/" . $this->view . ".phtml");
        }
    }

    /**
     * Redirects user immediately and ends script execution.
     * @param $url string URL to redirect user to
     */
    public function redirect($url) {
        header("Location: /$url");
        header("Connection: close");
        $_SESSION['redirects'][] = $url;
        exit;
    }

    /**
     * Recursively fixes special chars to prevent from XSS.
     * @param $x mixed What to secure
     * @return mixed Secured input
     */
    public function secureChars($x = null) {
        if (!isset($x))
            return null;
        else if (is_string($x))
            return htmlspecialchars($x, ENT_QUOTES);
        else if (is_array($x)) {
            foreach($x as $key => $value) {
                $x[$key] = $this->secureChars($value);
            }
            return $x;
        }
        else
            return $x;
    }

    public function addMessage($message) {
        if (isset($_SESSION['messages']))
            $_SESSION['messages'][] = $message;
        else
            $_SESSION['messages'] = array($message);
    }

    public function getMessages() {
        if (isset($_SESSION['messages'])) {
            $result = $_SESSION['messages'];
            unset($_SESSION['messages']);
            return $result;
        }
        else
            return array();
    }

    /**
     * Allows accessing this page only to certain roles
     * @param bool $visitor
     * @param bool $client
     * @param bool $employee
     * @param bool $admin
     */
    public function restrictAccess($visitor = false, $client = false, $employee = false, $admin = false) {
        if (!AuthManager::isLoggedIn()) {
            if (!$visitor) {
                $this->redirect('intro');
                return;
            }
        } else {
            if ($_SESSION['auth']['role'] === 'CLIENT' && !$client) {
                $this->redirect('ib-client');
                return;
            }
            if ($_SESSION['auth']['role'] === 'EMPLOYEE' && !$employee) {
                $this->redirect('ib-privileged');
                return;
            }
            if ($_SESSION['auth']['role'] === 'ADMIN' && !$admin) {
                $this->redirect('ib-privileged');
                return;
            }
        }
    }

    public function needAtLeastParams($params, $number, $custom_redirect = 'intro') {
        if (!isset($params) || !$params || sizeof($params) < $number || empty($params[$number-1])) {
            $this->redirect($custom_redirect);
        }
    }
}
