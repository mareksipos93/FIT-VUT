<?php
class DisponentManager {

    public static $request_states = array('PENDING' => 'Nevyřízena', 'AGREED' => 'Přijat souhlas', 'REJECTED' => 'Zamitnuta', 'ACTIVE' => 'Vyřízena', 'DEACTIVATED' => 'Vyřízena');
    public static $disponent_states = array('PENDING' => 'V žádosti', 'AGREED' => 'Přijat souhlas', 'REJECTED' => 'Zamitnut', 'ACTIVE' => 'Aktivní', 'DEACTIVATED' => 'Zrušen');

    public static function getAmountOfAgreedDisponents() {
        return Db::queryOneValue('SELECT COUNT(*) FROM disponent WHERE state = ?', array('AGREED'));
    }

    public static function getDisponentsOfClientWhoRequestedThem($ib_account_id) {
        return Db::queryMulti('SELECT disponent.* FROM disponent
                                      JOIN account ON disponent.account = account.id
                                      WHERE account.owner = ? AND disponent.ib_account != account.owner
                                      ORDER BY disponent.id DESC', array($ib_account_id));
    }

    public static function addDisponentRequest($disponent_ib_id, $bank_account_id) {
        Db::insert('disponent', array('state' => 'PENDING', 'account' => $bank_account_id, 'ib_account' => $disponent_ib_id));
        return Db::getLastInsertId();
    }

    public static function agreeDisponent($disponent_id) {
        Db::update('disponent', array('state' => 'AGREED'), 'WHERE id = ?', array($disponent_id));
    }

    public static function disagreeDisponent($disponent_id) {
        Db::update('disponent', array('state' => 'REJECTED'), 'WHERE id = ?', array($disponent_id));
    }

    public static function verifyDisponent($disponent_id, $new_state, $verified_by_ib, $comment = '') {
        $params = array(
            'state' => $new_state,
            'verified_by' => AuthManager::getEmployeeIdByIbAccountId($verified_by_ib),
            'verified_when' => DateUtils::dbNow(),
            'verified_comment' => nl2br($comment)
        );
        Db::update('disponent', $params, 'WHERE id = ?', array($disponent_id));
    }

    public static function getDisponentsOfBankAccount($bank_account_id, $states = array()) {
        $conditions = empty($bank_account_id) ? array() : array('account' => $bank_account_id);
        return Db::selectMulti('*', 'disponent', $conditions, 'state', $states, 'ORDER BY id DESC');
    }

    public static function getDisponentsOfIbAccount($ib_account_id) {
        return Db::queryMulti('SELECT * FROM disponent WHERE ib_account = ?', array($ib_account_id));
    }

    public static function getDisponent($disponent_id) {
        return Db::querySingle('SELECT * FROM disponent WHERE id = ?', array($disponent_id));
    }

    public static function getDisponentIdByIbAccountAndBankAccount($ib_account, $bank_account_id) {
        return Db::queryOneValue('SELECT id FROM disponent WHERE ib_account = ? AND account = ?', array($ib_account, $bank_account_id));
    }

    public static function getActiveDisponentByIbAccountAndBankAccount($ib_account_id, $bank_account_id) {
        return Db::querySingle('SELECT * FROM disponent WHERE ib_account = ? AND account = ? AND state = ?', array($ib_account_id, $bank_account_id, 'ACTIVE'));
    }

    public static function getAllDisponentsByIbAccountAndBankAccount($ib_account_id, $bank_account_id) {
        return Db::queryMulti('SELECT * FROM disponent WHERE ib_account = ? AND account = ?', array($ib_account_id, $bank_account_id));
    }

    public static function deleteDisponent($id) {
        return Db::query('DELETE FROM disponent WHERE id = ?', array($id));
    }

    public static function renderDisponentHref($disponent_id) {
        $builder = new HtmlBuilder();
        $disponent = self::getDisponent($disponent_id);
        $ib_name = AuthManager::getIbAccountName($disponent['ib_account']);
        $person_id = AuthManager::getPersonIdByIbAccountId($disponent['ib_account']);
        $title = self::$disponent_states[$disponent['state']];
        if ($disponent['state'] == 'ACTIVE') $mark = '&check; '; else $mark = '&cross; ';
        $builder->addValue($mark . AuthManager::renderPersonOneLiner($person_id, false, false) . ' ( ', true);
        $href = $disponent['state'] == 'PENDING' || $disponent['state'] == 'AGREED' ? '/pending-requests/disponent/' . $disponent_id : '/database/detail/ib-account/' . $ib_name;
        $builder->addValueElement('a', $ib_name, array('title' => $title, 'href' => $href), true);
        $builder->addValue(')');
        return $builder->render();
    }

    public static function renderDisponentsOfBankAccount($bank_account_id, $states = array()) {
        $disponents = self::getDisponentsOfBankAccount($bank_account_id);
        if (!empty($states)) {
            foreach ($disponents as $key => $value) {
                if (!in_array($value['state'], $states)) {
                    unset($disponents[$key]);
                }
            }
        }
        $disponents = ArrayUtils::mapSingles($disponents, 'id');
        foreach ($disponents as $key => $value) {
            $disponents[$key] = self::renderDisponentHref($value);
        }
        return empty($disponents) ? '---' : implode(', ', $disponents);
    }

    /**
     * @param $disponents
     * @param $show_date
     * @param $show_author 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_account
     * @param $show_disponent 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_state 1 = as disponent, 2 = as request
     * @param $show_verified_by 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_verified_when
     * @param $show_verified_comment
     * @param $show_req_delete
     * @param $show_req_detail
     * @param string $return
     * @return string
     */
    public static function renderDisponentsAsTable($disponents, $show_date, $show_author, $show_account, $show_disponent,
                                                   $show_state, $show_verified_by, $show_verified_when, $show_verified_comment,
                                                   $show_req_delete, $show_req_detail, $return = '') {
        if (!$disponents) return '';

        if (!empty($return))
            $return = '?return=' . $return;

        $builder = new HtmlBuilder();
        $builder->startElement('table', array('class' => 'table'));

        // table header
        $builder->startElement('tr');
        if ($show_date)
            $builder->addValueElement('th', 'Vytvořeno');
        if ($show_author)
            $builder->addValueElement('th', 'Zažádal');
        if ($show_account)
            $builder->addValueElement('th', 'K účtu');
        if ($show_disponent)
            $builder->addValueElement('th', 'Disponent');
        if ($show_state === true || $show_state === 1)
            $builder->addValueElement('th', 'Stav');
        if ($show_state === 2)
            $builder->addValueElement('th', 'Stav žádosti');
        if ($show_verified_by)
            $builder->addValueElement('th', 'Vyřídil');
        if ($show_verified_when)
            $builder->addValueElement('th', 'Kdy');
        if ($show_verified_comment)
            $builder->addValueElement('th', 'Komentář');
        if ($show_req_delete)
            $builder->addValueElement('th', '', array('class' => 'hide'));
        if ($show_req_detail)
            $builder->addValueElement('th', '', array('class' => 'hide'));
        $builder ->endElement(); // tr

        // table rows
        foreach ($disponents as $disponent) {
            $builder->startElement('tr');

            if ($show_date)
                $builder->addValueElement('td', DateUtils::getPrettyDateTime($disponent['inserted']));
            if ($show_author != false)
                $builder->addValueElement('td', AuthManager::renderClientOneLiner(AuthManager::getRequesterIbAccountIdOfDisponent($disponent['id']), $show_author === 2, $show_author === 3, $show_author === 3), array(), true);
            if ($show_account) {
                $acc = AccountManager::getBankAccount($disponent['account']);
                $builder->addValueElement('td', $acc['number'] . ' (' . AccountManager::getBankAccountNameByDisponentId($disponent['id']) . ')');
            }
            if ($show_disponent != false)
                $builder->addValueElement('td', AuthManager::renderClientOneLiner($disponent['ib_account'], $show_disponent === 2, $show_disponent === 3, $show_disponent === 3), array(), true);
            if ($show_state === true || $show_state === 1)
                $builder->addValueElement('td', self::$disponent_states[$disponent['state']]);
            if ($show_state === 2)
                $builder->addValueElement('td', self::$request_states[$disponent['state']]);
            if ($show_verified_by != false)
                $builder->addValueElement('dd', is_null($disponent['verified_by']) ? '---' : AuthManager::renderClientOneLiner(AuthManager::getIbAccountIdByEmployeeId($disponent['verified_by']), $show_verified_by === 2, $show_verified_by === 3, $show_verified_by === 3), array(), true);
            if ($show_verified_when)
                $builder->addValueElement('td', is_null($disponent['verified_when']) ? '---' : DateUtils::getPrettyDateTime($disponent['verified_when']));
            if ($show_verified_comment)
                $builder->addValueElement('td', is_null($disponent['verified_comment']) ? '---' : $disponent['verified_comment'], array(), true);
            if ($show_req_delete && ($disponent['state'] == 'PENDING' || $disponent['state'] == 'AGREED')) {
                $builder->startElement('td', array('class' => 'hide'));
                $builder->addValueElement('a', '', array('title' => 'Odstranit', 'class' => 'delete_btn', 'href' => '/delete-request/disponent/' . $disponent['id'] . $return));
                $builder->endElement(); // td
            }
            if ($show_req_detail) {
                $builder->startElement('td', array('class' => 'hide'));
                $builder->addValueElement('a', '', array('title' => 'Zobrazit detail', 'class' => 'view_btn', 'href' => '/request-detail/disponent/' . $disponent['id']));
                $builder->endElement(); // td
            }

            $builder ->endElement(); // tr
        }

        $builder->endElement(); // table
        return $builder->render();
    }

    /**
     * @param $disponent
     * @param $show_date
     * @param $show_disponent 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_account
     * @param $show_accowner 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_state 1 = as disponent, 2 = as request
     * @param $show_verified_by 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_verified_when
     * @param $show_verified_comment
     * @return string
     */
    public static function renderDisponent($disponent, $show_date, $show_disponent, $show_account, $show_accowner, $show_state,
                                           $show_verified_by, $show_verified_when, $show_verified_comment) {
        if (!$disponent) return '';

        $builder = new HtmlBuilder();
        $builder->startElement('dl', array('class' => 'detail'));

        if ($show_date) {
            $builder->addValueElement('dt', 'Vytvořeno');
            $builder->addValueElement('dd', DateUtils::getPrettyDateTime($disponent['inserted']));
        }
        if ($show_disponent != false) {
            $builder->addValueElement('dt', 'Disponent');
            $builder->addValueElement('dd', AuthManager::renderClientOneLiner($disponent['ib_account'], $show_disponent === 2, $show_disponent ===3, $show_disponent ===3), array(), true);
        }
        if ($show_account) {
            $builder->addValueElement('dt', 'K účtu');
            $builder->addValueElement('dd',AccountManager::renderBankAccountHref($disponent['account']),array(), true);
        }
        if ($show_accowner != false) {
            $builder->addValueElement('dt', 'Disponent');
            $builder->addValueElement('dd', AuthManager::renderClientOneLiner(AuthManager::getRequesterIbAccountIdOfDisponent($disponent['id']), $show_disponent === 2, $show_disponent ===3, $show_disponent ===3), array(), true);
        }
        if ($show_state === true || $show_state === 1) {
            $builder->addValueElement('dt', 'Stav disponenta');
            $builder->addValueElement('dd', self::$disponent_states[$disponent['state']]);
        }
        if ($show_state === 2) {
            $builder->addValueElement('dt', 'Stav žádosti');
            $builder->addValueElement('dd', self::$request_states[$disponent['state']]);
        }
        if ($show_verified_by != false) {
            $builder->addValueElement('dt', 'Vyřídil');
            $builder->addValueElement('dd', is_null($disponent['verified_by']) ? '---' : AuthManager::renderClientOneLiner(AuthManager::getIbAccountIdByEmployeeId($disponent['verified_by']), $show_verified_by === 2, $show_verified_by === 3, $show_verified_by === 3), array(), true);
        }
        if ($show_verified_when) {
            $builder->addValueElement('dt', 'Kdy');
            $builder->addValueElement('dd', is_null($disponent['verified_when']) ? '---' : DateUtils::getPrettyDateTime($disponent['verified_when']));
        }
        if ($show_verified_comment) {
            $builder->addValueElement('dt', 'Komentář');
            $builder->addValueElement('dd', is_null($disponent['verified_comment']) ? '---' : $disponent['verified_comment'], array(), true);
        }

        $builder->endElement(); // dl

        return $builder->render();
    }
}
