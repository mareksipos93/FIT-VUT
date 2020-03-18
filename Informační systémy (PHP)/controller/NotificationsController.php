<?php
class NotificationsController extends AbstractController {

    public function execute($params){
        $this->restrictAccess(false, true, true, true);

        $this->header['title'] = 'Zprávy';
        $this->header['keywords'] = 'zprávy, oznámení, informace';
        $this->header['description'] = 'Schránka s přijatými zprávami';

        $notifications = NotificationManager::getNotifications($_SESSION['auth']['name']);

        $this->data['notifs'] = array();
        if (!$notifications) {
            $this->data['no_notifs'] = true;
            $this->data['notifs'] = array();
        } else {
            $this->data['no_notifs'] = false;

            NotificationManager::markAllNotificationsAsRead($_SESSION['auth']['name']);

            // Unseen
            $first = true;
            foreach($notifications as $key => $notif) {
                if ($notif['seen'] == 0) {
                    $h2 = '';
                    if ($first) {
                        $first = false;
                        $h2 = '<h2>Nepřečtené</h2>';
                    }
                    $type = 'notif_unseen';
                    $date = DateUtils::getPrettyDateTime($notif['created']);
                    $text = $notif['text'];
                    $this->data['notifs'][] = array('h2' => $h2, 'type' => $type, 'date' => $date, 'text' => $text);
                    unset($notifications[$key]);
                }
            }

            // Recent
            $first = true;
            foreach($notifications as $key => $notif) {
                $datetime = new DateTime();
                if (DateUtils::getPrettyDate($notif['created']) == DateUtils::getPrettyDate($datetime->format(DateUtils::DB_DATETIME_FORMAT))) {
                    $h2 = '';
                    if ($first) {
                        $first = false;
                        $h2 = '<h2>Dnešní</h2>';
                    }
                    $type = 'notif_recent';
                    $date = DateUtils::getPrettyDateTime($notif['created']);
                    $text = $notif['text'];
                    $this->data['notifs'][] = array('h2' => $h2, 'type' => $type, 'date' => $date, 'text' => $text);
                    unset($notifications[$key]);
                }
            }

            // Older
            $first = true;
            foreach($notifications as $notif) {
                $h2 = '';
                if ($first) {
                    $first = false;
                    $h2 = '<h2>Starší</h2>';
                }
                $type = '';
                $date = DateUtils::getPrettyDateTime($notif['created']);
                $text = $notif['text'];
                $this->data['notifs'][] = array('h2' => $h2, 'type' => $type, 'date' => $date, 'text' => $text);
            }
        }

        $this->view = 'notifications';
    }
}