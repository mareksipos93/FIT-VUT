<?php
class BankAccountController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(false, true, false, false);
        $this->needAtLeastParams($params, 1); // no bank-account number = kick
        $this->needAtLeastParams($params, 2, 'bank-account/' . $params[0] . '/overview');

        $account = false;
        $disponent = false;
        try {
            $account = AccountManager::getBankAccountByNumber($params[0]);
            $disponent = DisponentManager::getActiveDisponentByIbAccountAndBankAccount($_SESSION['auth']['id'], $account['id']);
            if (!$account) {
                throw new CustomException('Účet neexistuje!');
            }
            if ($_SESSION['auth']['role'] == 'CLIENT') {
                $disponents = DisponentManager::getDisponentsOfBankAccount($account['id']);
                $can_access = false;
                foreach ($disponents as $disponent) {
                    if ($disponent['ib_account'] == $_SESSION['auth']['id']) {
                        $can_access = true;
                        break;
                    }
                }
                if (!$can_access) {
                    throw new CustomException('K účtu může přistupovat pouze jeho disponent!');
                }
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

        // OVERVIEW TAB
        if (StringUtils::urlPartEquals($params[1], 'overview')) {
            $this->header['title'] = 'Přehled k účtu';
            $this->header['keywords'] = 'přehled, bankovní, účet';
            $this->header['description'] = 'Přehled k bankovnímu účtu';

            $this->view = 'bankaccountoverview';

            try {
                $this->data['overview'] = AccountManager::renderBankAccount($account, false, true, true,
                    2, false, true, false, false, false,
                    false, false);
            }
            catch (PDOException $e) {
                $this->data['overview'] = 'Nepodařilo se načíst přehled';
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }

            $form = new FormFFW('change_name', FormFFW::METHOD_POST, true);
            $form->addTextBox('new_name', 'Název účtu')
                ->setText(is_null($disponent['custom_name']) ? '' : $disponent['custom_name'])
                ->addMaxLengthRule(50);
            $form->addButton('change_name', 'Změnit');

            if ($_POST && $form->isPostBack()) {
                try {
                    $formData = $form->getData();
                    if (!isset($formData['new_name']))
                        $formData['new_name'] = '';
                    else
                        $formData['new_name'] = trim($formData['new_name']);
                    if (empty($formData['new_name'])) {
                        AccountManager::removeCustomBankAccountName($disponent['id']);
                        $this->addMessage(new Message('Název účtu odebrán', Message::TYPE_SUCCESS));
                    } else {
                        AccountManager::setCustomBankAccountName($disponent['id'], $formData['new_name']);
                        $this->addMessage(new Message('Název účtu změněn', Message::TYPE_SUCCESS));
                    }
                }
                catch (CustomException $e) {
                    $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                }
                catch (PDOException $e) {
                    $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                }
            }

            $this->data['change_name'] = $form;
        }

        // PAYMENT TAB
        else if (StringUtils::urlPartEquals($params[1], 'payment')) {
            $this->header['title'] = 'Platba';
            $this->header['keywords'] = 'platba, zaplacení, pay';
            $this->header['description'] = 'Tvorba platebního příkazu';

            $this->view = 'bankaccountpayment';

            $form = new FormFFW('payment');
            $form->addTextBox('from_acc', 'Z účtu *', false, array('disabled' => 'disabled'))
                ->setText($account['number'] . '/1234 (' . AccountManager::getBankAccountNameByDisponentId($disponent['id']) . ')');
            $acc_field = $form->addTextBox('to_acc', 'Na účet *', true, array('placeholder' => 'číslo účtu / kód banky'));
            $money_field = $form->addTextBox('amount', 'Částka *', true, array('placeholder' => '0,00'));
            $form->addTextBox('variable_symbol', 'Variabilní symbol', false, array('placeholder' => 'Max 10 znaků'))
                ->addMaxLengthRule(10);
            $form->addTextBox('constant_symbol', 'Konstantní symbol', false, array('placeholder' => 'Max 4 znaky'))
                ->addMaxLengthRule(4);
            $form->addTextBox('specific_symbol', 'Specifický symbol', false, array('placeholder' => 'Max 10 znaků'))
                ->addMaxLengthRule(10);
            $form->addTextArea('message', 'Zpráva pro příjemce');
            $form->addButton('send', 'Odeslat');

            $this->data['form'] = $form;

            if ($_POST) {
                try {
                    $formData = $form->getData();

                    // check acc_field
                    $formData['to_acc'] = trim(str_replace(' ', '', $formData['to_acc']));
                    if (preg_match(AbstractControlFFW::PATTERN_BANK_ACCOUNT, $formData['to_acc']) != 1) {
                        $acc_field->addClass('invalid');
                        throw new CustomException('Bankovní účet je neplatný');
                    }
                    $formData['to_acc'] = explode('/', $formData['to_acc']);
                    if ($formData['to_acc'][0] == $account['number'] && $formData['to_acc'][1] == '1234') {
                        $acc_field->addClass('invalid');
                        throw new CustomException('Nelze zaslat peníze z účtu na tentýž účet');
                    }
                    $target_account = '';
                    if ($formData['to_acc'][1] == '1234') {
                        $target_account = AccountManager::getBankAccountByNumber($formData['to_acc'][0]);
                        if (!$target_account) {
                            $acc_field->addClass('invalid');
                            throw new CustomException('Tento účet v Simple Bank neexistuje!');
                        }
                    }

                    // check money_field
                    $formData['amount'] = trim(str_replace(',', '.', str_replace(' ', '', $formData['amount'])));
                    if (preg_match(AbstractControlFFW::PATTERN_MONEY, $formData['amount']) != 1) {
                        $money_field->addClass('invalid');
                        throw new CustomException('Zadaná částka je neplatná');
                    }
                    $formData['amount'] = floatval($formData['amount']);
                    if ($formData['amount'] === 0.00) {
                        $money_field->addClass('invalid');
                        throw new CustomException('Zadaná částka je nulová');
                    }

                    if ($account['funds'] < $formData['amount']) {
                        throw new CustomException('Na uskutečnění transakce nemáte na účtu dostatek prostředků');
                    }

                    AccountManager::subtractMoney($account['id'], $formData['amount']);
                    if ($formData['to_acc'][1] == '1234') {
                        AccountManager::addMoney($target_account['id'], $formData['amount']);
                    }

                    AccountManager::addPayment($account['number'], '1234', $formData['to_acc'][0],
                        $formData['to_acc'][1], $formData['amount'], $disponent['id'],
                        trim($formData['variable_symbol']), trim($formData['constant_symbol']),
                        trim($formData['specific_symbol']), trim($formData['message']));

                    if ($formData['to_acc'][1] != '1234')
                        $this->addMessage(new Message('Příkaz zadán, platba bude provedena v nejbližším možném termínu.', Message::TYPE_SUCCESS));
                    else
                        $this->addMessage(new Message('Platba byla úspěšně provedena.', Message::TYPE_SUCCESS));

                    $this->redirect('bank-account/' . $account['number'] . '/payment');
                }
                catch (CustomException $e) {
                    $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                }
                catch (PDOException $e) {
                    $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                }
            }

        }

        // CARDS TAB
        else if (StringUtils::urlPartEquals($params[1], 'cards')) {
            $this->header['title'] = 'Karty';
            $this->header['keywords'] = 'karty, kreditky';
            $this->header['description'] = 'Kreditní karty k účtu';

            $this->view = 'bankaccountcards';

            $new_card = new FormFFW('new_card');
            $new_card->addButton('submit', 'Zažádat o novou kartu');
            $this->data['new_card'] = $new_card;

            if ($_POST && $new_card->isPostBack()) {
                try {
                    CardManager::addCardRequest($disponent['id']);
                    $this->addMessage(new Message('Žádost o vytvoření nové karty odeslána!', Message::TYPE_SUCCESS));
                }
                catch (PDOException $e) {
                    $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                }
            }

            $this->data['active_cards'] = false;
            $this->data['other_cards'] = false;
            try {
                $active_cards = CardManager::getCardsByDisponent($disponent['id'], array('ACTIVE'));
                if ($active_cards)
                    $this->data['active_cards'] = CardManager::renderCardsAsTable($active_cards, false,
                        true, false, false, false, true,
                        false, false, false, false,
                        false, 'bank-account/' . $account['number'] . '/cards');

                $other_cards = CardManager::getCardsByDisponent($disponent['id'], array('PENDING', 'MAKING', 'DEACTIVATED'));
                if ($other_cards)
                    $this->data['other_cards'] = CardManager::renderCardsAsTable($other_cards, false,
                        true, false, false, true, false,
                        false, false, true, true,
                        false, 'bank-account/' . $account['number'] . '/cards');
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }

        }

        // DISPONENTS TAB
        else if (StringUtils::urlPartEquals($params[1], 'disponents')) {
            $this->header['title'] = 'Disponenti';
            $this->header['keywords'] = 'disponenti, oprávnění, uživatelé';
            $this->header['description'] = 'Disponenti bankovního účtu';

            $this->view = 'bankaccountdisponents';

            $new_disponent = new FormFFW('new_disponent');
            $box = $new_disponent->addTextBox('person', 'Přihlašovací jméno osoby', true);
            $new_disponent->addButton('submit', 'Přidat disponenta');
            $this->data['new_disponent'] = $new_disponent;

            $this->data['can_add'] = $disponent['ib_account'] == $account['owner'] || $_SESSION['auth']['role'] == 'EMPLOYEE' || $_SESSION['auth']['role'] == 'ADMIN';
            // Najít disponenta podle identifikace nebo IB accountu
            // Zkontrolovat, jestli už není přidaný

            if ($_POST && $new_disponent->isPostBack()) {
                try {
                    if (!$this->data['can_add'])
                        throw new CustomException('Přidávat disponenty k tomuto účtu může pouze jeho majitel');

                    $formData = $new_disponent->getData();

                    $person_ib_id = AuthManager::getIbAccountId($formData['person']);
                    if (!$person_ib_id) {
                        $box->addClass('invalid');
                        throw new CustomException('Zadaná osoba není našim klientem.');
                    }

                    $got_disponents = DisponentManager::getAllDisponentsByIbAccountAndBankAccount($person_ib_id, $account['id']);
                    if ($got_disponents) {
                        foreach ($got_disponents as $got_disponent) {
                            if ($got_disponent['state'] == 'PENDING' || $got_disponent['state'] == 'AGREED' || $got_disponent['state'] == 'ACTIVE') {
                                $box->addClass('invalid');
                                throw new CustomException('Tato osoba již tímto účtem disponuje nebo žádost již existuje.');
                            }
                        }
                    }

                    if (AuthManager::getRoleOfIbAccount($person_ib_id) != 'CLIENT') {
                        $box->addClass('invalid');
                        throw new CustomException('Tento účet nelze přidat jako disponenta.');
                    }

                    $new_id = DisponentManager::addDisponentRequest($person_ib_id, $account['id']);

                    $p = AuthManager::renderPersonOneLiner(AuthManager::getPersonIdByIbAccountId($account['owner']), true, false);
                    $a = $account['number'];
                    $agree = '<a href="/disponent-agree/'.$new_id.'/agree">ZDE</a>';
                    $disagree = '<a href="/disponent-agree/'.$new_id.'/disagree">ZDE</a>';
                    $msg = "Dobrý den,<br />uživatel $p vás chce přidat jako disponenta k účtu $a. Pokud s tím souhlasíte, klikněte prosím $agree, jinak klikněte $disagree.";
                    NotificationManager::createNotificationForIbId($person_ib_id, $msg);

                    $this->addMessage(new Message('Žádost o přidání disponenta odeslána!<br />Nyní ji musí disponent potvrdit.', Message::TYPE_SUCCESS));
                }
                catch (CustomException $e) {
                    $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                }
                catch (PDOException $e) {
                    $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                }
            }

            $this->data['active_disponents'] = false;
            $this->data['other_disponents'] = false;
            try {
                $active_disponents = DisponentManager::getDisponentsOfBankAccount($account['id'], array('ACTIVE'));
                if ($active_disponents)
                    $this->data['active_disponents'] = DisponentManager::renderDisponentsAsTable($active_disponents, true,
                        false, false, 2, false, false,
                        false, false, false, false,
                        'bank-account/' . $account['number'] . '/disponents');

                $other_disponents = DisponentManager::getDisponentsOfBankAccount($account['id'], array('PENDING', 'AGREED', 'DEACTIVATED'));
                if ($other_disponents)
                    $this->data['other_disponents'] = DisponentManager::renderDisponentsAsTable($other_disponents, true,
                        false, false, 2, 1, false,
                        false, true, true, false,
                        'bank-account/' . $account['number'] . '/disponents');
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
            }
        }

        // LOGS TAB
        else if (StringUtils::urlPartEquals($params[1], 'logs')) {
            $this->header['title'] = 'Výpisy';
            $this->header['keywords'] = 'výpisy, protokoly, pohyby';
            $this->header['description'] = 'Výpisy z účtu';

            $this->view = 'bankaccountlogs';

            // TODO - výpisy z účtu
        }

        else
            $this->redirect('bank-account/' . $params[0] . '/overview');

    }
}