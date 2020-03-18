<?php
class Message {

    const TYPE_SUCCESS = 0;
    const TYPE_INFO = 1;
    const TYPE_ERROR = 2;

    /** @var string */
    private $text;
    /** @var int */
    private $type;

    public function __construct($text, $type) {
        $this->text = $text;
        if ($type != self::TYPE_SUCCESS &&
            $type != self::TYPE_INFO &&
            $type != self::TYPE_ERROR)
            $this->type = 1;
        else
            $this->type = $type;
    }

    public function getStyle() {
        if ($this->type == self::TYPE_SUCCESS)
            return "msg_success";
        else if ($this->type == self::TYPE_ERROR)
            return "msg_error";
        else
            return "msg_info";
    }

    public function getText() {
        return $this->text;
    }
}