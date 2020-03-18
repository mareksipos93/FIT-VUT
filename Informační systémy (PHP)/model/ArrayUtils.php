<?php
// Inspired by: https://www.itnetwork.cz
class ArrayUtils {

    /**
     * Returns filtered array only with allowed keys
     * @param array $input Input array
     * @param array $allowedKeys Filter array with allowed keys as values
     * @return array Filtered array
     */
    public static function filterKeys($input, $allowedKeys) {
        return array_intersect_key($input, array_flip($allowedKeys));
    }

    /**
     * Returns filtered array only with keys which starts with given prefix
     * @param array $input Input array
     * @param string $prefix Prefix determining allowed keys
     * @return array Filtered array
     */
    public static function filterKeysPrefix($input, $prefix) {
        $output = array();
        foreach ($input as $key => $value) {
            if (StringUtils::startsWith($key, $prefix, true))
                $output[$key] = $value;
        }
        return $output;
    }

    /**
     * Creates array with pairs (keys and values) based on chosen column name
     * @param array $rows Input, one big array with sub-arrays representing rows
     * @param string $keyColumn Column name whose values will be used as keys
     * @param string $valueColumn Column name whose values will be used as values
     * @return array Mapped array (collisions are solved by additional numbering)
     */
    public static function mapPairs($rows, $keyColumn, $valueColumn) {
        $pairs = array();
        foreach ($rows as $row) {
            $key = $row[$keyColumn];
            // Numbering in case of collision
            if (isset($pairs[$key])) {
                $i = 1;
                while (isset($pairs[$key . ' (' . $i . ')'])) {
                    $i++;
                }
                $key .= ' (' . $i . ')';
            }
            $pairs[$key] = $row[$valueColumn];
        }
        return $pairs;
    }

    /**
     * Creates array with values based on chosen column name
     * @param array $rows Input, one big array with sub-arrays representing rows
     * @param string $column Column name whose values will be used
     * @return array Mapped array
     */
    public static function mapSingles($rows, $column) {
        $singles = array();
        foreach ($rows as $row) {
            $singles[] = $row[$column];
        }
        return $singles;
    }

    public static function addPrefix($prefix, $input) {
        $output = array();
        foreach ($input as $key => $value) {
            $key = $prefix . $key;
            if (is_array($value))
                $value = self::addPrefix($prefix, $value);
            $output[$key] = $value;
        }
        return $output;
    }

    public static function removePrefix($prefix, $input) {
        $output = array();
        foreach ($input as $key => $value)  {
            if (strpos($key, $prefix) === 0)
                $key = substr($key, mb_strlen($prefix));
            if (is_array($value))
                $value = self::removePrefix($prefix, $value);
            $output[$key] = $value;
        }
        return $output;
    }

    public static function keysFromCamelToSnake($inputArray) {
        $outputArray = array();
        foreach ($inputArray as $key => $value)  {
            $key = StringUtils::camelToSnake($key);
            if (is_array($value))
                $value = self::keysFromCamelToSnake($value);
            $outputArray[$key] = $value;
        }
        return $outputArray;
    }

    public static function keysFromSnakeToCamel($inputArray) {
        $outputArray = array();
        foreach ($inputArray as $key => $value) {
            $key = StringUtils::snakeToCamel($key);
            if (is_array($value))
                $value = self::keysFromSnakeToCamel($value);
            $outputArray[$key] = $value;
        }
        return $outputArray;
    }
}