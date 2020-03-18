<?php
class InputBoxFFW extends AbstractControlFFW {
    private $text;
    public $type;

    public function __construct($name, $type, $label = '', $attributes = array()) {
        $this->type = $type;
        parent::__construct($name, $label, $attributes);
    }

    public function setText($text) {
        $this->text = $text;
        return $this;
    }

    public function setData($key, $text) {
        if ($this->type != 'password')
            $this->text = $text;
    }

    public function renderControl($isPostBack) {
        $value = ($isPostBack && $this->sentDataKeyExists($this->name) && $this->type != 'password') ? $this->getSentData($this->name) : $this->text;
        $this->attributes['value'] = $value;
        $this->attributes['type'] = $this->type;
        $builder = new HtmlBuilder();
        $builder->addElement('input', $this->attributes);
        return $builder->render();
    }

}