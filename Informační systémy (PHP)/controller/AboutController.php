<?php
class AboutController extends AbstractController {

    public function execute($params) {
        $this->header['title'] = 'O projektu';
        $this->header['keywords'] = 'projekt, informace, IIS';
        $this->header['description'] = 'Informace o projektu Simple Bank';

        $this->view = 'about';
    }
}