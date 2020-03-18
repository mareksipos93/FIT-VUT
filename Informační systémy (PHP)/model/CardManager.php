<?php
class CardManager {

    public static $request_states = array('PENDING' => 'Nevyřízena', 'REJECTED' => 'Zamitnuta', 'MAKING' => 'Ve zpracování', 'ACTIVE' => 'Vyřízena', 'DEACTIVATED' => 'Vyřízena');
    public static $card_states = array('PENDING' => 'V žádosti', 'REJECTED' => 'Zamitnuta', 'MAKING' => 'Připravuje se', 'ACTIVE' => 'Aktivní', 'DEACTIVATED' => 'Deaktivována');

    public static function getCard($id) {
        return Db::querySingle('SELECT * FROM card WHERE id = ?', array($id));
    }

    public static function getCardsOfBankAccount($bank_account_id) {
        return Db::queryMulti('SELECT card.* FROM card 
                              JOIN disponent ON card.disponent = disponent.id 
                              WHERE disponent.account= ?', array($bank_account_id));
    }

    public static function getAmountOfPendingOrMakingCards() {
        return Db::queryOneValue('SELECT COUNT(*) FROM card WHERE state = ? OR state = ?', array('PENDING', 'MAKING'));
    }

    public static function addCardRequest($disponent_id) {
        Db::insert('card', array('state' => 'PENDING', 'disponent' => $disponent_id));
        return Db::getLastInsertId();
    }

    public static function verifyCardStepOne($card_id, $new_state, $verified_by_ib, $comment = '') {
        $params = array(
            'state' => $new_state,
            'verified_by1' => AuthManager::getEmployeeIdByIbAccountId($verified_by_ib),
            'verified_when1' => DateUtils::dbNow(),
            'verified_comment1' => nl2br($comment)
        );
        Db::update('card', $params, 'WHERE id = ?', array($card_id));
    }

    /**
     * @param $card_id
     * @param $new_state
     * @param $verified_by_ib
     * @param string $comment
     * @param string $str_number If activating - must be string, else empty string or nothing
     * @param string $str_pin If activating - must be string, else empty string or nothing
     * @param string $expires If activating - must be date from Form, else empty string or nothing
     */
    public static function verifyCardStepTwo($card_id, $new_state, $verified_by_ib, $comment = '', $str_number = '', $str_pin = '', $expires = '') {
        $params = array(
            'state' => $new_state,
            'verified_by2' => AuthManager::getEmployeeIdByIbAccountId($verified_by_ib),
            'verified_when2' => DateUtils::dbNow(),
            'verified_comment2' => nl2br($comment)
        );
        if ($new_state == 'ACTIVE') {
            $params['number'] = $str_number;
            $params['pin'] = password_hash($str_pin, PASSWORD_DEFAULT);
            $params['expires'] = $expires;
        }
        Db::update('card', $params, 'WHERE id = ?', array($card_id));
    }

    public static function getCardsByIbAccount($ib_account_id, $states = array()) {
        if (empty($ib_account_id))
            $cards = Db::selectMulti('*', 'card', array(), 'state', $states, 'ORDER BY id DESC');
        else {
            $cards = Db::queryMulti('SELECT * FROM card WHERE disponent IN (SELECT id FROM disponent WHERE ib_account = ?) ORDER BY id DESC', array($ib_account_id));
            if (!empty($states)) {
                foreach ($cards as $key => $card) {
                    if (!in_array($card['state'], $states))
                        unset($cards[$key]);
                }
            }
        }
        return $cards;
    }

    public static function getCardsByDisponent($disponent_id, $states = array()) {
        $conditions = empty($disponent_id) ? array() : array('disponent' => $disponent_id);
        return Db::selectMulti('*', 'card', $conditions, 'state', $states, 'ORDER BY id DESC');
    }

    public static function deleteCard($id) {
        return Db::query('DELETE FROM card WHERE id = ?', array($id));
    }

    public static function renderCardHref($card_id) {
        $builder = new HtmlBuilder();
        $card = self::getCard($card_id);
        $title = self::$card_states[$card['state']];
        if ($card['state'] == 'ACTIVE') $mark = '&check; '; else $mark = '&cross; ';
        $number = is_null($card['number']) ? 'Bez čísla' : $card['number'];
        $href = $card['state'] == 'PENDING' || $card['state'] == 'MAKING' ? '/pending-requests/card/' . $card_id : '/database/detail/card/' . $card_id;
        $builder->addValueElement('a', $mark . $number, array('title' => $title, 'href' => $href), true);
        return $builder->render();
    }

    public static function renderCardsOfBankAccount($bank_account_id, $states = array()) {
        $cards = self::getCardsOfBankAccount($bank_account_id);
        if (!empty($states)) {
            foreach ($cards as $key => $value) {
                if (!in_array($value['state'], $states))
                    unset($cards[$key]);
            }
        }
        $cards = ArrayUtils::mapSingles($cards, 'id');
        foreach ($cards as $key => $value)
            $cards[$key] = self::renderCardHref($value);
        return empty($cards) ? '---' : implode(', ', $cards);
    }

    /**
     * @param $cards
     * @param $show_date
     * @param $show_number
     * @param $show_disponent 1 = full name, 2 = +identification, 3 = +state warning +ib link
     * @param $show_account
     * @param $show_state 1 = as card, 2 = as request
     * @param $show_expires
     * @param $show_verified_by 1 = full name, 2 = +identification, 3 = +state warning +ib link
     * @param $show_verified_when
     * @param $show_verified_comment
     * @param $show_req_delete
     * @param $show_req_detail
     * @param string $return
     * @return string
     */
    public static function renderCardsAsTable($cards, $show_date, $show_number, $show_disponent, $show_account, $show_state,
                                              $show_expires, $show_verified_by, $show_verified_when, $show_verified_comment,
                                              $show_req_delete, $show_req_detail, $return = '') {
        if (!$cards) return '';

        if (!empty($return))
            $return = '?return=' . $return;

        $builder = new HtmlBuilder();
        $builder->startElement('table', array('class' => 'table'));

        // table header
        $builder->startElement('tr');
        if ($show_date)
            $builder->addValueElement('th', 'Vytvořeno');
        if ($show_number)
            $builder->addValueElement('th', 'Číslo karty');
        if ($show_disponent)
            $builder->addValueElement('th', 'Klient');
        if ($show_account)
            $builder->addValueElement('th', 'K účtu');
        if ($show_state === true || $show_state === 1)
            $builder->addValueElement('th', 'Stav karty');
        if ($show_state === 2)
            $builder->addValueElement('th', 'Stav žádosti');
        if ($show_expires)
            $builder->addValueElement('th', 'Platnost');
        if ($show_verified_by)
            $builder->addValueElement('th', 'Vyřídili');
        if ($show_verified_when)
            $builder->addValueElement('th', 'Kdy');
        if ($show_verified_comment)
            $builder->addValueElement('th', 'Komentáře');
        if ($show_req_delete)
            $builder->addValueElement('th', '', array('class' => 'hide'));
        if ($show_req_detail)
            $builder->addValueElement('th', '', array('class' => 'hide'));
        $builder ->endElement(); // tr

        // table rows
        foreach ($cards as $card) {
            $builder->startElement('tr');

            if ($show_date)
                $builder->addValueElement('td', DateUtils::getPrettyDateTime($card['inserted']));
            if ($show_number)
                $builder->addValueElement('td', is_null($card['number']) || $card['number'] == 0 ? '< NEPŘIŘAZENO >' : $card['number']);
            if ($show_disponent != false)
                $builder->addValueElement('td', AuthManager::renderClientOneLiner(AuthManager::getIbAccountIdByDisponentId($card['disponent']), $show_disponent >= 2, $show_disponent >= 3, $show_disponent >= 3), array(), true);
            if ($show_account) {
                $acc = AccountManager::getBankAccountOfCard($card['id']);
                $builder->addValueElement('td', $acc['number'] . ' (' . AccountManager::getBankAccountNameByDisponentId($card['disponent']) . ')');
            }
            if ($show_state === true || $show_state === 1)
                $builder->addValueElement('td', self::$card_states[$card['state']]);
            if ($show_state === 2)
                $builder->addValueElement('td', self::$request_states[$card['state']]);
            if ($show_expires)
                $builder->addValueElement('td', is_null($card['expires']) ? '---' : DateUtils::getPrettyDate($card['expires']));
            if ($show_verified_by != false) {
                $builder->startElement('td');
                $builder->addValue(is_null($card['verified_by1']) ? '---' : AuthManager::renderClientOneLiner(AuthManager::getIbAccountIdByEmployeeId($card['verified_by1']), $show_verified_by === 2, $show_verified_by === 3, $show_verified_by === 3), true);
                $builder->addElement('br');
                $builder->addValue(is_null($card['verified_by2']) ? '---' : AuthManager::renderClientOneLiner(AuthManager::getIbAccountIdByEmployeeId($card['verified_by2']), $show_verified_by === 2, $show_verified_by === 3, $show_verified_by === 3), true);
                $builder->endElement();
            }
            if ($show_verified_when) {
                $builder->startElement('td');
                $builder->addValue(is_null($card['verified_when1']) ? '---' : DateUtils::getPrettyDateTime($card['verified_when1']));
                $builder->addElement('br');
                $builder->addValue(is_null($card['verified_when2']) ? '---' : DateUtils::getPrettyDateTime($card['verified_when2']));
                $builder->endElement();
            }
            if ($show_verified_comment) {
                $builder->startElement('td');
                $builder->addValue(is_null($card['verified_comment1']) ? '---' : $card['verified_comment1'], true);
                $builder->addElement('br');
                $builder->addValue(is_null($card['verified_comment2']) ? '---' : $card['verified_comment2'], true);
                $builder->endElement();
            }
            if ($show_req_delete && $card['state'] == 'PENDING') {
                $builder->startElement('td', array('class' => 'hide'));
                $builder->addValueElement('a', '', array('title' => 'Odstranit', 'class' => 'delete_btn', 'href' => '/delete-request/card/' . $card['id'] . $return));
                $builder->endElement(); // td
            }
            if ($show_req_detail) {
                $builder->startElement('td', array('class' => 'hide'));
                $builder->addValueElement('a', '', array('title' => 'Zobrazit detail', 'class' => 'view_btn', 'href' => '/request-detail/card/' . $card['id']));
                $builder->endElement(); // td
            }

            $builder ->endElement(); // tr
        }

        $builder->endElement(); // table
        return $builder->render();
    }

    /**
     * @param $card
     * @param $show_date
     * @param $show_number
     * @param $show_disponent 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_account
     * @param $show_state 1 = as card, 2 = as request
     * @param $show_expires
     * @param $show_verified_by 1 = full name, 2 = +identification, 3 = +state warning
     * @param $show_verified_when
     * @param $show_verified_comment
     * @return string
     */
    public static function renderCard($card, $show_date, $show_number, $show_disponent, $show_account, $show_state,
                                      $show_expires, $show_verified_by, $show_verified_when, $show_verified_comment) {
        if (!$card) return '';

        $builder = new HtmlBuilder();
        $builder->startElement('dl', array('class' => 'detail'));

        if ($show_date) {
            $builder->addValueElement('dt', 'Vytvořeno');
            $builder->addValueElement('dd', DateUtils::getPrettyDateTime($card['inserted']));
        }
        if ($show_number) {
            $builder->addValueElement('dt', 'Číslo karty');
            $builder->addValueElement('dd', is_null($card['number']) || $card['number'] == 0 ? '< NEPŘIŘAZENO >' : $card['number']);
        }
        if ($show_disponent != false) {
            $builder->addValueElement('dt', 'Vlastník');
            $builder->addValueElement('dd', AuthManager::renderClientOneLiner(AuthManager::getIbAccountIdByDisponentId($card['disponent']), $show_disponent === 2, $show_disponent === 3, $show_disponent === 3), array(), true);
        }
        if ($show_account) {
            $builder->addValueElement('dt', 'K účtu');
            $acc = AccountManager::getBankAccountOfCard($card['id']);
            $builder->addValueElement('dd', $acc['number'] . ' (' . AccountManager::getBankAccountNameByDisponentId($card['disponent']) . ')');
        }
        if ($show_state === true || $show_state === 1) {
            $builder->addValueElement('dt', 'Stav karty');
            $builder->addValueElement('dd', self::$card_states[$card['state']]);
        }
        if ($show_state === 2) {
            $builder->addValueElement('dt', 'Stav žádosti');
            $builder->addValueElement('dd', self::$request_states[$card['state']]);
        }
        if ($show_expires) {
            $builder->addValueElement('dt', 'Vyprší');
            $builder->addValueElement('dd', is_null($card['expires']) ? '---' : DateUtils::getPrettyDate($card['expires']));
        }
        if ($show_verified_by != false) {
            $builder->addValueElement('dt', 'Vyřídili');
            $builder->startElement('dd');
            $builder->addValue( is_null($card['verified_by1']) ? '---' : AuthManager::renderClientOneLiner(AuthManager::getIbAccountIdByEmployeeId($card['verified_by1']), $show_verified_by === 2, $show_verified_by === 3, $show_verified_by === 3), true);
            $builder->addElement('br');
            $builder->addValue( is_null($card['verified_by2']) ? '---' : AuthManager::renderClientOneLiner(AuthManager::getIbAccountIdByEmployeeId($card['verified_by2']), $show_verified_by === 2, $show_verified_by === 3, $show_verified_by === 3), true);
            $builder->endElement();
        }
        if ($show_verified_when) {
            $builder->addValueElement('dt', 'Kdy');
            $builder->startElement('dd');
            $builder->addValue( is_null($card['verified_when1']) ? '---' : DateUtils::getPrettyDateTime($card['verified_when1']));
            $builder->addElement('br');
            $builder->addValue( is_null($card['verified_when2']) ? '---' : DateUtils::getPrettyDateTime($card['verified_when2']));
            $builder->endElement();
        }
        if ($show_verified_comment) {
            $builder->addValueElement('dt', 'Komentáře');
            $builder->startElement('dd');
            $builder->addValue( is_null($card['verified_comment1']) ? '---' : $card['verified_comment1'], true);
            $builder->addElement('br');
            $builder->addValue( is_null($card['verified_comment2']) ? '---' : $card['verified_comment2'], true);
            $builder->endElement();
        }

        $builder->endElement(); // dl

        return $builder->render();
    }

}