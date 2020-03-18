<?php
class IbClientController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(false, true, false, false);

        $this->header['title'] = 'Internetové bankovnictví';
        $this->header['keywords'] = 'internet, banking';
        $this->header['description'] = 'Klientský internet banking';

        // Account requests
        try {
            $accounts = AccountManager::getBankAccountsByFilter($_SESSION['auth']['id']);
            $this->data['account_requests'] = AccountManager::renderBankAccountRequestsAsTable($accounts, true,
                false, true, true, false, false,
                true, true, false, 'ib-client');
        } catch (PDOException $e) {
            $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            $this->data['account_requests'] = false;
        }

        // Card requests
        try {
            $cards = CardManager::getCardsByIbAccount($_SESSION['auth']['id']);
            $this->data['card_requests'] = CardManager::renderCardsAsTable($cards, true, false,
                false, true, 2, false, false,
                false, true, true, false, 'ib-client');
        } catch (PDOException $e) {
            $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            $this->data['card_requests'] = false;
        }

        // Disponent requests
        try {
            $disponents = DisponentManager::getDisponentsOfClientWhoRequestedThem($_SESSION['auth']['id']);
            $this->data['disponent_requests'] = DisponentManager::renderDisponentsAsTable($disponents, true,
                false, true, 2, 2, false, false,
            true, true, false, 'ib-client');
        } catch (PDOException $e) {
            $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            $this->data['disponent_requests'] = false;
        }



        $this->data['no_requests'] = (!$this->data['account_requests'] && !$this->data['card_requests'] && !$this->data['disponent_requests']);

        $this->view = 'ibclient';
    }
}
