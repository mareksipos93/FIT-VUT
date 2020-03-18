<?php
class ResetPasswordController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(true, false, false, false);

        $this->header['title'] = 'Obnovení hesla';
        $this->header['keywords'] = 'heslo, obnova, účet';
        $this->header['description'] = 'Obnovení hesla k účtu internetového bankovnictví';

        $account = false;
        if ($_GET && isset($_GET['code'])) {
            try {
                $account = AuthManager::getIbAccount(-1, '', '','' , $_GET['code']);
                if (!$account) {
                    throw new CustomException('Odkaz na obnovu hesla byl neplatný.');
                }
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                $this->redirect('intro');
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                $this->redirect('intro');
            }

            $form = new FormFFW('resetpw');
            $f_pw1 = $form->addPasswordBox('password', 'Nové heslo', true);
            $f_pw2 = $form->addPasswordBox('password_again', 'Nové heslo znovu', true);
            $form->addButton('changepw', 'Změnit heslo');

            if ($_POST) {
                try {
                    $formData = $form->getData();

                    // Passwords don't match
                    if ($formData['password'] != $formData['password_again']) {
                        $f_pw1->addClass('invalid');
                        $f_pw2->addClass('invalid');
                        throw new CustomException('Zadaná hesla se neshodují');
                    }

                    AuthManager::changePasswordUsingCode($_GET['code'], $formData['password']);

                    $this->addMessage(new Message('Heslo bylo úspěšně změněno!', Message::TYPE_SUCCESS));
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
            $this->data['acc'] = $account['name'];
            $this->view = 'resetpassword';
        } else {
            $this->redirect('intro');
        }



    }
}
