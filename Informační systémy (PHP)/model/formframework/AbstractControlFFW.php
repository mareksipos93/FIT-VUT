<?php
abstract class AbstractControlFFW {
    public $name;
    public $label;
    public $attributes;
    private $rules = array();
    private $method = FormFFW::METHOD_POST;

    const RULE_REQUIRED = 0;
    const RULE_MAX_LENGTH = 1;
    const RULE_PASSWORD = 2;
    const RULE_DATETIME = 3;
    const RULE_PATTERN = 4;

    const PATTERN_URL = '(http|https)://.*';
    const PATTERN_INTEGER = '[0-9]+';
    const PATTERN_EMAIL = '[a-zA-Z0-9._-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}$';
    const PATTERN_BANK_ACCOUNT = '/^[0-9]{4,}\/[0-9]{4}$/';
    const PATTERN_MONEY = '/^[0-9]{1,13}\.[0-9]{2}$/';

    public function __construct($name, $label = '', $attributes = array()) {
        $this->name = $name;
        $this->label = $label;
        $this->attributes = $attributes;
        $this->attributes['name'] = $name;
        $this->attributes['id'] = $name;
    }

    public function setMethod($method) {
        $this->method = $method;
    }

    public function getSentData($key = '') {
        if ($key)
            return ($this->method == FormFFW::METHOD_POST) ? $_POST[$key] : $_GET[$key];
        return ($this->method == FormFFW::METHOD_POST) ? $_POST : $_GET;
    }

    public function sentDataKeyExists($key) {
        return ($this->method == FormFFW::METHOD_POST) ? isset($_POST[$key]) : isset($_GET[$key]);
    }

    public function setTooltip($tooltip) {
        $this->attributes['title'] = $tooltip;
        return $this;
    }

    public function addClass($class) {
        if (isset($this->attributes['class']))
            $this->attributes['class'] .= " $class";
        else
            $this->attributes['class'] = $class;
        return $this;
    }

    public function getData() {
        if ($this->sentDataKeyExists($this->name))
            return array($this->name => $this->getSentData($this->name));
        else
            return array();
    }

    public abstract function setData($key, $value);

    public function getKeys() {
        return array($this->name);
    }

    protected abstract function renderControl($isPostBack);

    public function render($addValidationAttributes, $isPostBack)
    {
        if ($addValidationAttributes)
            $this->applyRulesToAttributes();
        return $this->renderControl($isPostBack);
    }

    private function applyRulesToAttributes() {
        foreach ($this->rules as $rule) {
            if ($rule['validate_client']) {
                switch ($rule['type']) {
                    case self::RULE_REQUIRED:
                        $this->attributes['required'] = 'required';
                        break;
                    case self::RULE_MAX_LENGTH:
                        $this->attributes['maxlength'] = $rule['max_length'];
                        break;
                    case self::RULE_PATTERN:
                        if (!isset($this->htmlParams['pattern']))
                            $this->attributes['pattern'] = $rule['pattern'];
                        break;
                }
            }
        }
    }

    /**
     * @throws CustomException
     */
    public function checkValidity() {
        foreach($this->rules as $rule) {
            if (($rule['validate_server']) && (!$this->checkRule($rule))) {
                $this->addClass('invalid');
                throw new CustomException($rule['message']);
            }
        }
    }

    private function checkRule($rule) {
        $name = $this->name;
        switch ($rule['type'])
        {
            case self::RULE_REQUIRED:
                return $this->sentDataKeyExists($name) && (is_numeric($this->getSentData($name)) || $this->getSentData($name));
            case self::RULE_MAX_LENGTH:
                return !$this->sentDataKeyExists($name) || !$this->getSentData($name) || mb_strlen($this->getSentData($name)) <= $rule['max_length'];
            case self::RULE_PATTERN:
                return !$this->sentDataKeyExists($name) || !$this->getSentData($name) || preg_match('~^' . $rule['pattern'] . '$~u', $this->getSentData($name));
            case self::RULE_DATETIME:
                return !$this->sentDataKeyExists($name) || !$this->getSentData($name) || DateUtils::isValidDate($this->getSentData($name), $rule['format']);
            case self::RULE_PASSWORD:
                return !$this->sentDataKeyExists($name) || !$this->getSentData($name) || ((StringUtils::removeAccents($this->getSentData($name)) == $this->getSentData($name)) && (mb_strlen($this->getSentData($name)) >= 6));
        }
        return false;
    }

