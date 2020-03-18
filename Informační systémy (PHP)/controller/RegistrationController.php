<?php
class RegistrationController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(true, false, false, false);

        $this->header['title'] = 'Registrace';
        $this->header['keywords'] = 'registrace, tvorba, účet';
        $this->header['description'] = 'Registrace nového klienta';

        $form = new FormFFW('registration');
        $f_name = $form->addTextBox('name', 'Uživatelské jméno', true, array('autocomplete' => 'login'))
            ->addMinLengthRule(4, false)->addMaxLengthRule(50, false);
        $f_pw1 = $form->addPasswordBox('password', 'Heslo', true);
        $f_pw2 = $form->addPasswordBox('password_again', 'Heslo znovu', true);
        $f_email = $form->addEmailBox('email', 'E-mail', true)
            ->addMaxLengthRule(255, false);
        $form->addTextBox('first_name', 'Jméno', true, array('autocomplete' => 'given-name'))
            ->addMinLengthRule(2, false)->addMaxLengthRule(255, false);
        $form->addTextBox('second_name', 'Příjmení', true, array('autocomplete' => 'family-name'))
            ->addMinLengthRule(2, false)->addMaxLengthRule(255, false);
        $f_id = $form->addTextBox('identification', 'Číslo občanského průkazu', true)
            ->addMaxLengthRule(200, false);
        $form->addComboBox('country', 'Občanství', true)
            ->setValues(array_flip(AuthManager::$country_codes));
        $form->addTextBox('address', 'Trvalé bydliště', true)
            ->addMaxLengthRule(1024, false);
        $form->addDatePicker('birthdate', 'Datum narození', true);
        $form->addButton('register', 'Registrovat');

        if ($_POST) {
            try {
                $formData = $form->getData();

                // Passwords don't match
                if ($formData['password'] != $formData['password_again']) {
                    $f_pw1->addClass('invalid');
                    $f_pw2->addClass('invalid');
                    throw new CustomException('Zadaná hesla se neshodují');
                }

                // IB account exists
                if (AuthManager::isRegistered($formData['name'])) {
                    $f_name->addClass('invalid');
                    throw new CustomException('Uživatel se jménem ' . $formData['name'] . ' je již registrován');
                }

                // Person exists
                if (AuthManager::personExists($formData['identification'])) {
                    $f_id->addClass('invalid');
                    throw new CustomException('Tato osoba již v systému existuje');
                }

                // Email exists
                if (AuthManager::emailExists($formData['email'])) {
                    $f_email->addClass('invalid');
                    throw new CustomException('Tuto e-mailovou adresu již někdo používá');
                }

                // Insert person
                $new_id = AuthManager::insertPerson($formData['first_name'], $formData['second_name'],
                    $formData['identification'], $formData['country'],
                    $formData['address'], $formData['birthdate']);

                // Insert IB account
                AuthManager::insertIbAccount($formData['name'], password_hash($formData['password'], PASSWORD_DEFAULT),
                    $new_id, $formData['email']);

                // Send verification e-mail
                AuthManager::sendVerificationEmail($formData['email'], $formData['name']);

                $this->addMessage(new Message('Registrace byla úspěšná!', Message::TYPE_SUCCESS));
                $this->addMessage(new Message('Na vámi zadanou e-mailovou adresu byl odeslán potvrzovací kód.', Message::TYPE_INFO));
                $this->redirect('activation');
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }
        }

        $this->data['form'] = $form;
        $this->view = 'registration';
    }
}
