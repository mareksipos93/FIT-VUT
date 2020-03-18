<?php
class AuthManager {

    public static $person_request_states = array('PENDING' => 'Nevyřízena', 'VERIFIED' => 'Vyřízena', 'INVALID' => 'Zamítnuta');
    public static $person_states = array('PENDING' => 'Neověřeny', 'VERIFIED' => 'Ověřeny', 'INVALID' => 'Neplatné');
    public static $ib_account_states = array('PENDING' => 'Neověřen email', 'ACTIVE' => 'Aktivní', 'DEACTIVATED' => 'Deaktivován');
    public static $country_codes = array('CZE' => 'Česká republika', 'SVK' => 'Slovenská republika');
    public static $roles = array('CLIENT' => 'Klient', 'EMPLOYEE' => 'Zaměstnanec', 'ADMIN' => 'Administrátor');

    public static function isRegistered($name) {
        return Db::queryOneValue('SELECT COUNT(*) FROM ib_account WHERE name = ?', array($name)) > 0;
    }

    public static function personExists($identification) {
        return Db::queryOneValue('SELECT COUNT(*) FROM person WHERE identification LIKE "%\_' . $identification . '"', array()) > 0;
    }

    public static function emailExists($email) {
        return Db::queryOneValue('SELECT COUNT(*) FROM ib_account WHERE email = ?', array($email)) > 0;
    }

    public static function insertPerson($first_name, $second_name, $identification, $country_code, $address, $birthdate) {
        Db::insert('person', array(
            'first_name' => $first_name,
            'second_name' => $second_name,
            'identification' => $identification,
            'country_code' => $country_code,
            'address' => $address,
            'birthdate' => $birthdate
        ));
        $id = Db::getLastInsertId();
        Db::update('person', array('identification' => $id . '_' . $identification), 'WHERE id = ?', array($id));
        return $id;
    }

    public static function verifyPerson($person_id, $new_state, $verified_by_ib, $comment = '') {
        $params = array(
            'state' => $new_state,
            'verified_by' => self::getEmployeeIdByIbAccountId($verified_by_ib),
            'verified_when' => DateUtils::dbNow(),
            'verified_comment' => nl2br($comment)
        );
        Db::update('person', $params, 'WHERE id = ?', array($person_id));
    }

    public static function insertIbAccount($name, $password, $personID, $email) {
        Db::insert('ib_account', array(
            'name' => $name,
            'password' => $password,
            'person' => $personID,
            'email' => $email
        ));
        $last_id = Db::getLastInsertId();
        self::createVerificationCodeByAccountId($last_id);
        return $last_id;
    }

    public static function changeIbAccountState($ib_account_id, $new_state) {
        Db::update('ib_account', array('state' => $new_state), 'WHERE id = ?', array($ib_account_id));
    }

    public static function changePersonState($person_id, $new_state) {
        Db::update('person', array('state' => $new_state), 'WHERE id = ?', array($person_id));
    }

    public static function createVerificationCodeByAccountId($ib_account_id) {
        Db::update('ib_account', array('activation_code' => self::generateVerificationCode($ib_account_id)),
            'WHERE id = ?', array($ib_account_id));
    }

    public static function createChangePwCodeByAccountId($ib_account_id) {
        Db::update('ib_account', array('reset_pw_code' => self::generateVerificationCode($ib_account_id)),
            'WHERE id = ?', array($ib_account_id));
    }

    private static function generateVerificationCode($id = '') {
        $id = "$id";
        return $id . StringUtils::generateRandomString(16-strlen($id));
    }

    public static function getActivationCode($ib_name) {
        return Db::queryOneValue('SELECT activation_code FROM ib_account WHERE name = ?', array($ib_name));
    }

    public static function activateAccountUsingCode($code) {
        $ibAccount = self::getIbAccount(-1, '', '', $code);
        if (!$ibAccount || $ibAccount['state'] != 'PENDING') return false;
        Db::update('ib_account', array('state' => 'ACTIVE', 'activation_code' => NULL), 'WHERE activation_code = ?', array($code));
        return $ibAccount;
    }

    public static function changePasswordUsingCode($code, $new_pw) {
        Db::update('ib_account', array('password' => password_hash($new_pw, PASSWORD_DEFAULT), 'reset_pw_code' => NULL), 'WHERE reset_pw_code = ?', array($code));
    }

    public static function changePassword($ib_acc_id, $new_pw) {
        Db::update('ib_account', array('password' => password_hash($new_pw, PASSWORD_DEFAULT)), 'WHERE id = ?', array($ib_acc_id));
    }

