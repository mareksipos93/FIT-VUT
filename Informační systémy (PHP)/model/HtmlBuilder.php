<?php
// Inspired by: https://www.itnetwork.cz
class HtmlBuilder {

    /** How many spaces for indentation, -1 = use tab, 0 = no indentation */
    const INDENTATION_SPACES = 4;

    private $html = '';
    private $elementStack = array();

    private function renderElement($tag, $attributes, $pair) {
        $this->renderIndentation();
        $this->html .= '<' . htmlspecialchars($tag);
        foreach ($attributes as $attr => $value)  {
            $this->html .= ' ' . htmlspecialchars($attr) . '="' . htmlspecialchars($value) . '"';
        }
        if (!$pair)
            $this->html .= ' /';
        $this->html .= '>';
        if ($pair)
            array_push($this->elementStack, $tag);
    }

    private function renderIndentation() {
        if (self::INDENTATION_SPACES != 0) {
            for ($i = 0; $i < sizeof($this->elementStack); $i++) {
                if (self::INDENTATION_SPACES < 0)
                    $this->html .= "\t";
                else {
                    for ($j = 0; $j < self::INDENTATION_SPACES; $j++)
                        $this->html .= ' ';
                }
            }
        }
    }

    private function renderNewline() {
        $this->html .= "\n";
    }

    public function addElement($tag, $attributes = array()) {
        $this->renderElement($tag, $attributes, false);
    }

    public function startElement($tag, $attributes = array()) {
        $this->renderElement($tag, $attributes, true);
        $this->renderNewline();
    }

    public function addValue($value, $doNotEscape = false) {
        $this->html .= $doNotEscape ? $value : htmlspecialchars($value);
    }

    public function endElement($tag = null) {
        if (!$tag)
            $tag = array_pop($this->elementStack);
        $this->html .= '</' . htmlspecialchars($tag) . '>';
        $this->renderNewline();
    }

    public function addValueElement($tag, $value, $attributes = array(), $doNotEscape = false) {
        $this->renderElement($tag, $attributes, true);
        $this->addValue($value, $doNotEscape);
        $this->endElement();
    }

    public function render() {
        return $this->html;
    }
}
