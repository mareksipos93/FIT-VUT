<?php
// Inspired by: https://www.itnetwork.cz
class StringUtils {

    const DB_ERR = 'Při komunikaci s databází došlo k chybě, zkuste to prosím znovu později';

    public static function startsWith($input, $prefix, $caseSensitive = true) {
        if (!$caseSensitive) {
            mb_strtolower($input);
            mb_strtolower($prefix);
        }
        return (mb_strpos($input, $prefix) === 0);
    }

    public static function endsWith($input, $suffix, $caseSensitive = true) {
        if (!$caseSensitive) {
            mb_strtolower($input);
            mb_strtolower($suffix);
        }
        return ((mb_strlen($input) >= mb_strlen($suffix)) &&
            ((mb_strpos($input, $suffix,
                    mb_strlen($input) - mb_strlen($suffix))) !== false));
    }

    public static function firstToUpper($text) {
        return mb_strtoupper(mb_substr($text, 0, 1)) .
            mb_substr($text, 1, mb_strlen($text));
    }

    public static function firstToLower($text) {
        return mb_strtolower(mb_substr($text, 0, 1)) .
            mb_substr($text, 1, mb_strlen($text));
    }

    /**
     * Converts text to hyphen notation<br />
     * Example: This is EXAMPLE text -> this-is-example-text
     * @param string $text Input text
     * @return string hyphenized text
     */
    public static function hyphenize($text) {
        return preg_replace("/\-{2,}/u", "-", preg_replace("/[^a-z0-9]/u", "-", mb_strtolower(self::removeAccents($text))));
    }

    /**
     * Converts words delimeted by hyphens to camel case notation.<br />
     * Example: some-SAMPLE-text -> SomeSampleText
     * @param string $text Words delimeted by hyphens
     * @param bool $firstLower Whether first word should be lower case
     * @return string
     */
    public static function hyphensToCamel($text, $firstLower = true) {
        return self::convertToCamel($text, '-', $firstLower);
    }

    /**
     * Converts words delimeted by underscores to camel case notation.<br />
     * Example: some_SAMPLE_text -> SomeSampleText
     * @param string $text Words delimeted by underscores
     * @param bool $firstLower Whether first word should be lower case
     * @return string
     */
    public static function snakeToCamel($text, $firstLower = true) {
        return self::convertToCamel($text, '_', $firstLower);
    }

    /**
     * Converts text in camel case into hyphen-delimeted text.<br />
     * Example: SomeSampleText -> some-sample-text
     * @param string $text Text in camel case
     * @return string
     */
    public static function camelToHyphens($text) {
        return self::convertFromCamel($text, '-');
    }

    /**
     * Converts text in camel case into underscore-delimeted text.<br />
     * Example: SomeSampleText -> some_sample_text
     * @param string $text Text in camel case
     * @return string
     */
    public static function camelToSnake($text) {
        return self::convertFromCamel($text, '_');
    }

    public static function formatMoney($money) {
        return number_format($money, 2, ',', ' ');
    }

    public static function generateRandomString($length, $onlyNumbers = false) {
        $result = '';
        if ($length <= 0) return $result;

        $allowedChars = array();
        if (!$onlyNumbers) {
            $allowedChars[] = array(ord('a'), ord('z'));
            $allowedChars[] = array(ord('A'), ord('Z'));
        }
        $allowedChars[] = array(ord('0'), ord('9'));

        while (strlen($result) < $length) {
            $n = rand(1, 255);
            foreach ($allowedChars as $interval) {
                if ($n > $interval[0] && $n < $interval[1]) {
                    $result .= chr($n);
                    break;
                }
            }
        }

        return $result;
    }

    public static function urlPartEquals($part1, $part2) {
        $part1 = mb_strtolower(self::hyphensToCamel($part1));
        $part2 = mb_strtolower(self::hyphensToCamel($part2));
        return $part1 == $part2;
    }

