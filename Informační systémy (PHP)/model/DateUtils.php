<?php
// Inspired by: https://www.itnetwork.cz
class DateUtils {
    const CZE_DATETIME_FORMAT = 'j.n.Y G:i:s';    // 31.12.9999 23:59:59 (no leading zeros)
    const CZE_DATE_FORMAT = 'j.n.Y';              // 31.12.9999 (no leading zeros)
    const CZE_TIME_FORMAT = 'G:i:s';              // 23:59:59 (no leading zeros)
    const DB_DATETIME_FORMAT = 'Y-m-d H:i:s';     // 9999-12-31 23:59:59
    const DB_DATE_FORMAT = 'Y-m-d';               // 9999-12-31
    const DB_TIME_FORMAT = 'H:i:s';               // 23:59:59

    /**
     * @param $date
     * @return DateTime
     */
    public static function getDateTime($date) {
        if (ctype_digit($date))
            $date = '@' . $date; // timestamp
        return new DateTime($date);
    }

    public static function getPrettyDate($date) {
        return self::prettyDate(self::getDateTime($date));
    }

    public static function getPrettyDateTime($date) {
        $dateTime = self::getDateTime($date);
        return self::prettyDate($dateTime) . ' ' . $dateTime->format(self::CZE_TIME_FORMAT);
    }

    /**
     * @param string $date Input (czech) date
     * @param string $inputFormat How is input date formatted (best to use constants from this class)
     * @return string Safe DB format
     * @throws CustomException Input was invalid
     */
    public static function convertToDbFormat($date, $inputFormat = self::CZE_DATETIME_FORMAT) {;
        // 23:59 -> 23:59:00
        if (mb_substr_count($date, ':') == 1)
            $date .= ':00';
        // 31.12. 9999 -> 31.12.9999
        $a = array('/([\.\:\/])\s+/', '/\s+([\.\:\/])/', '/\s{2,}/');
        $b = array('\1', '\1', ' ');
        $date = trim(preg_replace($a, $b, $date));
        // 01.01.1000 -> 1.1.1000
        $a = array('/^0(\d+)/', '/([\.\/])0(\d+)/');
        $b = array('\1', '\1\2');
        $date = preg_replace($a, $b, $date);
        // Create DateTime from input format to validate
        $dateTime = DateTime::createFromFormat($inputFormat, $date);
        $errors = DateTime::getLastErrors();
        if ($errors['warning_count'] + $errors['error_count'] > 0) {
            throw new CustomException('Zadané datum nebo čas nebylo platné');
        }
        return $dateTime->format(self::$formatDictionary[$inputFormat]);
    }

    public static function isValidDate($date, $inputFormat = self::CZE_DATETIME_FORMAT) {
        try {
            self::convertToDbFormat($date, $inputFormat);
            return true;
        } catch (CustomException $e) {
            return false;
        }
    }

    public static function dbNow() {
        $dateTime = new DateTime();
        return $dateTime->format(self::DB_DATETIME_FORMAT);
    }

    /****************************************************************************************/

    private static $months = array('ledna', 'února', 'března', 'dubna', 'května', 'června',
        'července', 'srpna', 'září', 'října', 'listopadu', 'prosince');

    private static $formatDictionary = array(
        self::CZE_DATE_FORMAT => self::DB_DATE_FORMAT,
        self::CZE_DATETIME_FORMAT => self::DB_DATETIME_FORMAT,
        self::CZE_TIME_FORMAT => self::DB_TIME_FORMAT,
    );

    /**
     * @param $dateTime DateTime
     * @return string Pretty date representation
     */
    private static function prettyDate($dateTime) {
        $now = new DateTime();
        // Different year
        if ($dateTime->format('Y') != $now->format('Y'))
            return $dateTime->format(self::CZE_DATE_FORMAT);
        $dayMonth = $dateTime->format('d-m');
        // Today
        if ($dayMonth == $now->format('d-m'))
            return "Dnes";
        // Yesterday
        $now->modify('-1 DAY');
        if ($dayMonth == $now->format('d-m'))
            return "Včera";
        // Tomorrow
        $now->modify('+2 DAYS');
        if ($dayMonth == $now->format('d-m'))
            return "Zítra";
        // Same year
        return $dateTime->format('j.') . self::$months[$dateTime->format('n') - 1];
    }


}
