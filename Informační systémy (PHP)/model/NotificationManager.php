<?php
class NotificationManager {

    public static function getAmountOfUnreadNotifications($ib_name) {
        return Db::queryOneValue('SELECT COUNT(*) FROM notification JOIN ib_account 
                                         ON ib_account.id = notification.account WHERE ib_account.name = ? 
                                         AND notification.seen = 0', array($ib_name));

    }

    public static function createNotificationForIbId($ib_id, $text = '') {
        if ($ib_id) Db::insert('notification', array('account' => $ib_id, 'text' => $text));
    }

    public static function getNotifications($ib_name) {
        return Db::queryMulti('SELECT text, seen, created FROM notification JOIN ib_account 
                                         ON ib_account.id = notification.account WHERE ib_account.name = ?
                                         ORDER BY created DESC', array($ib_name));
    }

    public static function markAllNotificationsAsRead($ib_name) {
        Db::query('UPDATE notification JOIN ib_account ON ib_account.id = notification.account 
                          SET seen = ? WHERE ib_account.name = ?', array(1, $ib_name));
    }

}