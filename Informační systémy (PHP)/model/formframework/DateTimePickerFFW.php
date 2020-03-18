<?php
class DateTimePickerFFW extends AbstractControlFFW {
    private $value;
    private $format;

    public function __construct($name, $format, $label = '', $attributes = array()) {
        $this->format = $format;
        if ($format == DateUtils::CZE_DATE_FORMAT)
            $this->addClass('form-datepicker');
        parent::__construct($name, $label, $attributes);
    }

    public function renderControl($isPostBack) {
        $value = ($isPostBack && $this->sentDataKeyExists($this->name)) ? $this->getSentData($this->name) : $this->value;
        $this->attributes['value'] = $value;
        $this->attributes['type'] = 'text';
        $builder = new HtmlBuilder();
        $builder->addElement('input', $this->attributes);
        return $builder->render();
    }

    /**
     * @return array
     * @throws CustomException
     */
    public function getData() {
        try {
            return $this->sentDataKeyExists($this->name) && $this->getSentData($this->name) ? array($this->name => DateUtils::convertToDbFormat($this->getSentData($this->name), $this->format)) : array();
        }
        catch (CustomException $ex) {
            throw new CustomException($ex->getMessage());
        }
    }

    public function setValue($value) {
        $this->value = $value;
        return $this;
    }

    public function setData($key, $value) {
        if ($this->format == DateUtils::CZE_TIME_FORMAT)
            $date = DateTime::createFromFormat(DateUtils::DB_TIME_FORMAT, $value);
        else if ($this->format == DateUtils::CZE_DATE_FORMAT)
            $date = DateTime::createFromFormat(DateUtils::DB_DATE_FORMAT, $value);
        else
            $date = DateTime::createFromFormat(DateUtils::DB_DATETIME_FORMAT, $value);
        if ($date)
            $this->value = $date->format($this->format);
    }
}
