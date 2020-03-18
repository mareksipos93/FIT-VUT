<?php
class IntroController extends AbstractController {

    public function execute($params){
        $this->restrictAccess(true, false, false, false);

        $this->header['keywords'] = 'simple, bank';
        $this->header['description'] = 'Simple Bank - Banking made simple';

        $this->view = 'intro';
    }
}
