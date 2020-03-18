<?php
class SendMessageController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(false, false, true, true);

        $form = new FormFFW('send_message');
        $field_person = $form->addTextBox('person', 'Název IB účtu osoby', true);
        $field_message = $form->addTextArea('message', 'Zpráva', true, array('placeholder' => "Podporuje HTML tagy."));
        $form->addButton('send', 'Odeslat');

        $this->data['form'] = $form;

        if ($_POST) {
            try {
                $formData = $form->getData();

                $person = AuthManager::getIbAccount(-1, $formData['person']);
                if (!$person) {
                    $field_person->addClass('invalid');
                    throw new CustomException('Tato osoba nemá účet.');
                }
                $formData['message'] = trim($formData['message']);
                if (empty($formData['message'])) {
                    $field_message->addClass('invalid');
                    throw new CustomException('Musíte zadat text zprávy.');
                }

                NotificationManager::createNotificationForIbId($person['id'], nl2br($formData['message']));
                $this->addMessage(new Message('Zpráva úspěšně odeslána!', Message::TYPE_SUCCESS));
                $this->redirect('send-message');
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }
        }

        $this->view = 'sendmessage';
    }
}