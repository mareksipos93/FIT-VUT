<?php
class AccountManager {

    public static $request_states = array('PENDING' => 'Nevyřízena', 'REJECTED' => 'Zamitnuta', 'ACTIVE' => 'Vyřízena', 'DEACTIVATED' => 'Vyřízena');
    public static $account_states = array('PENDING' => 'V žádosti', 'REJECTED' => 'Zamitnut', 'ACTIVE' => 'Aktivní', 'DEACTIVATED' => 'Uzavřen');

    public static function getAllActiveUsersBankAccounts($name) {
        $sql = 'SELECT a.number, a.funds, t.name, d.custom_name FROM disponent AS d
                JOIN account AS a ON d.account = a.id
                JOIN account_type AS t ON a.type = t.id
                JOIN ib_account AS i ON d.ib_account = i.id
                WHERE a.state = ? AND i.name = ? AND d.state = ?';
        return Db::queryMulti($sql, array('ACTIVE', $name, 'ACTIVE'));
    }

    public static function verifyBankAccount($bank_account_id, $new_state, $verified_by_ib, $comment = '') {
        $params = array(
            'state' => $new_state,
            'verified_by' => AuthManager::getEmployeeIdByIbAccountId($verified_by_ib),
            'verified_when' => DateUtils::dbNow(),
            'verified_comment' => nl2br($comment)
        );
        Db::update('account', $params, 'WHERE id = ?', array($bank_account_id));
        if ($new_state == 'ACTIVE') {
            self::addNumberToBankAccount($bank_account_id);
        }
    }

    public static function addNumberToBankAccount($bank_account_id) {
        $account = self::getBankAccount($bank_account_id);
        if ($account && (is_null($account['number']) || $account['number'] == 0)) {
            $num = StringUtils::generateRandomString(10, true);
            while (self::getBankAccountByNumber($num) != false) {
                $num = StringUtils::generateRandomString(10, true);
            }
            Db::update('account', array('number' => $num), 'WHERE id = ?', array($bank_account_id));
        }
    }

    public static function getBankAccountByNumber($account_number) {
        return Db::querySingle('SELECT * FROM account WHERE number = ?', array($account_number));
    }

    public static function getAmountOfPendingBankAccounts() {
        return Db::queryOneValue('SELECT COUNT(*) FROM account WHERE state = ?', array('PENDING'));
    }

    public static function getOfferedAccountTypes() {
        return Db::queryMulti('SELECT * FROM account_type WHERE offered = 1');
    }

    public static function getAllAccountTypes() {
        return Db::queryMulti('SELECT * FROM account_type');
    }

    public static function getAccountTypeName($type_id) {
        return Db::queryOneValue('SELECT name FROM account_type WHERE id = ?', array($type_id));
    }

    public static function getAccountTypeByTypeId($type_id) {
        return Db::querySingle('SELECT * FROM account_type WHERE id = ?', array($type_id));
    }

    public static function getAccountNumber($bank_account_id) {
        return Db::queryOneValue('SELECT number FROM account WHERE id = ?', array($bank_account_id));
    }