    public static function changeEmail($ib_acc_id, $new_email) {
        Db::update('ib_account', array('email' => $new_email), 'WHERE id = ?', array($ib_acc_id));
    }

    public static function getIbAccount($id = -1, $name = '', $email = '', $activation_code = '', $reset_pw_code = '') {
        if ($id != -1) {
            return Db::querySingle('SELECT * FROM ib_account WHERE id = ?', array($id));
        } else if ($name != '') {
            return Db::querySingle('SELECT * FROM ib_account WHERE name = ?', array($name));
        } else if ($email != '') {
            return Db::querySingle('SELECT * FROM ib_account WHERE email = ?', array($email));
        } else if ($activation_code != '') {
            return Db::querySingle('SELECT * FROM ib_account WHERE activation_code = ?', array($activation_code));
        } else if ($reset_pw_code != '') {
            return Db::querySingle('SELECT * FROM ib_account WHERE reset_pw_code = ?', array($reset_pw_code));
        } else {
            return false;
        }
    }

    public static function getRoleOfIbAccount($ib_account_id) {
        return Db::queryOneValue('SELECT role FROM ib_account WHERE id = ?', array($ib_account_id));
    }

    public static function getEmployeeIdByIbAccountId($ib_account_id) {
        return Db::queryOneValue('SELECT id FROM employee WHERE ib_account = ?', array($ib_account_id));
    }

    public static function getIbAccountsOfPerson($person_id) {
        return Db::queryMulti('SELECT * FROM ib_account WHERE person = ?', array($person_id));
    }

    public static function getIbAccountIdByEmployeeId($employee_id) {
        return Db::queryOneValue('SELECT ib_account FROM employee WHERE id = ?', array($employee_id));
    }

