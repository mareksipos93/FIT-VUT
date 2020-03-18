<?php
class DisponentAgreeController extends AbstractController {

    public function execute($params) {
        $this->restrictAccess(false, true, false, false);
        $this->needAtLeastParams($params, 2);

        try {
            $disponent = DisponentManager::getDisponent($params[0]);
            if (!$disponent)
                throw new CustomException('Žádost nebyla nalezena.');
            if ($disponent['ib_account'] != $_SESSION['auth']['id'])
                throw new CustomException('Žádost může vyřídit jen ten, koho se týká.');
            if ($disponent['state'] != 'PENDING')
                throw new CustomException('Žádost již byla vyřízena.');

            if ($params[1] == 'agree') {
                DisponentManager::agreeDisponent($disponent['id']);
                $this->addMessage(new Message('Potvrzení uděleno!', Message::TYPE_SUCCESS));

                $d = AuthManager::renderPersonOneLiner(AuthManager::getPersonIdByIbAccountId($_SESSION['auth']['id']), true, false);
                $a = AccountManager::getAccountNumber($disponent['account']);
                $msg = "Vaše žádost o přidání disponenta $d k účtu $a byla disponentem potvrzena!<br />Disponent bude přidán po ověření pracovníkem.";
                $author = AuthManager::getRequesterIbAccountIdOfDisponent($disponent['id']);
                NotificationManager::createNotificationForIbId($author, $msg);
            }
            if ($params[1] == 'disagree') {
                DisponentManager::disagreeDisponent($disponent['id']);
                $this->addMessage(new Message('Žádost zamítnuta!', Message::TYPE_SUCCESS));

                $d = AuthManager::renderPersonOneLiner(AuthManager::getPersonIdByIbAccountId($_SESSION['auth']['id']), true, false);
                $a = AccountManager::getAccountNumber($disponent['account']);
                $msg = "Vaše žádost o přidání disponenta $d k účtu $a byla disponentem zamítnuta!";
                $author = AuthManager::getRequesterIbAccountIdOfDisponent($disponent['id']);
                NotificationManager::createNotificationForIbId($author, $msg);
            }
        }
        catch (CustomException $e) {
            $this->addMessage(new Message($e->getMessage(), Message::TYPE_ERROR));
        }
        catch (PDOException $e) {
            $this->addMessage(new Message($_SESSION['debug_enabled'] ? $e->getMessage() : StringUtils::DB_ERR, Message::TYPE_ERROR));
        }

        $this->redirect('intro');
    }
}