<?php
class LoginController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(true, false, false, false);

        $this->header['title'] = 'Přihlášení';
        $this->header['keywords'] = 'prihlaseni, vstup';
        $this->header['description'] = 'Přihlášení k účtu internetového bankovnictví';

        $form = new FormFFW('log_in');
        $form->addTextBox('name', 'Uživatelské jméno', true, array('autocomplete' => 'login'))
            ->addMinLengthRule(4, false)->addMaxLengthRule(50, false);
        $form->addPasswordBox('password', 'Heslo', true);
        $form->addButton('login', 'Přihlásit');

        if ($_POST) {
            try {
                $formData = $form->getData();

                AuthManager::tryLogin($formData['name'], $formData['password']);

                $this->addMessage(new Message('Přihlášení bylo úspěšné!', Message::TYPE_SUCCESS));
                $this->redirect('intro'); // restrict access from intro redirects to correct page
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }
        }

        $this->data['form'] = $form;
        $this->view = 'login';
    }
}
