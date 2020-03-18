<?php
class FormFFW
{

    const METHOD_POST = 'POST';
    const METHOD_GET = 'GET';

    /** @var AbstractControlFFW[] */
    private $controls = array();
    private $buttons = array();
    public $name;
    private $isPostBack = false;
    private $valid = false;
    public $validateClient = true;
    public $validateServer = true;
    private $rendered = array();
    private $method; // post get
    private $inline;

    public function __construct($name, $method = self::METHOD_POST, $inline = false) {
        $this->name = $name;
        $this->method = $method;
        $this->inline = $inline;
        $formNameBox = $this->addHiddenBox('form-name', $this->name);
        $this->isPostBack = $formNameBox->sentDataKeyExists('form-name') && $formNameBox->getSentData('form-name') == $this->name;
    }

    public function isPostBack() {
        return $this->isPostBack;
    }

    /**
     * @return string
     * @throws FormExceptionFFW
     */
    public function render() {
        $html = $this->renderStartForm();
        $html .= $this->renderControls();
        $html .= $this->renderButtons();
        $html .= $this->renderEndForm();
        return $html;
    }

    public function getSentButton() {
        foreach ($this->buttons as $button) {
            if ($button->sentDataKeyExists($button->name))
                return $button->name;
        }
        return null;
    }

    /**
     * @param array $_
     * @return array
     * @throws CustomException
     */
    public function getData($_ = array()) {
        if (!$this->valid)
            $this->checkValidity();
        $keys = func_get_args();
        if (!$keys)
            $keys = array_keys($this->controls);
        $controls = ArrayUtils::filterKeys($this->controls, $keys);
        $controls = array_diff_key($controls, $this->buttons);
        $controls = array_diff_key($controls, array_flip(array('form-name')));
        $data = array();
        /** @var AbstractControlFFW $control */
        foreach ($controls as $control) {
            $controlData = $control->getData();
            foreach ($controlData as $key => $value) {
                $data[$key] = $value;
            }
        }
        return $data;
    }

    public function setData($data) {
        $keymap = $this->getKeymap();

        foreach ($data as $key => $value) {
            if (isset($keymap[$key]))
                $keymap[$key]->setData($key, $value);
        }
    }

    private function getKeymap() {
        $keymap = array();
        foreach ($this->controls as $control) {
            $keys = $control->getKeys();
            foreach ($keys as $key) {
                $keymap[$key] = $control;
            }
        }
        return $keymap;
    }

    private function renderControl($name) {
        $this->rendered[$name] = $this->controls[$name];
        return $this->controls[$name]->render($this->validateClient, $this->isPostBack);
    }

    /**
     * @param array $names
     * @return array
     * @throws FormExceptionFFW
     */
    private function getControlsToRender($names = array()) {
        // No names given = render all
        if (!$names)
            $names = array_keys($this->controls);
        else {
            // Check all requested controls presence
            $diff = array_diff($names, array_keys($this->controls));
            if ($diff)
                throw new FormExceptionFFW('Ve formuláři neexistuje: ' . implode(', ', $diff));
        }
        // Do not render already rendered (and buttons)
        $controls = array_diff_key($this->controls, $this->buttons, $this->rendered);
        // Filter requested controls
        $controls = ArrayUtils::filterKeys($controls, $names);
        return $controls;
    }

    public function renderStartForm() {
        $attributes = array(
            'method' => $this->method,
            'id' => $this->name,
            'class' => 'fancyform',
        );
        if ($this->inline)
            $attributes['class'] .= ' inline-form';
        $builder = new HtmlBuilder();
        $builder->startElement('form', $attributes);
        foreach ($this->controls as $control) {
            if ($control instanceof InputBoxFFW && $control->type == 'hidden') {
                $builder->addValue($control->render(false, $this->isPostBack()), true);
                $this->rendered[$control->name] = $control;
            }
        }
        return $builder->render();
    }

    /**
     * @param null $_
     * @return string
     * @throws FormExceptionFFW
     */
    public function renderControls($_ = null) {
        $controls = $this->getControlsToRender(func_get_args());
        $builder = new HtmlBuilder();
        foreach ($controls as $control) {
            $builder->startElement('div', array(
                'class' => 'form-component',
            ));
            $builder->addValueElement('label', $control->label, array(
                'for' => $control->attributes['id'],
            ), $control->label === '&nbsp;');
            $builder->addValue($this->renderControl($control->attributes['name']), true);
            $builder->startElement('div', array(
                'class' => 'clear',
            ));
            $builder->endElement();
            $builder->endElement();
        }
        return $builder->render();
    }

    public function renderButtons() {
        if (!$this->buttons)
            return '';
        $builder = new HtmlBuilder();
        $builder->startElement('div', array(
            'class' => 'form-buttons',
        ));
        foreach ($this->buttons as $name => $button) {
            $builder->addValue($this->renderControl($name), true);
        }
        $builder->endElement();
        return $builder->render();
    }

