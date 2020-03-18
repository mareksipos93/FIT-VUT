<?php
class SettingsController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(false, true, true, true);

        $this->header['title'] = 'Nastavení';
        $this->header['keywords'] = 'nastavení, změna, údajů';
        $this->header['description'] = 'Nastavení IB účtu a osobních údajů';

        $ib_account = false;
        $person = false;

        try {
            $ib_account = AuthManager::getIbAccount($_SESSION['auth']['id']);
            $person = AuthManager::getPersonByIbAccountId($_SESSION['auth']['id']);
        }
        catch (PDOException $e) {
            $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            $this->redirect('intro');
        }

        $change_email = new FormFFW('change_email');
        $f_email1 = $change_email->addEmailBox('email1', 'Nový e-mail', true)
            ->addMaxLengthRule(255, false);
        $f_email2 = $change_email->addEmailBox('email2', 'Nový e-mail znovu', true)
            ->addMaxLengthRule(255, false);
        $change_email->addButton('btn_change_email', 'Změnit e-mail');

        $change_password = new FormFFW('change_password');
        $change_password->addHiddenBox('name', $ib_account['name'], false, array('autocomplete' => 'login'));
        $f_pw1 = $change_password->addPasswordBox('password', 'Nové heslo', true);
        $f_pw2 = $change_password->addPasswordBox('password_again', 'Nové heslo znovu', true);
        $change_password->addButton('btn_change_password', 'Změnit heslo');

        $change_personal = new FormFFW('change_personal');
        /** @noinspection PhpUndefinedMethodInspection */
        $change_personal->addTextBox('first_name', 'Jméno', true, array('autocomplete' => 'given-name'))
            ->addMinLengthRule(2, false)->addMaxLengthRule(255, false)->setText($person['first_name']);
        /** @noinspection PhpUndefinedMethodInspection */
        $change_personal->addTextBox('second_name', 'Příjmení', true, array('autocomplete' => 'family-name'))
            ->addMinLengthRule(2, false)->addMaxLengthRule(255, false)->setText($person['second_name']);
        /** @noinspection PhpUndefinedMethodInspection */
        $f_id = $change_personal->addTextBox('identification', 'Číslo občanského průkazu', true)
            ->addMaxLengthRule(200, false)->setText(AuthManager::trimIdentification($person['identification']));
        $change_personal->addComboBox('country', 'Občanství', true)
            ->setValues(array_flip(AuthManager::$country_codes))->setSelectedValue($person['country_code']);
        /** @noinspection PhpUndefinedMethodInspection */
        $change_personal->addTextBox('address', 'Trvalé bydliště', true)
            ->addMaxLengthRule(1024, false)->setText($person['address']);
        $change_personal->addDatePicker('birthdate', 'Datum narození', true)->setValue(DateUtils::getDateTime($person['birthdate'])->format(DateUtils::CZE_DATE_FORMAT));
        $change_personal->addButton('btn_change_personal', 'Upravit osobní údaje');

        $this->data['change_email'] = $change_email;
        $this->data['change_password'] = $change_password;
        $this->data['change_personal'] = $change_personal;

        if ($_POST) {
            try {
                if ($change_email->isPostBack()) {
                    $formData = $change_email->getData();

                    // E-mails do not match
                    if ($formData['email1'] != $formData['email2']) {
                        $f_email1->addClass('invalid');
                        $f_email2->addClass('invalid');
                        throw new CustomException('Zadané e-maily se neshodují');
                    }

                    // E-mail exists
                    if (AuthManager::emailExists($formData['email1'])) {
                        $f_email1->addClass('invalid');
                        $f_email2->addClass('invalid');
                        throw new CustomException('Tuto e-mailovou adresu již někdo používá');
                    }

                    // Change e-mail
                    AuthManager::createVerificationCodeByAccountId($ib_account['id']);
                    AuthManager::sendVerificationEmail($formData['email1'], $ib_account['name']);
                    AuthManager::changeIbAccountState($ib_account['id'], 'PENDING');
                    AuthManager::changeEmail($ib_account['id'], $formData['email1']);
                    $this->addMessage(new Message('E-mail byl změněn!', Message::TYPE_SUCCESS));
                    $this->addMessage(new Message('Na e-mail vám byla zaslána zpráva s pokyny pro aktivaci nového e-mailu!', Message::TYPE_INFO));
                }

                else if ($change_password->isPostBack()) {
                    $formData = $change_password->getData();

                    // Passwords do not match
                    if ($formData['password'] != $formData['password_again']) {
                        $f_pw1->addClass('invalid');
                        $f_pw2->addClass('invalid');
                        throw new CustomException('Zadaná hesla se neshodují');
                    }

                    // Change password
                    AuthManager::changePassword($ib_account['id'], $formData['password']);
                    $this->addMessage(new Message('Heslo bylo úspěšně změněno!', Message::TYPE_SUCCESS));
                }

                else if ($change_personal->isPostBack()) {
                    $formData = $change_personal->getData();
                    
                    $params = array();
                    if ($person['identification'] != $person['id'] . '_' . $formData['identification']) {
                        // Person exists
                        if (AuthManager::personExists($formData['identification'])) {
                            $f_id->addClass('invalid');
                            throw new CustomException('Tato osoba již v systému existuje');
                        }
                        $params['identification'] = $person['id'] . '_' . $formData['identification'];
                    }
                    if ($person['first_name'] != $formData['first_name'])
                        $params['first_name'] = $formData['first_name'];
                    if ($person['second_name'] != $formData['second_name'])
                        $params['second_name'] = $formData['second_name'];
                    if ($person['country_code'] != $formData['country'])
                        $params['country_code'] = $formData['country'];
                    if ($person['first_name'] != $formData['first_name'])
                        $params['first_name'] = $formData['first_name'];
                    if ($person['address'] != $formData['address'])
                        $params['address'] = $formData['address'];
                    if ($person['birthdate'] != $formData['birthdate'])
                        $params['birthdate'] = $formData['birthdate'];

                    // Update values
                    if (!empty($params)) {
                        AuthManager::changePersonState($person['id'], 'PENDING');
                        Db::update('person', $params, 'WHERE id = ?', array($person['id']));
                        $this->addMessage(new Message('Osobní údaje upraveny!', Message::TYPE_SUCCESS));
                        $this->addMessage(new Message('Náš pracovník zadané údaje v brzké době ověří.<br />Do té doby mohou být některé funkce omezeny.', Message::TYPE_INFO));
                    } else
                        $this->addMessage(new Message('Nic nebylo upraveno', Message::TYPE_INFO));
                }

            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }
        }
        
        $this->view = 'settings';
    }
}