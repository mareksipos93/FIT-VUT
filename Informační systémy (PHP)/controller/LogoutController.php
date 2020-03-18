<?php
class LogoutController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(false, true, true, true);

        AuthManager::logOut();
        $this->addMessage(new Message('Úspěšně odhlášeno. Děkujeme za návštěvu!', Message::TYPE_SUCCESS));
        $this->redirect('intro');
    }
}