    public function renderEndForm() {
        $builder = new HtmlBuilder();
        $builder->endElement('form');
        return $builder->render();
    }

    /**
     * @throws CustomException
     */
    private function checkValidity() {
        if ($this->validateServer) {
            $messages = array();
            foreach($this->controls as $control) {
                try {
                    $control->checkValidity();
                }
                catch (CustomException $e) {
                    $messages[] = $e->getMessage();
                }
            }
            if (!empty($messages))
                throw new CustomException(nl2br(implode("\n", $messages)));
        }
        $this->valid = true;
    }

    /**
     * @param AbstractControlFFW $control
     * @param bool $required
     * @return mixed
     */
    private function addControl($control, $required = false) {
        if ($required)
            $control->addRequiredRule();
        $control->setMethod($this->method);
        $this->controls[$control->attributes['name']] = $control;
        return $control;
    }

    /**
     * @param string $name Name of field
     * @param string $type Type of field (for example "text")
     * @param string $pattern Additional regex pattern to check
     * @param string $label Associated label
     * @param bool $required Whether this field is required
     * @param array $attributes HTML attributes
     * @return InputBoxFFW
     */
    private function addInputBox($name, $type, $pattern, $label, $required = false, $attributes = array()) {
        $textBox = new InputBoxFFW($name, $type, $label, $attributes);
        if ($pattern)
            $textBox->addPatternRule($pattern, false, true);
        return $this->addControl($textBox, $required);
    }

    public function addTextBox($name, $label, $required = false, $attributes = array()) {
        return $this->addInputBox($name, 'text', null, $label, $required, $attributes);
    }

    public function addHiddenBox($name, $text = '', $required = false, $attributes = array()) {
        return $this->addInputBox($name, 'hidden', null, '', $required, $attributes)
            ->setText($text);
    }

    public function addEmailBox($name, $label, $required = false, $attributes = array()) {
        return $this->addInputBox($name, 'email', AbstractControlFFW::PATTERN_EMAIL, $label, $required, $attributes);
    }

    public function addUrlBox($name, $label, $required = false, $attributes = array()) {
        return $this->addInputBox($name, 'url', AbstractControlFFW::PATTERN_URL, $label, $required, $attributes);
    }

    public function addNumberBox($name, $label, $required = false, $attributes = array()) {
        return $this->addInputBox($name, 'number', AbstractControlFFW::PATTERN_INTEGER, $label, $required, $attributes);
    }

    public function addPasswordBox($name, $label, $required = false, $attributes = array()) {
        $attributes['placeholder'] = 'Min 8 znaků, bez diakritiky';
        return $this->addInputBox($name, 'password', '', $label, $required, $attributes)
            ->addPasswordRule(false);
    }

    public function addDateTimePicker($name, $label, $required = false, $attributes = array()) {
        $attributes['placeholder'] = 'dd.mm.yyyy hh:mm';
        return $this->addControl(new DateTimePickerFFW($name, DateUtils::CZE_DATETIME_FORMAT, $label, $attributes), $required)
            ->addDateTimeRule();
    }

    public function addDatePicker($name, $label, $required = false, $attributes = array()) {
        $attributes['placeholder'] = 'dd.mm.yyyy';
        return $this->addControl(new DateTimePickerFFW($name, DateUtils::CZE_DATE_FORMAT, $label, $attributes), $required)
            ->addDateRule();
    }

    public function addTimePicker($name, $label, $required = false, $attributes = array()) {
        $attributes['placeholder'] = 'hh:mm';
        return $this->addControl(new DateTimePickerFFW($name, DateUtils::CZE_TIME_FORMAT, $label, $attributes), $required)
            ->addTimeRule();
    }

    public function addComboBox($name, $label, $required = false, $attributes = array()) {
        $comboBox = new ListBoxFFW($name, $required, false, $label, $attributes);
        $this->addControl($comboBox, $required);
        return $comboBox;
    }

    public function addListBox($name, $label, $required = false, $multiple = false, $attributes = array()) {
        if (!$multiple)
            $attributes['size'] = 4;
        $comboBox = new ListBoxFFW($name, $required, $multiple, $label, $attributes);
        $this->addControl($comboBox, $required);
        return $comboBox;
    }

    public function addTextArea($name, $label, $required = false, $attributes = array()) {
        return $this->addControl(new TextAreaFFW($name, $label, $attributes), $required);
    }

    public function addButton($name, $text, $attributes = array()) {
        $button = $this->addInputBox($name, 'submit', '', '', false, $attributes)
            ->setText($text);
        $this->buttons[$name] = $button;
        return $button;
    }
}
