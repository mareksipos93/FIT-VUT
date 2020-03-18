<?php
class PendingRequestsController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(false, false, true, true);
        $this->needAtLeastParams($params, 1, 'pending-requests/person');

        $this->header['title'] = 'Čekající žádosti';
        $this->header['keywords'] = 'žádosti, čekající';
        $this->header['description'] = 'Seznam žádostí k vyřízení';

        $this->data['additional_text'] = '';

        // Person requests
        if ($params[0] == 'person') {
            $this->data['h1'] = 'Kontrola osobních údajů';
            $this->data['additional_text'] = 'Věnujte pozornost tomu, zda má klient alespoň jeden ověřený IB účet. Postupujte podle příslušných firemních směrnic.';
            try {
                $persons = AuthManager::getPersonsByFilter(array('PENDING'));
                if (!$persons)
                    $this->data['requests'] = 'Žádné nevyřízené žádosti';
                else
                    $this->data['requests'] = AuthManager::renderPersonRequestsAsTable($persons, true, false,
                    2, true, true, false, false,
                    false, false, true, true, 'pending-requests/person');
            } catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                $this->data['requests'] = 'Nastala chyba';
            }
        }

        // Account requests
        else if ($params[0] == 'bank-account') {
            $this->data['h1'] = 'Žádosti o bankovní účty';
            try {
                $accounts = AccountManager::getBankAccountsByFilter('', array('PENDING'));
                if (!$accounts)
                    $this->data['requests'] = 'Žádné nevyřízené žádosti';
                else
                    $this->data['requests'] = AccountManager::renderBankAccountRequestsAsTable($accounts, true,
                    3, true, false, false, false,
                    false, true, true, 'pending-requests/bank-account');
            } catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                $this->data['requests'] = 'Nastala chyba';
            }
        }

        // Card requests
        else if ($params[0] == 'card') {
            $this->data['h1'] = 'Žádosti o karty';
            try {
                $cards = CardManager::getCardsByIbAccount('', array('PENDING', 'MAKING'));
                if (!$cards)
                    $this->data['requests'] = 'Žádné nevyřízené žádosti';
                else
                    $this->data['requests'] = CardManager::renderCardsAsTable($cards, true, false,
                    3, true, 2, false, false,
                    false, false, true, true, 'pending-requests/card');
            } catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                $this->data['requests'] = 'Nastala chyba';
            }
        }

        // Disponent requests
        else if ($params[0] == 'disponent') {
            $this->data['h1'] = 'Žádosti o disponenty';
            try {
                $disponents = DisponentManager::getDisponentsOfBankAccount('', array('AGREED'));
                if (!$disponents)
                    $this->data['requests'] = 'Žádné nevyřízené žádosti';
                else
                    $this->data['requests'] = DisponentManager::renderDisponentsAsTable($disponents, true,
                    3, true, 3, false, false,
                    false, false, true, true, 'pending-requests/disponent');
            } catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                $this->data['requests'] = 'Nastala chyba';
            }
        }
        else
            $this->redirect('pending-requests');


        $this->view = 'pendingrequests';

    }
}