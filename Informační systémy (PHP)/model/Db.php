<?php
class Db {
    /** @var PDO */
    private static $connection;

    private static $settings = array(
        PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
        PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8",
        PDO::ATTR_EMULATE_PREPARES => false,
    );

    public static function connect($host, $user, $password, $db) {
        if (!isset(self::$connection)) {
            try {
                self::$connection = @new PDO(
                    "mysql:host=$host;dbname=$db",
                    $user,
                    $password,
                    self::$settings
                );
            } catch (Exception $ex) {
                return false;
            }
        }
        return true;
    }

    public static function query($query, $params = array()) {
        $return = self::$connection->prepare($query);
        $return->execute($params);
        return $return->rowCount();
    }

    public static function querySingle($query, $params = array()) {
        $result = self::$connection->prepare($query);
        $result->execute($params);
        return $result->fetch();
    }

    public static function queryOneValue($query, $params = array()) {
        $result = self::querySingle($query, $params);
        return $result[0];
    }

    public static function queryMulti($query, $params = array()) {
        $result = self::$connection->prepare($query);
        $result->execute($params);
        return $result->fetchAll();
    }

    public static function insert($table, $params = array()) {
        $columns = implode(", ", array_keys($params));
        $qmarks = str_repeat("?, ", sizeof($params) - 1) . "?";
        $query = "INSERT INTO $table ($columns) VALUES ($qmarks)";
        return self::query($query, array_values($params));
    }

    public static function update($table, $params = array(), $condition = "", $cparams = array()) {
        $columns = implode(" = ?, ", array_keys($params)) . " = ? ";
        $query = "UPDATE $table SET $columns $condition";
        return self::query($query, array_merge(array_values($params), $cparams));
    }

    public static function getLastInsertId() {
        return self::$connection->lastInsertId();
    }

    public static function selectMulti($columns, $table, $conditions = array(), $IN_column = '', $IN_values = array(), $suffix = '') {
        if (empty($table)) return false;
        if (!is_array($columns)) $columns = array($columns);
        if (empty($columns)) return false;
        if (!is_array($conditions)) $conditions = array($conditions);
        if (!is_array($IN_values)) $IN_values = array($IN_values);

        $columns = implode(', ', $columns);
        $where_columns = array_keys($conditions);
        $condition = empty($where_columns) ? '' : 'WHERE ' . implode(' = ? AND ', $where_columns) . ' = ?';
        $params = array_values($conditions);

        if (!empty($IN_column) && !empty($IN_values)) {
            if (empty($condition))
                $condition .= ' WHERE ';
            else
                $condition .= ' AND ';
            $condition .= $IN_column . ' IN (' . str_repeat('? ,', sizeof($IN_values)-1) . ' ?)';
            $params = array_merge($params, $IN_values);
        }

        $query = "SELECT $columns FROM $table $condition $suffix";
        return Db::queryMulti($query, $params);
    }



}
