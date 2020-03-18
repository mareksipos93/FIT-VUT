<?php
class DatabaseController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(false, false, true, true);
        $this->needAtLeastParams($params, 1, 'database/find/ib-account');

        if (StringUtils::urlPartEquals($params[0], 'detail')) {
            $this->needAtLeastParams($params, 3);

            $this->data['render_ib_account'] = false;
            $this->data['render_bank_account'] = false;
            $this->data['render_card'] = false;

            if (StringUtils::urlPartEquals($params[1], 'ib-account')) {
                $this->header['title'] = 'Detail online účtu';
                $this->header['keywords'] = 'detail, online, účet, IB';
                $this->header['description'] = 'Detaily o účtu internetového bankovnictví';

                $this->data['render_ib_account'] = true;
                $this->data['h1'] = 'Detail IB účtu';
                try {
                    $ib_account = AuthManager::getIbAccount(-1, $params[2]);
                    if (!$ib_account) {
                        throw new CustomException('Účet neexistuje.');
                    }
                    $person = AuthManager::getPersonByIbAccountId($ib_account['id']);

                    if ($ib_account['role'] == 'CLIENT')
                        $this->data['h1'] .= ' klienta';
                    if ($ib_account['role'] == 'EMPLOYEE')
                        $this->data['h1'] .= ' zaměstnance';
                    if ($ib_account['role'] == 'ADMIN')
                        $this->data['h1'] .= ' administrátora';

                    $this->data['ib_account'] = AuthManager::renderIbAccount($ib_account, true, true,
                        true, true, true);

                    $this->data['person'] = AuthManager::renderPerson($person, true, 1, 3,
                        false, true, true, true, true,
                        false, false, true);
                }
                catch (CustomException $e) {
                    $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                    $this->redirect('intro');
                }
                catch (PDOException $e) {
                    $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                    $this->redirect('intro');
                }
            }

            else if (StringUtils::urlPartEquals($params[1], 'bank-account')) {
                $this->header['title'] = 'Detail bankovního účtu';
                $this->header['keywords'] = 'detail, bankovní, účet';
                $this->header['description'] = 'Detaily o bankovním účtu';

                $this->data['render_bank_account'] = true;
                $this->data['h1'] = 'Detail bankovního účtu';
                try {
                    $bank_account = AccountManager::getBankAccount($params[2]);
                    if (!$bank_account) {
                        throw new CustomException('Účet neexistuje.');
                    }
                    $this->data['h1'] .= ' ' . $bank_account['number'];

                    $this->data['bank_account'] = AccountManager::renderBankAccount($bank_account, true,
                        true, true, 3, true, true, true,
                        true, false, false, true);
                }
                catch (CustomException $e) {
                    $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                    $this->redirect('intro');
                }
                catch (PDOException $e) {
                    $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                    $this->redirect('intro');
                }
            }

            else if (StringUtils::urlPartEquals($params[1], 'card')) {
                $this->header['title'] = 'Detail karty';
                $this->header['keywords'] = 'detail, karta, debetka';
                $this->header['description'] = 'Detaily o platební kartě';

                $this->data['render_card'] = true;
                $this->data['h1'] = 'Detail karty';
                try {
                    $card = CardManager::getCard($params[2]);
                    if (!$card) {
                        throw new CustomException('Karta neexistuje.');
                    }
                    $this->data['h1'] .= ' ' . $card['number'];

                    $this->data['card'] = CardManager::renderCard($card, true, true, 3,
                        true, true, true, false, false, true);
                }
                catch (CustomException $e) {
                    $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                    $this->redirect('intro');
                }
                catch (PDOException $e) {
                    $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                    $this->redirect('intro');
                }
            }

            else
                $this->redirect('intro');

            $this->view = 'databasedetail';
        }
        else if (StringUtils::urlPartEquals($params[0], 'find')) {
            $this->needAtLeastParams($params, 2, 'database/find/ib-account');

            // TODO - hledání zaměstnancem
            // database/find/ib_account
            // database/find/bank-account
            // database/find/card

            $this->view = 'databasefind';
        }
        else
            $this->redirect('intro');
    }
}