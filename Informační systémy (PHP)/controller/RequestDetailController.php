<?php
class RequestDetailController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(false, false, true, true);
        $this->needAtLeastParams($params, 2, 'pending-requests');

        $this->header['title'] = 'Detail žádosti';
        $this->header['keywords'] = 'detail, informace, žádost';
        $this->header['description'] = 'Podrobnosti o žádosti';

        $this->data['form'] = false;
        $this->data['additional_text'] = '';
        $comment = null;
        $commentmsg = '';

        // Show person request
        if ($params[0] == 'person') {

            $this->data['h1'] = 'Detail žádosti o ověření údajů osoby';
            $person = false;

            // Load person from DB
            try {
                $person = AuthManager::getPerson($params[1]);

                if (!$person || empty($person))
                    throw new CustomException('Žádost neexistuje');
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                $this->redirect('pending-requests/person');
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                $this->redirect('pending-requests/person');
            }

            $form = new FormFFW('req_form');
            $form->addTextArea('comment', 'Komentář (zobrazí se uživateli)', false, array('placeholder' => "Podporuje HTML tagy."));
            $form->addButton('btn_accept', 'Potvrdit správnost údajů');
            $form->addButton('btn_reject', 'Údaje nesouhlasí');

            // Processing
            if ($_POST) {
                try {
                    $formData = $form->getData();
                    if (isset($formData['comment']) && !empty($formData['comment'])) {
                        $comment = $formData['comment'];
                        $commentmsg = '<br /><br />Zaměstnanec přidal tento komentář:<br />' . $comment;
                    }

                    if ($form->getSentButton() == 'btn_accept' && $person['state'] == 'PENDING') {

                        AuthManager::verifyPerson($person['id'], 'VERIFIED', $_SESSION['auth']['id'], $comment);
                        $this->addMessage(new Message('Údaje úspěšně ověřeny!', Message::TYPE_SUCCESS));

                        $person = AuthManager::getPerson($person['id']);

                        $msg = 'Vaše osobní údaje byly úspěšně ověřeny zaměstnancem!<br />V případě jakýchkoliv dotazů kontaktujte naši infolinku!' . $commentmsg;
                        foreach (AuthManager::getIbAccountsOfPerson($person['id']) as $ib_acc)
                            NotificationManager::createNotificationForIbId($ib_acc['id'], $msg);
                    }
                    if ($form->getSentButton() == 'btn_reject' && $person['state'] == 'PENDING') {

                        AuthManager::verifyPerson($person['id'], 'INVALID', $_SESSION['auth']['id'], $comment);
                        $this->addMessage(new Message('Údaje označeny jako neplatné!', Message::TYPE_SUCCESS));

                        $person = AuthManager::getPerson($person['id']);

                        $msg = 'Vaše osobní údaje byly zaměstnancem označeny jako neplatné.<br />Přejděte prosím do <a href="/settings">nastavení</a> a údaje opravte.' . $commentmsg;
                        foreach (AuthManager::getIbAccountsOfPerson($person['id']) as $ib_acc)
                            NotificationManager::createNotificationForIbId($ib_acc['id'], $msg);
                    }
                }
                catch (CustomException $e) {
                    $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                }
                catch (PDOException $e) {
                    $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                }
            }

            // Render page
            try {
                $this->data['request'] = AuthManager::renderPerson($person, true, 2, 1,
                    true, true, true, true, true,
                    3, true, true);
                if ($person && $person['state'] == 'PENDING') {
                    $this->data['additional_text'] = 'Pokud údaje nesouhlasí, klient si je bude muset upravit v nastavení. Poté proběhne kontrola znovu.';
                    $this->data['form'] = $form;
                }
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }
        }

        // Show bank account request
        else if ($params[0] == 'bank-account') {

            $this->data['h1'] = 'Detail žádosti o otevření bankovního účtu';
            $bankAccount = false;

            // Load bank account from DB
            try {
                $bankAccount = AccountManager::getBankAccount($params[1]);

                if (!$bankAccount || empty($bankAccount))
                    throw new CustomException('Žádost neexistuje');
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                $this->redirect('pending-requests/bank-account');
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                $this->redirect('pending-requests/bank-account');
            }

            $form = new FormFFW('req_form');
            $form->addTextArea('comment', 'Komentář (zobrazí se uživateli)', false, array('placeholder' => "Podporuje HTML tagy."));
            $form->addButton('btn_accept', 'Otevřít účet');
            $form->addButton('btn_reject', 'Zamítnout');

            // Processing
            if ($_POST) {
                try {
                    $formData = $form->getData();
                    if (isset($formData['comment']) && !empty($formData['comment'])) {
                        $comment = $formData['comment'];
                        $commentmsg = '<br /><br />Zaměstnanec přidal tento komentář:<br />' . $comment;
                    }

                    if ($form->getSentButton() == 'btn_accept' && $bankAccount['state'] == 'PENDING') {

                        AccountManager::verifyBankAccount($bankAccount['id'], 'ACTIVE', $_SESSION['auth']['id'], $comment);
                        $d_id = DisponentManager::addDisponentRequest($bankAccount['owner'], $bankAccount['id']);
                        DisponentManager::verifyDisponent($d_id, 'ACTIVE', $_SESSION['auth']['id'], '(vytvořeno automaticky - majitel účtu)');
                        $this->addMessage(new Message('Účet byl schválen a otevřen!', Message::TYPE_SUCCESS));

                        $bankAccount = AccountManager::getBankAccount($params[1]);

                        $n = AccountManager::getAccountTypeName($bankAccount['type']);
                        $d = DateUtils::getDateTime($bankAccount['inserted'])->format(DateUtils::CZE_DATE_FORMAT);
                        $msg = "Vaše žádost o otevření účtu ($n) ze dne $d byla schválena!<br />Účet byl otevřen." . $commentmsg;
                        NotificationManager::createNotificationForIbId($bankAccount['owner'], $msg);
                    }
                    if ($form->getSentButton() == 'btn_reject' && $bankAccount['state'] == 'PENDING') {

                        AccountManager::verifyBankAccount($bankAccount['id'], 'REJECTED', $_SESSION['auth']['id'], $comment);
                        $this->addMessage(new Message('Žádost byla zamítnuta!', Message::TYPE_SUCCESS));

                        $bankAccount = AccountManager::getBankAccount($params[1]);

                        $n = AccountManager::getAccountTypeName($bankAccount['type']);
                        $d = DateUtils::getDateTime($bankAccount['inserted'])->format(DateUtils::CZE_DATE_FORMAT);
                        $msg = "Vaše žádost o otevření účtu ($n) ze dne $d byla zamítnuta zaměstnancem.<br />V případě zájmu si můžete zažádat o účet znovu." . $commentmsg;
                        NotificationManager::createNotificationForIbId($bankAccount['owner'], $msg);
                    }
                }
                catch (CustomException $e) {
                    $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                }
                catch (PDOException $e) {
                    $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                }
            }

            // Render page
            try {
                $this->data['request'] = AccountManager::renderBankAccount($bankAccount, true, true,
                    false, 3, 2, false, false, false,
                    3, true, true);
                if ($bankAccount && $bankAccount['state'] == 'PENDING') {
                    $this->data['additional_text'] = 'Pokud žádost schválíte, účet bude automaticky otevřen. Číslo účtu se generuje automaticky.<br />
                                                      V případě zamítnutí si bude moci klient vytvořit žádost novou.';
                    $this->data['form'] = $form;
                }
            } catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }
        }

        // Show card request
        else if ($params[0] == 'card') {

            $this->data['h1'] = 'Detail žádosti o platební kartu';
            $card = false;

            // Load card from DB
            try {
                $card = CardManager::getCard($params[1]);

                if (!$card || empty($card))
                    throw new CustomException('Žádost neexistuje');
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                $this->redirect('pending-requests/card');
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                $this->redirect('pending-requests/card');
            }

            $form = new FormFFW('anonymous'); // important, do not delete

            $form_pending = new FormFFW('req_form');
            $form_pending->addTextArea('comment', 'Komentář (zobrazí se uživateli)', false, array('placeholder' => "Podporuje HTML tagy."));
            $form_pending->addButton('btn_accept1', 'Potvrdit žádost (první krok)');
            $form_pending->addButton('btn_reject1', 'Zamítnout');

            $form_making = new FormFFW('req_form');
            $form_making->addTextArea('comment', 'Komentář (zobrazí se uživateli)', false, array('placeholder' => "Podporuje HTML tagy."));
            $form_making->addTextBox('card_number', 'Číslo karty');
            $form_making->addTextBox('card_pin', 'PIN');
            $form_making->addDatePicker('card_expires', 'Datum platnosti');
            $form_making->addButton('btn_accept2', 'Potvrdit odeslání karty');
            $form_making->addButton('btn_reject2', 'Problém při vydávání (zrušit)');
            
            if ($card['state'] == 'PENDING')
                $form = $form_pending;
            if ($card['state'] == 'MAKING')
                $form = $form_making;

            // Processing
            if ($_POST) {
                try {
                    $formData = $form->getData();
                    if (isset($formData['comment']) && !empty($formData['comment'])) {
                        $comment = $formData['comment'];
                        $commentmsg = '<br /><br />Zaměstnanec přidal tento komentář:<br />' . $comment;
                    }

                    if ($form->getSentButton() == 'btn_accept1' && $card['state'] == 'PENDING') {

                        CardManager::verifyCardStepOne($card['id'], 'MAKING', $_SESSION['auth']['id'], $comment);
                        $this->addMessage(new Message('Žádost přijata, nyní objednejte kartu u dodavatele.', Message::TYPE_SUCCESS));

                        $card = CardManager::getCard($params[1]);

                        $disponent = DisponentManager::getDisponent($card['disponent']);
                        $a = AccountManager::getAccountNumber($disponent['account']);
                        $a2 = AccountManager::getBankAccountNameByDisponentId($disponent['id']);
                        $d = DateUtils::getDateTime($card['inserted'])->format(DateUtils::CZE_DATE_FORMAT);
                        $msg = "Vaše žádost o přidání karty k účtu $a ($a2) ze dne $d je nyní zpracovávána zaměstnancem!<br />Kartu vám brzy zašleme na vaši adresu." . $commentmsg;
                        NotificationManager::createNotificationForIbId(AuthManager::getIbAccountIdByDisponentId($card['disponent']), $msg);
                    }
                    if ($form->getSentButton() == 'btn_reject1' && $card['state'] == 'PENDING') {

                        CardManager::verifyCardStepOne($card['id'], 'REJECTED', $_SESSION['auth']['id'], $comment);
                        $this->addMessage(new Message('Žádost byla zamítnuta.', Message::TYPE_SUCCESS));

                        $card = CardManager::getCard($params[1]);

                        $disponent = DisponentManager::getDisponent($card['disponent']);
                        $a = AccountManager::getAccountNumber($disponent['account']);
                        $a2 = AccountManager::getBankAccountNameByDisponentId($disponent['id']);
                        $d = DateUtils::getDateTime($card['inserted'])->format(DateUtils::CZE_DATE_FORMAT);
                        $msg = "Vaše žádost o přidání karty k účtu $a ($a2) ze dne $d byla zamítnuta zaměstnancem!<br />Můžete si vytvořit žádost novou." . $commentmsg;
                        NotificationManager::createNotificationForIbId(AuthManager::getIbAccountIdByDisponentId($card['disponent']), $msg);
                    }
                    if ($form->getSentButton() == 'btn_accept2' && $card['state'] == 'MAKING') {

                        if (!isset($formData['card_number']) || empty($formData['card_number']) || !isset($formData['card_pin']) || empty($formData['card_pin']) || !isset($formData['card_expires']) || empty($formData['card_expires']))
                            throw new CustomException('Musíte zadat údaje ze štítku od výrobce!');

                        CardManager::verifyCardStepTwo($card['id'], 'ACTIVE', $_SESSION['auth']['id'], $comment, $formData['card_number'], $formData['card_pin'], $formData['card_expires']);
                        $this->addMessage(new Message('Karta aktivována a potvrzeno její odeslání na adresu klienta!', Message::TYPE_SUCCESS));

                        $card = CardManager::getCard($params[1]);

                        $disponent = DisponentManager::getDisponent($card['disponent']);
                        $a = AccountManager::getAccountNumber($disponent['account']);
                        $a2 = AccountManager::getBankAccountNameByDisponentId($disponent['id']);
                        $d = DateUtils::getDateTime($card['inserted'])->format(DateUtils::CZE_DATE_FORMAT);
                        $msg = "Vaše žádost o přidání karty k účtu $a ($a2) ze dne $d byla vyřízena zaměstnancem!<br />Karta byla zaslána na vaši adresu." . $commentmsg;
                        NotificationManager::createNotificationForIbId(AuthManager::getIbAccountIdByDisponentId($card['disponent']), $msg);
                    }
                    if ($form->getSentButton() == 'btn_reject2' && $card['state'] == 'MAKING') {

                        CardManager::verifyCardStepTwo($card['id'], 'REJECTED', $_SESSION['auth']['id'], $comment);
                        $this->addMessage(new Message('Žádost byla stornována!', Message::TYPE_SUCCESS));

                        $card = CardManager::getCard($params[1]);

                        $disponent = DisponentManager::getDisponent($card['disponent']);
                        $a = AccountManager::getAccountNumber($disponent['account']);
                        $a2 = AccountManager::getBankAccountNameByDisponentId($disponent['id']);
                        $d = DateUtils::getDateTime($card['inserted'])->format(DateUtils::CZE_DATE_FORMAT);
                        $msg = "Vaše žádost o přidání karty k účtu $a ($a2) ze dne $d byla stornována zaměstnancem!<br />Můžete si vytvořit žádost novou." . $commentmsg;
                        NotificationManager::createNotificationForIbId(AuthManager::getIbAccountIdByDisponentId($card['disponent']), $msg);
                    }
                }
                catch (CustomException $e) {
                    $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                }
                catch (PDOException $e) {
                    $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                }
            }

            if ($card['state'] == 'PENDING')
                $form = $form_pending;
            if ($card['state'] == 'MAKING')
                $form = $form_making;

            // Render page
            try {
                $this->data['request'] = CardManager::renderCard($card, true, false, 3,
                    true, 2, false, 3, true, true);
                if ($card && $card['state'] == 'PENDING') {
                    $this->data['additional_text'] = 'Pokud žádost schválíte, je nutné kartu objednat u dodavatele.<br />
                                                      V případě zamítnutí si bude moci klient vytvořit žádost novou.';
                    $this->data['form'] = $form;
                }
                if ($card && $card['state'] == 'MAKING') {
                    $this->data['additional_text'] = 'Po přijetí karty od dodavatele žádost potvrďte až po odeslání karty na adresu klienta.<br />
                                                      V takovém případě vyplňte údaje k dané kartě od výrobce uvedené na speciálním štítku, ten poté zničte.<br />
                                                      Pokud došlo k problémům u dodavatele, klient si bude moci vytvořit žádost novou.';
                    $this->data['form'] = $form;
                }
            } catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }
        }

        // Show disponent request
        else if ($params[0] == 'disponent') {

            $this->data['h1'] = 'Detail žádosti o přidání disponenta';
            $disponent = false;

            // Load disponent from DB
            try {
                $disponent = DisponentManager::getDisponent($params[1]);

                if (!$disponent || empty($disponent))
                    throw new CustomException('Žádost neexistuje');
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                $this->redirect('pending-requests/disponent');
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                $this->redirect('pending-requests/disponent');
            }

            $form = new FormFFW('req_form');
            $form->addTextArea('comment', 'Komentář (zobrazí se uživateli)', false, array('placeholder' => "Podporuje HTML tagy."));
            $form->addButton('btn_accept', 'Potvrdit přidání disponenta');
            $form->addButton('btn_reject', 'Zamítnout žádost');

            // Processing
            if ($_POST) {
                try {
                    $formData = $form->getData();
                    if (isset($formData['comment']) && !empty($formData['comment'])) {
                        $comment = $formData['comment'];
                        $commentmsg = '<br /><br />Zaměstnanec přidal tento komentář:<br />' . $comment;
                    }

                    if ($form->getSentButton() == 'btn_accept' && $disponent['state'] == 'AGREED') {
                        DisponentManager::verifyDisponent($disponent['id'], 'ACTIVE', $_SESSION['auth']['id'], $comment);
                        $this->addMessage(new Message('Žádost byla potvrzena, disponent byl přidán k účtu!', Message::TYPE_SUCCESS));

                        $disponent = DisponentManager::getDisponent($params[1]);

                        $di = AuthManager::renderPersonOneLiner(AuthManager::getPersonIdByIbAccountId($disponent['ib_account']), true, false);
                        $a = AccountManager::getAccountNumber($disponent['account']);
                        $requester_ib_id = AuthManager::getRequesterIbAccountIdOfDisponent($disponent['id']);
                        $requester_disponent_id = DisponentManager::getDisponentIdByIbAccountAndBankAccount($requester_ib_id, $disponent['account']);
                        $a2 = AccountManager::getBankAccountNameByDisponentId($requester_disponent_id);
                        $d = DateUtils::getDateTime($disponent['inserted'])->format(DateUtils::CZE_DATE_FORMAT);
                        $msg = "Vaše žádost o přidání disponenta $di k účtu $a ($a2) ze dne $d byla zpracována zaměstnancem!<br />Disponent byl přidán k účtu." . $commentmsg;
                        NotificationManager::createNotificationForIbId($requester_ib_id, $msg);

                        $msg = "Bylo vám uděleno dispoziční právo k účtu $a !" . $commentmsg;
                        NotificationManager::createNotificationForIbId($disponent['ib_account'], $msg);
                    }
                    if ($form->getSentButton() == 'btn_decline' && $disponent['state'] == 'AGREED') {
                        DisponentManager::verifyDisponent($disponent['id'], 'REJECTED', $_SESSION['auth']['id'], $comment);
                        $this->addMessage(new Message('Žádost byla zamítnuta', Message::TYPE_SUCCESS));

                        $disponent = DisponentManager::getDisponent($params[1]);

                        $di = AuthManager::renderPersonOneLiner(AuthManager::getPersonIdByIbAccountId($disponent['ib_account']), true, false);
                        $a = AccountManager::getAccountNumber($disponent['account']);
                        $requester_ib_id = AuthManager::getRequesterIbAccountIdOfDisponent($disponent['id']);
                        $requester_disponent_id = DisponentManager::getDisponentIdByIbAccountAndBankAccount($requester_ib_id, $disponent['account']);
                        $a2 = AccountManager::getBankAccountNameByDisponentId($requester_disponent_id);
                        $d = DateUtils::getDateTime($disponent['inserted'])->format(DateUtils::CZE_DATE_FORMAT);
                        $msg = "Vaše žádost o přidání disponenta $di k účtu $a ($a2) ze dne $d byla zamítnuta zaměstnancem!<br />Můžete si vytvořit žádost novou." . $commentmsg;
                        NotificationManager::createNotificationForIbId($requester_ib_id, $msg);

                        $msg = "Žádost o dispoziční právo k účtu $a, ke které jste udělil(a) souhlas, byla zamítnuta zaměstnancem!" . $commentmsg;
                        NotificationManager::createNotificationForIbId($disponent['ib_account'], $msg);
                    }
                }
                catch (CustomException $e) {
                    $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                }
                catch (PDOException $e) {
                    $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                }
            }

            // Render page
            try {
                $this->data['request'] = DisponentManager::renderDisponent($disponent, true, 3,
                    true, 3, 2, 3, true, true);
                if ($disponent && $disponent['state'] == 'AGREED') {
                    $this->data['additional_text'] = 'Oba účastníci s přidáním souhlasili.<br />
                                                      V případě zamítnutí si bude moci vlastník účtu vytvořit žádost novou a disponent ji bude muset opět potvrdit.';
                    $this->data['form'] = $form;
                }
            } catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }
        }
        else
            $this->redirect('pending-requests');

        $this->view = 'requestdetail';
    }
}
