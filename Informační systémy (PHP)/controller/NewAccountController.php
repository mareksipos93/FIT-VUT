<?php
class NewAccountController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(false, true, false, false);

        $this->header['title'] = 'Nový bankovní účet';
        $this->header['keywords'] = 'nový, bankovní, účet';
        $this->header['description'] = 'Otevření nového bankovního účtu';

        $types = AccountManager::getOfferedAccountTypes();
        if (!$types || empty($types)) {
            $this->addMessage(new Message('Nebyly nalezeny žádné typy účtů', Message::TYPE_ERROR));
            $this->redirect('intro');
        }

        $types_array = array();
        foreach ($types as $type) {
            $types_array[$type['name'] . ' - Měsíční poplatek: ' . StringUtils::formatMoney($type['monthly_fee']) . ' Kč - Roční úroková sazba: ' . $type['interest'] . ' %'] = $type['id'];
        }

        $form = new FormFFW('newaccount');
        $form->addComboBox('type', 'Typ účtu', true)
                ->setValues($types_array);
        $form->addButton('send', 'Odeslat');

        if ($_POST) {
            try {
                $formData = $form->getData();

                if ($_SESSION['auth']['role'] == 'CLIENT') {
                    AccountManager::newAccountRequest($formData['type'], $_SESSION['auth']['id']);

                    $this->addMessage(new Message('Žádost o založení účtu byla odeslána!', Message::TYPE_SUCCESS));
                    $this->addMessage(new Message('Náš pracovník se žádosti bude brzy věnovat.<br />Oznámení o vyřízení 
                                                        vám odešleme jako zprávu zde v internetovém bankovnictví.', Message::TYPE_INFO));
                    $this->redirect('intro');
                }

                // TODO - přímá tvorba účtu zaměstnancem

            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }
        }


        $this->data['form'] = $form;
        $this->view = 'newaccount';
    }
}
