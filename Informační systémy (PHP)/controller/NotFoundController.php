<?php

class NotFoundController extends AbstractController {

    public function execute($params) {
        $this->addMessage(new Message('Požadovaná stránka nebyla nalezena', Message::TYPE_ERROR));
        $this->restrictAccess(false, false, false, false);
    }
}
