<?php
class TextAreaFFW extends AbstractControlFFW {
    private $text;

    public function setText($text) {
        $this->text = $text;
        return $this;
    }

    public function setData($key, $text) {
        $this->text = $text;
    }

    public function renderControl($isPostBack) {
        $value = ($isPostBack && $this->sentDataKeyExists($this->name)) ? $this->getSentData($this->name) : $this->text;
        $builder = new HtmlBuilder();
        $builder->addValueElement('textarea', $value, $this->attributes);
        return $builder->render();
    }
}