<?php
class ActivationController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(true, true, true, true);

        $this->header['title'] = 'Aktivace';
        $this->header['keywords'] = 'aktivace, potvrzení, účet';
        $this->header['description'] = 'Aktivace e-mailové adresy';

        $form = new FormFFW('activation');
        $f_code = $form->addTextBox('code', 'Aktivační kód', true);
        $form->addButton('activate', 'Ověřit');

        $specifiedCode = false;
        if ($_POST) {
            try {
                $formData = $form->getData();
                $specifiedCode = $formData['code'];
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
            }
        } else if ($_GET) {
            $_SESSION['GET_activation'] = $_GET['code'];
            $this->redirect('activation');
        } else if (isset($_SESSION['GET_activation'])) {
            $specifiedCode = $_SESSION['GET_activation'];
            unset($_SESSION['GET_activation']);
        }

        if ($specifiedCode) {
            try {
                $f_code->setText($specifiedCode);

                $result = AuthManager::activateAccountUsingCode($specifiedCode);
                if (!$result) {
                    $f_code->addClass('invalid');
                    throw new CustomException('Aktivační kód není platný.');
                }

                if (!AuthManager::isLoggedIn())
                    AuthManager::tryLogin($result['name'], $result['password']);

                $this->addMessage(new Message('Ověření proběhlo úspěšně!', Message::TYPE_SUCCESS));
                $p = AuthManager::getPersonByIbAccountId($_SESSION['auth']['id']);
                if ($p['state'] == 'PENDING')
                    $this->addMessage(new Message('V brzké době náš pracovník ověří zadané osobní údaje a zpřístupní vám veškeré funkce.', Message::TYPE_INFO));
                NotificationManager::createNotificationForIbId($_SESSION['auth']['id'], <<<'EOT'
Dobrý den,<br />
<br />
vaše e-mailová adresa byla ověřena. Děkujeme!<br />
Pokud budete mít jakékoliv dotazy, neváhejte a obraťte se na naši infolinku.<br />
<br />
<em>Vaše Simple Bank</em>
EOT
);
                $this->redirect('intro');
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }
        }

        $this->data['form'] = $form;
        $this->view = 'activation';
    }
}
