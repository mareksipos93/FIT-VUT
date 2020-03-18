<?php
class LostCredentialsController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(true, false, false, false);

        $this->header['title'] = 'Obnova údajů';
        $this->header['keywords'] = 'obnova, ztracene, udaje';
        $this->header['description'] = 'Obnovení přihlašovacích údajů k internetovému bankovnictví';

        $form = new FormFFW('lostcredentials');
        $f_email = $form->addEmailBox('email', 'E-mail', true);
        $form->addButton('recover', 'Obnovit údaje');

        if ($_POST) {
            try {
                $formData = $form->getData();

                $ib_acc = AuthManager::getIbAccount(-1, '', $formData['email']);
                if (!$ib_acc) {
                    $f_email->addClass('invalid');
                    throw new CustomException('K této e-mailové adrese není přiřazen žádný aktivní účet.');
                }
                if ($ib_acc['state'] == 'DEACTIVATED') {
                    throw new CustomException('Tento účet je deaktivován.');
                }

                AuthManager::createChangePwCodeByAccountId($ib_acc['id']);
                AuthManager::sendLostCredentialsEmail($formData['email']);

                $this->addMessage(new Message('Na vámi zadanou e-mailovou adresu byly odeslány pokyny pro obnovení účtu.', Message::TYPE_SUCCESS));
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }
        }

        $this->data['form'] = $form;
        $this->view = 'lostcredentials';
    }
}
