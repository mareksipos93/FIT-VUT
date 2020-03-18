<?php
class ListBoxFFW extends AbstractControlFFW {
    private $selectedValues = array();
    private $disabledValues = array();
    private $values = array();
    private $required;

    public function __construct($name, $required, $multiple, $label = '', $attributes = array()) {
        if ($required)
            $this->required = $required;
        parent::__construct($name, $label, $attributes);
        if ($multiple)
        {
            $this->attributes['multiple'] = 'multiple';
            $this->attributes['name'] .= '[]';
        }
    }

    /**
     * @return array
     * @throws CustomException
     */
    public function getData() {
        if ($this->sentDataKeyExists($this->name)) {
            $array_post = isset($this->attributes['multiple']) ? $this->getSentData($this->name) : array($this->getSentData($this->name));
            if (count(array_intersect($array_post, $this->values)) != count($array_post)) {
                $this->addClass('invalid');
                throw new CustomException('Vybrána neplatná data');
            }
            return array($this->name => $this->getSentData($this->name));
        }
        return array();
    }

    public function setSize($size) {
        $this->attributes['size'] = $size;
        return $this;
    }

    public function getKeys() {
        return array($this->name);
    }

    public function setData($key, $values) {
        if (is_array($values))
            $this->setSelectedValues($values);
        else
            $this->setSelectedValue($values);
    }

    public function setValues($values) {
        $this->values = $values;
        return $this;
    }

    public function setSelectedValues($values) {
        $this->selectedValues = $values;
        return $this;
    }

    public function setDisabledValues($values) {
        $this->disabledValues = $values;
        return $this;
    }

    public function setSelectedValue($value) {
        $this->selectedValues = array($value);
        return $this;
    }

    private function renderOptions(HtmlBuilder $builder, $isPostBack) {
        foreach ($this->values as $key => $value)
        {
            $params = array(
                'value' => $value,
            );

            $values = array();
            if ($isPostBack && $this->sentDataKeyExists($this->name))
                $values = (is_array($this->getSentData($this->name)) ? $this->getSentData($this->name) : array($this->getSentData($this->name)));
            else if (!$isPostBack)
                $values = $this->selectedValues;
            if (in_array($value, $values))
                $params['selected'] = 'selected';
            if (in_array($value, $this->disabledValues))
                $params['disabled'] = 'disabled';

            $builder->addValueElement('option', $key, $params);
        }
    }

    public function renderControl($isPostBack) {
        $builder = new HtmlBuilder();

        $builder->startElement('select', $this->attributes);
        if ((!$this->required) && (!isset($this->attributes['multiple'])))
            $this->values = array('' => '') + $this->values;
        $this->renderOptions($builder, $isPostBack);
        $builder->endElement();

        return $builder->render();
    }
}