    public static function getIbAccountIdByDisponentId($disponent_id) {
        return Db::queryOneValue('SELECT ib_account.id FROM ib_account
                                         JOIN disponent ON ib_account.id = disponent.ib_account
                                         WHERE disponent.id = ?', array($disponent_id));
    }

    public static function getAmountOfPendingPersons() {
        return Db::queryOneValue('SELECT COUNT(*) FROM person WHERE state = ?', array('PENDING'));
    }

    /**
     * @param $name
     * @param $password
     * @throws CustomException
     */
    public static function tryLogin($name, $password) {
        $ibAccount = self::getIbAccount(-1, $name);
        if (!$ibAccount || !self::checkIbAccountPassword($name, $password)) {
            throw new CustomException('Nesprávné uživatelské jméno nebo heslo');
        }
        $person = self::getIbAccountPerson($ibAccount['name']);
        if (!$person) {
            throw new CustomException('Nepodařilo se načíst osobu přidruženou k tomuto účtu');
        }

        // Check if account is active
        if ($ibAccount['state'] == 'PENDING') {
            self::sendVerificationEmail(self::getIbAccountEmail($name), $name);
            throw new CustomException('Tento účet není ověřen. Zadejte kód, který vám přišel e-mailem, na <a href="/activation">aktivační stránce</a>.');
        }
        if ($ibAccount['state'] == 'DEACTIVATED') {
            throw new CustomException('Tento účet je deaktivován.');
        }

        self::logOut();

        // Login
        $_SESSION['auth'] = array(
            'id' => $ibAccount['id'],
            'name' => $ibAccount['name'],
            'role' => $ibAccount['role'],
            'first_name' => $person['first_name'],
            'second_name' => $person['second_name'],
        );
    }

    public static function checkIbAccountPassword($name,  $password) {
        $correctPw = self::getIbAccountPassword($name);
        return ($password === $correctPw || password_verify($password, $correctPw));
    }

    public static function sendVerificationEmail($email, $ib_name) {
        $code = self::getActivationCode($ib_name);
        EmailUtils::sendEmail($email, 'admin@iwitrag.cz', 'Aktivace účtu - SIMPLE BANK',
            'Dobrý den,<br /><br />váš e-mail je nutné aktivovat.<br />Váš aktivační kód je <b>' . $code . '</b><br />
                      E-mail můžete aktivovat kliknutím <a href="http://'.$_SERVER['SERVER_NAME'].'/activation?code='.$code.'">ZDE</a>.
                      <br /><i>Vaše Simple Bank</i>');
    }

    public static function sendLostCredentialsEmail($email) {
        $account = self::getIbAccount(-1, '', $email);
        if ($account) {
            EmailUtils::sendEmail($email, 'admin@iwitrag.cz', 'Ztracené údaje - SIMPLE BANK',
                'Dobrý den,<br /><br />zapomenuté heslo k účtu <b>' . $account['name'] . '</b> si můžete obnovit 
                          kliknutím na <a href="http://'.$_SERVER['SERVER_NAME'].'/reset-password?code='.$account['reset_pw_code'].'">TENTO ODKAZ</a>.
                          <br /><i>Vaše Simple Bank</i>');
        }
    }

    public static function getIbAccountId($name) {
        return Db::queryOneValue('SELECT id FROM ib_account WHERE name = ?', array($name));
    }

    public static function getIbAccountName($id) {
        return Db::queryOneValue('SELECT name FROM ib_account WHERE id = ?', array($id));
    }

    public static function getIbAccountEmail($name) {
        return Db::queryOneValue('SELECT email FROM ib_account WHERE name = ?', array($name));
    }

    public static function getIbAccountState($name) {
        return Db::queryOneValue('SELECT state FROM ib_account WHERE name = ?', array($name));
    }

    public static function getIbAccountPassword($name) {
        return Db::queryOneValue('SELECT password FROM ib_account WHERE name = ?', array($name));
    }

    public static function getIbAccountPerson($ib_name) {
        return Db::querySingle('SELECT person.* FROM person
                                       JOIN ib_account ON ib_account.person = person.id
                                       WHERE ib_account.name = ?', array($ib_name));
    }

    public static function getPersonByIbAccountId($ib_account_id) {
        return Db::querySingle('SELECT person.* FROM person
                                      JOIN ib_account ON person.id = ib_account.person
                                      WHERE ib_account.id = ?', array($ib_account_id));
    }

    public static function getPersonIdByIbAccountId($ib_account_id) {
        return Db::queryOneValue('SELECT person FROM ib_account WHERE id = ?', array($ib_account_id));
    }

    public static function getPerson($person_id) {
        return Db::querySingle('SELECT * FROM person WHERE id = ?', array($person_id));
    }

    public static function getPersonsByFilter($states = array()) {
        return Db::selectMulti('*', 'person', array(), 'state', $states, 'ORDER BY id DESC');
    }

    public static function getRequesterOfDisponent($disponent_id) {
        return Db::querySingle('SELECT ib_account.* FROM ib_account
                                       JOIN account ON ib_account.id = account.owner
                                       JOIN disponent ON account.id = disponent.account
                                       WHERE disponent.id = ?', array($disponent_id));
    }

    public static function getRequesterIbAccountIdOfDisponent($disponent_id) {
        return Db::queryOneValue('SELECT ib_account.id FROM ib_account
                                       JOIN account ON ib_account.id = account.owner
                                       JOIN disponent ON account.id = disponent.account
                                       WHERE disponent.id = ?', array($disponent_id));
    }

    public static function renderIbAccountHref($ib_account_id) {
        $builder = new HtmlBuilder();
        $ib_account = self::getIbAccount($ib_account_id);
        $title = self::$ib_account_states[$ib_account['state']];
        if ($ib_account['state'] == 'ACTIVE') $mark = '&check; '; else $mark = '&cross; ';
        $builder->addValueElement('a', $mark . $ib_account['name'], array('title' => $title, 'href' => '/database/detail/ib-account/' . $ib_account['name']), true);
        return $builder->render();
    }

    public static function renderIbAccountsOfPerson($person_id) {
        $ib_accounts = ArrayUtils::mapSingles(self::getIbAccountsOfPerson($person_id), 'id');
        foreach ($ib_accounts as $key => $value)
            $ib_accounts[$key] = self::renderIbAccountHref($value);
        return empty($ib_accounts) ? '---' : implode(', ', $ib_accounts);
    }

    public static function renderClientOneLiner($ib_account_id, $include_identification, $include_state_warning, $include_ib_link, $force_single_line = false) {
        $builder = new HtmlBuilder();
        $builder->addValue(self::renderPersonOneLiner(self::getPersonIdByIbAccountId($ib_account_id), $include_identification, $include_state_warning), true);
        if ($include_ib_link) {
            if (!$force_single_line)
                $builder->addElement('br');
            else
                $builder->addValue(' ');
            $builder->addValue('IB účet: ');
            $builder->addValue(self::renderIbAccountHref($ib_account_id), true);
        }
        return $builder->render();
    }

    public static function renderPersonOneLiner($person_id, $include_identification, $include_state_warning) {
        $person = self::getPerson($person_id);
        $builder = new HtmlBuilder();
        if ($include_state_warning) {
            if ($person['state'] == 'PENDING')
                $builder->addValueElement('a', '', array('title' => 'Tento klient není ověřen! Kliknutím přejdete k žádosti', 'class' => 'warning_btn', 'href' => '/request-detail/person/' . $person['id']), true);
            if ($person['state'] == 'INVALID')
                $builder->addValueElement('a', '', array('title' => 'Údaje klienta nejsou platné! Kliknutím přejdete k žádosti', 'class' => 'warning_btn', 'href' => '/request-detail/person/' . $person['id']), true);
        }
        $identification = '';
        if ($include_identification)
            $identification = ' (' . self::trimIdentification($person['identification']) . ')';
        $builder->addValue($person['first_name'] . ' ' . $person['second_name'] . $identification);
        return $builder->render();
    }

    public static function trimIdentification($identification) {
        return substr(strstr($identification, '_'), 1);
    }

    public static function isLoggedIn() {
        return isset($_SESSION['auth']);
    }

    public static function logOut() {
        unset($_SESSION['auth']);
    }

    /**
     * @param $persons
     * @param $show_date
     * @param $show_state
     * @param $show_person 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_ib_accounts
     * @param $show_country
     * @param $show_address
     * @param $show_birthdate
     * @param $show_verified_by 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_verified_when
     * @param $show_verified_comment
     * @param $show_detail
     * @param string $return
     * @return string
     */
    public static function renderPersonRequestsAsTable($persons, $show_date, $show_state, $show_person, $show_ib_accounts, $show_country, $show_address,
                                                       $show_birthdate, $show_verified_by, $show_verified_when, $show_verified_comment,
                                                       $show_detail, $return = '') {
        if (!$persons) return '';

        if (!empty($return))
            $return = '?return=' . $return;

        $builder = new HtmlBuilder();
        $builder->startElement('table', array('class' => 'table'));

        // table header
        $builder->startElement('tr');
        if ($show_date)
            $builder->addValueElement('th', 'Zažádáno');
        if ($show_state)
            $builder->addValueElement('th', 'Stav žádosti');
        if ($show_person)
            $builder->addValueElement('th', 'Osoba');
        if ($show_ib_accounts)
            $builder->addValueElement('th', 'IB účty');
        if ($show_country)
            $builder->addValueElement('th', 'Země');
        if ($show_address)
            $builder->addValueElement('th', 'Adresa');
        if ($show_birthdate)
            $builder->addValueElement('th', 'Datum narození');
        if ($show_verified_by)
            $builder->addValueElement('th', 'Vyřídil');
        if ($show_verified_when)
            $builder->addValueElement('th', 'Kdy');
        if ($show_verified_comment)
            $builder->addValueElement('th', 'Komentář');
        if ($show_detail)
            $builder->addValueElement('th', '', array('class' => 'hide'));
        $builder ->endElement(); // tr

        // table rows
        foreach ($persons as $person) {
            $builder->startElement('tr');

            if ($show_date)
                $builder->addValueElement('td', DateUtils::getPrettyDateTime($person['inserted']));
            if ($show_state)
                $builder->addValueElement('td', self::$person_request_states[$person['state']]);
            if ($show_person != false)
                $builder->addValueElement('td',self::renderPersonOneLiner($person['id'], $show_person >= 2, $show_person >= 3), array(), true);
            if ($show_ib_accounts)
                $builder->addValueElement('td',self::renderIbAccountsOfPerson($person['id']), array(), true);
            if ($show_country)
                $builder->addValueElement('td', isset(self::$country_codes[$person['country_code']]) ? self::$country_codes[$person['country_code']] : $person['country_code']);
            if ($show_address)
                $builder->addValueElement('td', $person['address']);
            if ($show_birthdate)
                $builder->addValueElement('td', DateUtils::getPrettyDate($person['birthdate']));
            if ($show_verified_by != false)
                $builder->addValueElement('dd', is_null($person['verified_by']) ? '---' : AuthManager::renderClientOneLiner(AuthManager::getIbAccountIdByEmployeeId($person['verified_by']), $show_verified_by === 2, $show_verified_by === 3, $show_verified_by === 3), array(), true);
            if ($show_verified_when)
                $builder->addValueElement('td', is_null($person['verified_when']) ? '---' : DateUtils::getPrettyDateTime($person['verified_when']));
            if ($show_verified_comment)
                $builder->addValueElement('td', is_null($person['verified_comment']) ? '---' : $person['verified_comment'], array(), true);
            if ($show_detail) {
                $builder->startElement('td', array('class' => 'hide'));
                $builder->addValueElement('a', '', array('title' => 'Zobrazit detail', 'class' => 'view_btn', 'href' => '/request-detail/person/' . $person['id']));
                $builder->endElement(); // td
            }

            $builder ->endElement(); // tr
        }

        $builder->endElement(); // table
        return $builder->render();
    }

    /**
     * @param $person
     * @param $show_date
     * @param $show_state 1 = as person, 2 = as request
     * @param $show_person 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_identification
     * @param $show_ib_accounts
     * @param $show_country
     * @param $show_address
     * @param $show_birthdate
     * @param $show_verified_by 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_verified_when
     * @param $show_verified_comment
     * @return string
     */
    public static function renderPerson($person, $show_date, $show_state, $show_person, $show_identification, $show_ib_accounts, $show_country, $show_address,
                                               $show_birthdate, $show_verified_by, $show_verified_when, $show_verified_comment) {
        if (!$person) return '';

        $builder = new HtmlBuilder();
        $builder->startElement('dl', array('class' => 'detail'));
        
        if ($show_date) {
            $builder->addValueElement('dt', 'Vytvořeno');
            $builder->addValueElement('dd', DateUtils::getPrettyDateTime($person['inserted']));
        }
        if ($show_state === true || $show_state === 1) {
            $builder->addValueElement('dt', 'Osobní údaje');
            $builder->addValueElement('dd', self::$person_states[$person['state']]);
        }
        if ($show_state === 2) {
            $builder->addValueElement('dt', 'Stav žádosti');
            $builder->addValueElement('dd', self::$person_request_states[$person['state']]);
        }
        if ($show_person != false) {
            $builder->addValueElement('dt', 'Osoba');
            $builder->addValueElement('dd',self::renderPersonOneLiner($person['id'], $show_person >= 2, $show_person >= 3), array(), true);
        }
        if ($show_identification) {
            $builder->addValueElement('dt', 'Číslo OP');
            $builder->addValueElement('dd',self::trimIdentification($person['identification']));
        }
        if ($show_ib_accounts) {
            $builder->addValueElement('dt', 'IB účty');
            $builder->addValueElement('dd',self::renderIbAccountsOfPerson($person['id']), array(), true);
        }
        if ($show_country) {
            $builder->addValueElement('dt', 'Země');
            $builder->addValueElement('dd', isset(self::$country_codes[$person['country_code']]) ? self::$country_codes[$person['country_code']] : $person['country_code']);
        }
        if ($show_address) {
            $builder->addValueElement('dt', 'Adresa');
            $builder->addValueElement('dd', $person['address']);
        }
        if ($show_birthdate) {
            $builder->addValueElement('dt', 'Datum narození');
            $builder->addValueElement('dd', DateUtils::getPrettyDate($person['birthdate']));
        }
        if ($show_verified_by != false) {
            $builder->addValueElement('dt', 'Vyřídil');
            $builder->addValueElement('dd', is_null($person['verified_by']) ? '---' : AuthManager::renderClientOneLiner(AuthManager::getIbAccountIdByEmployeeId($person['verified_by']), $show_verified_by === 2, $show_verified_by === 3, $show_verified_by === 3), array(), true);
        }
        if ($show_verified_when) {
            $builder->addValueElement('dt', 'Kdy');
            $builder->addValueElement('dd', is_null($person['verified_when']) ? '---' : DateUtils::getPrettyDateTime($person['verified_when']));
        }
        if ($show_verified_comment) {
            $builder->addValueElement('dt', 'Komentář');
            $builder->addValueElement('dd', is_null($person['verified_comment']) ? '---' : $person['verified_comment'], array(), true);
        }

        $builder->endElement(); // dl

        return $builder->render();
    }

    public static function renderIbAccount($ib_account, $show_name, $show_role, $show_state, $show_date, $show_email) {
        if (!$ib_account) return '';

        $builder = new HtmlBuilder();
        $builder->startElement('dl', array('class' => 'detail'));

        if ($show_name) {
            $builder->addValueElement('dt', 'Název');
            $builder->addValueElement('dd', $ib_account['name']);
        }
        if ($show_role) {
            $builder->addValueElement('dt', 'Role');
            $builder->addValueElement('dd', self::$roles[$ib_account['role']]);
        }
        if ($show_state) {
            $builder->addValueElement('dt', 'Stav');
            $builder->addValueElement('dd', self::$ib_account_states[$ib_account['state']]);
        }
        if ($show_date) {
            $builder->addValueElement('dt', 'Vytvořeno');
            $builder->addValueElement('dd', DateUtils::getPrettyDateTime($ib_account['inserted']));
        }
        if ($show_email) {
            $builder->addValueElement('dt', 'E-mail');
            $builder->addValueElement('dd', $ib_account['email']);
        }

        $builder->endElement(); // dl

        return $builder->render();
    }

}