    public function addRequiredRule($validateClient = true, $validateServer = true) {
        return $this->addRule(array(
            'type' => self::RULE_REQUIRED,
            'message' => 'Pole je povinné'
        ), $validateClient, $validateServer);
    }

    public function addMaxLengthRule($maxLength, $validateClient = true, $validateServer = true) {
        if ($maxLength < 0) $maxLength = 0;
        return $this->addRule(array(
            'type' => self::RULE_MAX_LENGTH,
            'message' => "Hodnota může být maximálně $maxLength znaků dlouhá",
            'max_length' => $maxLength
        ), $validateClient, $validateServer);
    }

    public function addMinLengthRule($minLength, $validateClient = true, $validateServer = true) {
        if ($minLength < 0) $minLength = 0;
        return $this->addPatternRule('.{' . $minLength . ',}', $validateClient, $validateServer);
    }

    public function addPatternRule($pattern, $validateClient = true, $validateServer = true) {
        return $this->addRule(array(
            'message' => 'Hodnota nemá správný formát',
            'type' => self::RULE_PATTERN,
            'pattern' => $pattern
        ), $validateClient, $validateServer);
    }

    public function addPasswordRule($validateClient = true, $validateServer = true) {
        $passwordMinLength = 8;
        $this->addMinLengthRule($passwordMinLength, $validateClient);
        return $this->addRule(array(
            'type' => self::RULE_PASSWORD,
            'message' => 'Heslo nesmí obsahovat diakritiku a musí být dlouhé alespoň '. $passwordMinLength .' znaků.',
        ), $validateClient, $validateServer);
    }

    public function addDateTimeRule($validateClient = true, $validateServer = true) {
        $this->addPatternRule('[0-3]?[0-9]\.[0-1]?[0-9]\.[0-9]{4}\s[0-2]?[0-9]\:[0-5]?[0-9](\:[0-5]?[0-9])?');
        return $this->addRule(array(
            'type' => self::RULE_DATETIME,
            'format' => DateUtils::CZE_DATETIME_FORMAT,
            'message' => 'Neplatné datum nebo čas, tvar musí být dd.mm.rrrr hh:mm (nebo hh:mm:ss)'
        ), $validateClient, $validateServer);
    }

    public function addDateRule($validateClient = true, $validateServer = true) {
        $this->addPatternRule('[0-3]?[0-9]\.[0-1]?[0-9]\.[0-9]{4}');
        return $this->addRule(array(
            'type' => self::RULE_DATETIME,
            'format' => DateUtils::CZE_DATE_FORMAT,
            'message' => 'Neplatné datum, datum musí být ve tvaru dd.mm.rrrr',
        ), $validateClient, $validateServer);
    }

    public function addTimeRule($validateClient = true, $validateServer = true) {
        $this->addPatternRule('[0-2]?[0-9]\:[0-5]?[0-9](\:[0-5]?[0-9])?');
        return $this->addRule(array(
            'type' => self::RULE_DATETIME,
            'format' => DateUtils::CZE_TIME_FORMAT,
            'message' => 'Neplatný čas, čas musí být ve tvaru hh:mm (nebo hh:mm:ss)',
        ), $validateClient, $validateServer);
    }

    private function addRule($rule, $validateClient, $validateServer) {
        $rule['validate_client'] = $validateClient;
        $rule['validate_server'] = $validateServer;
        $this->rules[] = $rule;
        return $this;
    }
}
