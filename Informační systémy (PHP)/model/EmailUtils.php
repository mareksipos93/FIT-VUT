<?php
class EmailUtils {
    /**
     * Sends an e-mail message formatted with HTML with UTF-8 encoding
     * @param $to string Recipient
     * @param $from string Sender
     * @param $subject string Subject of message
     * @param $message string Text of message with HTML
     */
    public static function sendEmail($to, $from, $subject, $message) {
        $header = "From: " . $from;
        $header .= "\nMIME-Version: 1.0\n";
        $header .= "Content-Type: text/html; charset=\"utf-8\"\n";
        mb_send_mail($to, $subject, $message, $header);
    }
}
