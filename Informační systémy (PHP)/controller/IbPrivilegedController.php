<?php
class IbPrivilegedController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(false, false, true, true);

        if ($this->data['is_employee']) {
            $this->header['title'] = 'Panel zaměstnance';
            $this->header['keywords'] = 'panel, zaměstnanec';
            $this->header['description'] = 'Hlavní ovládací panel zaměstnance';
        } else {
            $this->header['title'] = 'Panel administrátora';
            $this->header['keywords'] = 'panel, administrátor';
            $this->header['description'] = 'Hlavní ovládací panel administrátora';
        }

        try {
            $this->data['pending_persons'] = AuthManager::getAmountOfPendingPersons();
            $this->data['pending_bank_accounts'] = AccountManager::getAmountOfPendingBankAccounts();
            $this->data['pending_cards'] = CardManager::getAmountOfPendingOrMakingCards();
            $this->data['pending_disponents'] = DisponentManager::getAmountOfAgreedDisponents();
        } catch (PDOException $e) {
            $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            $this->data['pending_persons'] = '?';
            $this->data['pending_bank_accounts'] = '?';
            $this->data['pending_cards'] = '?';
            $this->data['pending_disponents'] = '?';
        }

        $this->view = 'ibprivileged';
    }
}