    /* Source: https://core.trac.wordpress.org/browser/trunk/src/wp-includes/formatting.php */
    public static function removeAccents($text) {
        $chars = array(
            // Decompositions for Latin-1 Supplement
            chr(195).chr(128) => 'A', chr(195).chr(129) => 'A',
            chr(195).chr(130) => 'A', chr(195).chr(131) => 'A',
            chr(195).chr(132) => 'A', chr(195).chr(133) => 'A',
            chr(195).chr(135) => 'C', chr(195).chr(136) => 'E',
            chr(195).chr(137) => 'E', chr(195).chr(138) => 'E',
            chr(195).chr(139) => 'E', chr(195).chr(140) => 'I',
            chr(195).chr(141) => 'I', chr(195).chr(142) => 'I',
            chr(195).chr(143) => 'I', chr(195).chr(145) => 'N',
            chr(195).chr(146) => 'O', chr(195).chr(147) => 'O',
            chr(195).chr(148) => 'O', chr(195).chr(149) => 'O',
            chr(195).chr(150) => 'O', chr(195).chr(153) => 'U',
            chr(195).chr(154) => 'U', chr(195).chr(155) => 'U',
            chr(195).chr(156) => 'U', chr(195).chr(157) => 'Y',
            chr(195).chr(159) => 's', chr(195).chr(160) => 'a',
            chr(195).chr(161) => 'a', chr(195).chr(162) => 'a',
            chr(195).chr(163) => 'a', chr(195).chr(164) => 'a',
            chr(195).chr(165) => 'a', chr(195).chr(167) => 'c',
            chr(195).chr(168) => 'e', chr(195).chr(169) => 'e',
            chr(195).chr(170) => 'e', chr(195).chr(171) => 'e',
            chr(195).chr(172) => 'i', chr(195).chr(173) => 'i',
            chr(195).chr(174) => 'i', chr(195).chr(175) => 'i',
            chr(195).chr(177) => 'n', chr(195).chr(178) => 'o',
            chr(195).chr(179) => 'o', chr(195).chr(180) => 'o',
            chr(195).chr(181) => 'o', chr(195).chr(182) => 'o',
            chr(195).chr(182) => 'o', chr(195).chr(185) => 'u',
            chr(195).chr(186) => 'u', chr(195).chr(187) => 'u',
            chr(195).chr(188) => 'u', chr(195).chr(189) => 'y',
            chr(195).chr(191) => 'y',
            // Decompositions for Latin Extended-A
            chr(196).chr(128) => 'A', chr(196).chr(129) => 'a',
            chr(196).chr(130) => 'A', chr(196).chr(131) => 'a',
            chr(196).chr(132) => 'A', chr(196).chr(133) => 'a',
            chr(196).chr(134) => 'C', chr(196).chr(135) => 'c',
            chr(196).chr(136) => 'C', chr(196).chr(137) => 'c',
            chr(196).chr(138) => 'C', chr(196).chr(139) => 'c',
            chr(196).chr(140) => 'C', chr(196).chr(141) => 'c',
            chr(196).chr(142) => 'D', chr(196).chr(143) => 'd',
            chr(196).chr(144) => 'D', chr(196).chr(145) => 'd',
            chr(196).chr(146) => 'E', chr(196).chr(147) => 'e',
            chr(196).chr(148) => 'E', chr(196).chr(149) => 'e',
            chr(196).chr(150) => 'E', chr(196).chr(151) => 'e',
            chr(196).chr(152) => 'E', chr(196).chr(153) => 'e',
            chr(196).chr(154) => 'E', chr(196).chr(155) => 'e',
            chr(196).chr(156) => 'G', chr(196).chr(157) => 'g',
            chr(196).chr(158) => 'G', chr(196).chr(159) => 'g',
            chr(196).chr(160) => 'G', chr(196).chr(161) => 'g',
            chr(196).chr(162) => 'G', chr(196).chr(163) => 'g',
            chr(196).chr(164) => 'H', chr(196).chr(165) => 'h',
            chr(196).chr(166) => 'H', chr(196).chr(167) => 'h',
            chr(196).chr(168) => 'I', chr(196).chr(169) => 'i',
            chr(196).chr(170) => 'I', chr(196).chr(171) => 'i',
            chr(196).chr(172) => 'I', chr(196).chr(173) => 'i',
            chr(196).chr(174) => 'I', chr(196).chr(175) => 'i',
            chr(196).chr(176) => 'I', chr(196).chr(177) => 'i',
            chr(196).chr(178) => 'IJ',chr(196).chr(179) => 'ij',
            chr(196).chr(180) => 'J', chr(196).chr(181) => 'j',
            chr(196).chr(182) => 'K', chr(196).chr(183) => 'k',
            chr(196).chr(184) => 'k', chr(196).chr(185) => 'L',
            chr(196).chr(186) => 'l', chr(196).chr(187) => 'L',
            chr(196).chr(188) => 'l', chr(196).chr(189) => 'L',
            chr(196).chr(190) => 'l', chr(196).chr(191) => 'L',
            chr(197).chr(128) => 'l', chr(197).chr(129) => 'L',
            chr(197).chr(130) => 'l', chr(197).chr(131) => 'N',
            chr(197).chr(132) => 'n', chr(197).chr(133) => 'N',
            chr(197).chr(134) => 'n', chr(197).chr(135) => 'N',
            chr(197).chr(136) => 'n', chr(197).chr(137) => 'N',
            chr(197).chr(138) => 'n', chr(197).chr(139) => 'N',
            chr(197).chr(140) => 'O', chr(197).chr(141) => 'o',
            chr(197).chr(142) => 'O', chr(197).chr(143) => 'o',
            chr(197).chr(144) => 'O', chr(197).chr(145) => 'o',
            chr(197).chr(146) => 'OE',chr(197).chr(147) => 'oe',
            chr(197).chr(148) => 'R',chr(197).chr(149) => 'r',
            chr(197).chr(150) => 'R',chr(197).chr(151) => 'r',
            chr(197).chr(152) => 'R',chr(197).chr(153) => 'r',
            chr(197).chr(154) => 'S',chr(197).chr(155) => 's',
            chr(197).chr(156) => 'S',chr(197).chr(157) => 's',
            chr(197).chr(158) => 'S',chr(197).chr(159) => 's',
            chr(197).chr(160) => 'S', chr(197).chr(161) => 's',
            chr(197).chr(162) => 'T', chr(197).chr(163) => 't',
            chr(197).chr(164) => 'T', chr(197).chr(165) => 't',
            chr(197).chr(166) => 'T', chr(197).chr(167) => 't',
            chr(197).chr(168) => 'U', chr(197).chr(169) => 'u',
            chr(197).chr(170) => 'U', chr(197).chr(171) => 'u',
            chr(197).chr(172) => 'U', chr(197).chr(173) => 'u',
            chr(197).chr(174) => 'U', chr(197).chr(175) => 'u',
            chr(197).chr(176) => 'U', chr(197).chr(177) => 'u',
            chr(197).chr(178) => 'U', chr(197).chr(179) => 'u',
            chr(197).chr(180) => 'W', chr(197).chr(181) => 'w',
            chr(197).chr(182) => 'Y', chr(197).chr(183) => 'y',
            chr(197).chr(184) => 'Y', chr(197).chr(185) => 'Z',
            chr(197).chr(186) => 'z', chr(197).chr(187) => 'Z',
            chr(197).chr(188) => 'z', chr(197).chr(189) => 'Z',
            chr(197).chr(190) => 'z', chr(197).chr(191) => 's',
            // Euro Sign
            chr(226).chr(130).chr(172) => 'E',
            // GBP (Pound) Sign
            chr(194).chr(163) => ''
        );
        return strtr($text, $chars);
    }

    /**************************************************************/

    private static function convertToCamel($text, $separator, $firstLower = true) {
        $result = str_replace(' ', '', mb_convert_case(
            str_replace($separator, ' ', $text), MB_CASE_TITLE));
        if ($firstLower)
            $result = self::firstToLower($result);
        return $result;
    }

    private static function convertFromCamel($text, $separator) {
        return ltrim(mb_strtolower(
            preg_replace('/[A-Z]/', $separator . '$0', $text)),
            $separator);
    }
}