    public static function getAccountIdByCardId($card_id) {
        return Db::queryOneValue('SELECT account.id FROM account 
                                      JOIN disponent ON account.id = disponent.account 
                                      JOIN card ON disponent.id = card.disponent
                                      WHERE card.id = ?', array($card_id));
    }

    public static function newAccountRequest($account_type, $ib_account_id) {
        Db::insert('account', array('type' => $account_type, 'owner' => $ib_account_id));
        return Db::getLastInsertId();
    }

    public static function getBankAccount($id) {
        return Db::querySingle('SELECT * FROM account WHERE id = ?', array($id));
    }

    public static function getBankAccountsByFilter($ib_account_id, $states = array()) {
        $conditions = empty($ib_account_id) ? array() : array('owner' => $ib_account_id);
        return Db::selectMulti('*', 'account', $conditions, 'state', $states, 'ORDER BY id DESC');
    }

    public static function getBankAccountOfCard($card_id) {
        return Db::querySingle('SELECT a.* FROM account AS a
                                       JOIN disponent AS d ON d.account = a.id
                                       JOIN card AS c ON c.disponent = d.id
                                       WHERE c.id = ?', array($card_id));
    }

    public static function getBankAccountNameByDisponentId($disponent_id) {
        $disponent = DisponentManager::getDisponent($disponent_id);
        if (!is_null($disponent['custom_name']))
            return $disponent['custom_name'];
        $account = self::getBankAccount($disponent['account']);
        $r = self::getAccountTypeByTypeId($account['type']);
        return $r['name'];
    }

    public static function getBankAccountIdByDisponentId($disponent_id) {
        return Db::queryOneValue('SELECT account FROM disponent WHERE id = ?', array($disponent_id));
    }

    public static function removeCustomBankAccountName($disponent_id) {
        Db::update('disponent', array('custom_name' => null), 'WHERE id = ?', array($disponent_id));
    }

    public static function setCustomBankAccountName($disponent_id, $new_name) {
        Db::update('disponent', array('custom_name' => $new_name), 'WHERE id = ?', array($disponent_id));
    }

    public static function deleteBankAccount($id) {
        return Db::query('DELETE FROM account WHERE id = ?', array($id));
    }

    public static function addMoney($bank_account_id, $amount) {
        return Db::query('UPDATE account SET funds = funds + ? WHERE id = ?', array($amount, $bank_account_id));
    }

    public static function subtractMoney($bank_account_id, $amount) {
        return Db::query('UPDATE account SET funds = funds - ? WHERE id = ?', array($amount, $bank_account_id));
    }

    public static function addPayment($from_acc, $from_bank_code, $to_acc, $to_bank_code, $amount, $creator_disponent_id, $variable_symbol = '', $constant_symbol = '', $specific_symbol = '', $message = '') {
        $params = array(
            'from_acc' => $from_acc,
            'from_bank_code' => $from_bank_code,
            'to_acc' => $to_acc,
            'to_bank_code' => $to_bank_code,
            'amount' => $amount,
            'creator_disponent_id' => $creator_disponent_id
        );
        if (!empty($variable_symbol))
            $params['variable_symbol'] = $variable_symbol;
        if (!empty($constant_symbol))
            $params['constant_symbol'] = $constant_symbol;
        if (!empty($specific_symbol))
            $params['specific_symbol'] = $specific_symbol;
        if (!empty($message))
            $params['message'] = $message;
        return Db::insert('payment', $params);
    }

    public static function renderBankAccountHref($bank_account_id) {
        $builder = new HtmlBuilder();
        $bank_account = self::getBankAccount($bank_account_id);
        $title = self::$account_states[$bank_account['state']];
        if ($bank_account['state'] == 'ACTIVE') $mark = '&check; '; else $mark = '&cross; ';
        $number = is_null($bank_account['number']) ? 'Bez čísla' : $bank_account['number'];
        $href = $bank_account['state'] == 'PENDING' ? '/pending-requests/bank-account/' . $bank_account_id : '/database/detail/bank-account/' . $bank_account_id;
        $builder->addValueElement('a', $mark . $number, array('title' => $title, 'href' => $href), true);
        return $builder->render();
    }

    /**
     * @param $accounts
     * @param $show_date
     * @param $show_author 1 = full name, 2 = +identification, 3 = +state warning +ib link
     * @param $show_type
     * @param $show_state
     * @param $show_verified_by 1 = full name, 2 = +identification, 3 = +state warning +ib link
     * @param $show_verified_when
     * @param $show_verified_comment
     * @param $show_delete
     * @param $show_detail
     * @param string $return
     * @return string
     */
    public static function renderBankAccountRequestsAsTable($accounts, $show_date, $show_author, $show_type, $show_state,
                                                            $show_verified_by, $show_verified_when, $show_verified_comment,
                                                            $show_delete, $show_detail, $return = '') {
        if (!$accounts) return '';

        if (!empty($return))
            $return = '?return=' . $return;

        $builder = new HtmlBuilder();
        $builder->startElement('table', array('class' => 'table'));

        // table header
        $builder->startElement('tr');
        if ($show_date)
            $builder->addValueElement('th', 'Zažádáno');
        if ($show_author)
            $builder->addValueElement('th', 'Zažádal');
        if ($show_type)
            $builder->addValueElement('th', 'Typ účtu');
        if ($show_state)
            $builder->addValueElement('th', 'Stav žádosti');
        if ($show_verified_by)
            $builder->addValueElement('th', 'Vyřídil');
        if ($show_verified_when)
            $builder->addValueElement('th', 'Kdy');
        if ($show_verified_comment)
            $builder->addValueElement('th', 'Komentář');
        if ($show_delete)
            $builder->addValueElement('th', '', array('class' => 'hide'));
        if ($show_detail)
        $builder->addValueElement('th', '', array('class' => 'hide'));
        $builder ->endElement(); // tr

        // table rows
        foreach ($accounts as $account) {
            $builder->startElement('tr');

            if ($show_date)
                $builder->addValueElement('td', DateUtils::getPrettyDateTime($account['inserted']));
            if ($show_author != false)
                $builder->addValueElement('td', AuthManager::renderClientOneLiner($account['owner'], $show_author >= 2, $show_author >= 3, $show_author >= 3), array(), true);
            if ($show_type)
                $builder->addValueElement('td', self::getAccountTypeName($account['type']));
            if ($show_state)
                $builder->addValueElement('td', self::$request_states[$account['state']]);
            if ($show_verified_by != false)
                $builder->addValueElement('dd', is_null($account['verified_by']) ? '---' : AuthManager::renderClientOneLiner(AuthManager::getIbAccountIdByEmployeeId($account['verified_by']), $show_verified_by === 2, $show_verified_by === 3, $show_verified_by === 3), array(), true);
            if ($show_verified_when)
                $builder->addValueElement('td', is_null($account['verified_when']) ? '---' : DateUtils::getPrettyDateTime($account['verified_when']));
            if ($show_verified_comment)
                $builder->addValueElement('td', is_null($account['verified_comment']) ? '---' : $account['verified_comment'], array(), true);
            if ($show_delete && $account['state'] == 'PENDING') {
                $builder->startElement('td', array('class' => 'hide'));
                $builder->addValueElement('a', '', array('title' => 'Odstranit', 'class' => 'delete_btn', 'href' => '/delete-request/bank-account/' . $account['id'] . $return));
                $builder->endElement(); // td
            }
            if ($show_detail) {
                $builder->startElement('td', array('class' => 'hide'));
                $builder->addValueElement('a', '', array('title' => 'Zobrazit detail', 'class' => 'view_btn', 'href' => '/request-detail/bank-account/' . $account['id']));
                $builder->endElement(); // td
            }

            $builder ->endElement(); // tr
        }

        $builder->endElement(); // table
        return $builder->render();
    }

    /**
     * @param $bankAccount
     * @param $show_date
     * @param $show_type
     * @param $show_number
     * @param $show_owner 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_state 1 = as account, 2 = as request
     * @param $show_funds
     * @param $show_cards
     * @param $show_disponents
     * @param $show_verified_by 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_verified_when
     * @param $show_verified_comment
     * @return string
     */
    public static function renderBankAccount($bankAccount, $show_date, $show_type, $show_number, $show_owner, $show_state,
                $show_funds, $show_cards, $show_disponents, $show_verified_by, $show_verified_when, $show_verified_comment) {
        if (!$bankAccount) return '';

        $builder = new HtmlBuilder();
        $builder->startElement('dl', array('class' => 'detail'));

        if ($show_date) {
            $builder->addValueElement('dt', 'Vytvořeno');
            $builder->addValueElement('dd', DateUtils::getPrettyDateTime($bankAccount['inserted']));
        }
        if ($show_type == true) {
            $builder->addValueElement('dt', 'Typ účtu');
            $builder->addValueElement('dd', self::getAccountTypeName($bankAccount['type']));
        }
        if ($show_number == true) {
            $builder->addValueElement('dt', 'Číslo účtu');
            $builder->addValueElement('dd', is_null($bankAccount['number']) || $bankAccount['number'] == 0 ? '< NEPŘIŘAZENO >' : $bankAccount['number']);
        }
        if ($show_owner != false) {
            $builder->addValueElement('dt', 'Vlastník');
            $builder->addValueElement('dd',AuthManager::renderClientOneLiner($bankAccount['owner'], $show_owner >= 2, $show_owner >= 3, $show_owner >= 3), array(), true);
        }
        if ($show_state === true || $show_state === 1) {
            $builder->addValueElement('dt', 'Stav účtu');
            $builder->addValueElement('dd', self::$account_states[$bankAccount['state']]);
        }
        if ($show_state === 2) {
            $builder->addValueElement('dt', 'Stav žádosti');
            $builder->addValueElement('dd', self::$request_states[$bankAccount['state']]);
        }
        if ($show_funds == true) {
            $builder->addValueElement('dt', 'Zůstatek');
            $builder->addValueElement('dd', StringUtils::formatMoney($bankAccount['funds']) . ' Kč');
        }
        if ($show_cards == true) {
            $builder->addValueElement('dt', 'Karty k účtu');
            $builder->addValueElement('dd', CardManager::renderCardsOfBankAccount($bankAccount['id'], array('ACTIVE')), array(), true);
        }
        if ($show_disponents == true) {
            $builder->addValueElement('dt', 'Účtem disponují');
            $builder->addValueElement('dd', DisponentManager::renderDisponentsOfBankAccount($bankAccount['id'], array('ACTIVE')), array(), true);
        }
        if ($show_verified_by != false) {
            $builder->addValueElement('dt', 'Vyřídil');
            $builder->addValueElement('dd', is_null($bankAccount['verified_by']) ? '---' : AuthManager::renderClientOneLiner(AuthManager::getIbAccountIdByEmployeeId($bankAccount['verified_by']), $show_verified_by === 2, $show_verified_by === 3, $show_verified_by === 3), array(), true);
        }
        if ($show_verified_when) {
            $builder->addValueElement('dt', 'Kdy');
            $builder->addValueElement('dd', is_null($bankAccount['verified_when']) ? '---' : DateUtils::getPrettyDateTime($bankAccount['verified_when']));
        }
        if ($show_verified_comment) {
            $builder->addValueElement('dt', 'Komentář');
            $builder->addValueElement('dd', is_null($bankAccount['verified_comment']) ? '---' : $bankAccount['verified_comment'], array(), true);
        }

        // TODO - sjednotit styly vypsání bankovního účtu (one linery), přidat výběr podrobnosti do parametrů, které výpisů využívají

        $builder->endElement(); // dl

        return $builder->render();
    }

}