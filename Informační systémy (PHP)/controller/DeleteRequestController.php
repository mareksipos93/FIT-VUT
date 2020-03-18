<?php
class DeleteRequestController extends AbstractController {

    public function execute($params){
        $this->restrictAccess(false, true, true, true);

        $return = 'intro';
        if ($_GET && isset($_GET['return'])) {
            $return = $_GET['return'];
        }

        $this->needAtLeastParams($params, 2, $return);

        // Delete bank account request
        if ($params[0] == 'bank-account') {
            try {
                $account = AccountManager::getBankAccount($params[1]);

                if (!$account || empty($account)) {
                    throw new CustomException('Žádost neexistuje.');
                }

                if ($_SESSION['auth']['role'] == 'CLIENT' && $account['owner'] != $_SESSION['auth']['id']) {
                    throw new CustomException('Žádost může smazat jen ten, kdo ji vytvořil.');
                }

                if ($account['state'] != 'PENDING') {
                    throw new CustomException('Žádost již byla vyřízena.');
                }

                AccountManager::deleteBankAccount($params[1]);
                $this->addMessage(new Message('Žádost byla odstraněna.', Message::TYPE_SUCCESS));

                // Send notification to requester
                if ($_SESSION['auth']['role'] == 'EMPLOYEE' || $_SESSION['auth']['role'] == 'ADMIN') {
                    $t = AccountManager::getAccountTypeName($account['type']);
                    $d = DateUtils::getDateTime($account['inserted'])->format(DateUtils::CZE_DATE_FORMAT);
                    NotificationManager::createNotificationForIbId($account['owner'],
                        "Vaše žádost o otevření bankovního účtu ($t) ze dne $d byla zrušena zaměstnancem.");
                }
                $this->redirect($return);
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                $this->redirect($return);
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                $this->redirect($return);
            }
        }

        // Delete card request
        else if ($params[0] == 'card') {
            try {
                $card = CardManager::getCard($params[1]);
                $ib_id = AuthManager::getIbAccountIdByDisponentId($card['disponent']);

                if (!$card || empty($card)) {
                    throw new CustomException('Žádost neexistuje.');
                }

                if ($_SESSION['auth']['role'] == 'CLIENT' && $ib_id != $_SESSION['auth']['id']) {
                    throw new CustomException('Žádost může smazat jen ten, kdo ji vytvořil.');
                }

                if ($card['state'] != 'PENDING') {
                    throw new CustomException('Žádost již byla vyřízena nebo se zpracovává.');
                }

                CardManager::deleteCard($params[1]);
                $this->addMessage(new Message('Žádost byla odstraněna.', Message::TYPE_SUCCESS));

                // Send notification to requester
                if ($_SESSION['auth']['role'] == 'EMPLOYEE' || $_SESSION['auth']['role'] == 'ADMIN') {
                    $account = AccountManager::getBankAccountOfCard($card['id']);
                    $a = $account['number'];
                    $n = AccountManager::getBankAccountNameByDisponentId($card['disponent']);
                    $d = DateUtils::getDateTime($card['inserted'])->format(DateUtils::CZE_DATE_FORMAT);
                    NotificationManager::createNotificationForIbId($ib_id,
                        "Vaše žádost o vytvoření karty k účtu $a ($n) ze dne $d byla zrušena zaměstnancem.");
                }
                $this->redirect($return);
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                $this->redirect($return);
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                $this->redirect($return);
            }
        }

        // Delete disponent request
        else if ($params[0] == 'disponent') {
            try {
                $disponent = DisponentManager::getDisponent($params[1]);

                if (!$disponent || empty($disponent)) {
                    throw new CustomException('Žádost neexistuje.');
                }

                $requester = AuthManager::getRequesterOfDisponent($disponent['id']);
                if ($_SESSION['auth']['role'] == 'CLIENT' && $requester['id'] != $_SESSION['auth']['id']) {
                    throw new CustomException('Žádost může smazat jen ten, kdo ji vytvořil.');
                }

                if ($disponent['state'] != 'PENDING' && $disponent['state'] != 'AGREED') {
                    throw new CustomException('Žádost již byla vyřízena.');
                }

                DisponentManager::deleteDisponent($params[1]);
                $this->addMessage(new Message('Žádost byla odstraněna.', Message::TYPE_SUCCESS));

                // Send notification to requester
                if ($_SESSION['auth']['role'] == 'EMPLOYEE' || $_SESSION['auth']['role'] == 'ADMIN') {
                    $account = AccountManager::getBankAccount($disponent['account']);
                    $disponentasperson = AuthManager::getPersonIdByIbAccountId($disponent['ib_account']);
                    $ownerasdisponent = DisponentManager::getDisponentIdByIbAccountAndBankAccount($requester, $account['id']);
                    $di = AuthManager::renderPersonOneLiner($disponentasperson, true, false);
                    $a = $account['number'];
                    $n = AccountManager::getBankAccountNameByDisponentId($ownerasdisponent);
                    $da = DateUtils::getDateTime($disponent['inserted'])->format(DateUtils::CZE_DATE_FORMAT);
                    NotificationManager::createNotificationForIbId($requester,
                        "Vaše žádost o přidání disponenta $di k účtu $a ($n) ze dne $da byla zrušena zaměstnancem.");
                }
                $this->redirect($return);
            }
            catch (CustomException $e) {
                $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
                $this->redirect($return);
            }
            catch (PDOException $e) {
                $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
                $this->redirect($return);
            }
        }

        $this->redirect($return);

    }
}
