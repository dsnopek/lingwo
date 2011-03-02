<?php
// $Id: $

/**
 * @file
 * Views handler intergration: displays the pos field in a human readable way
 */

class lingwo_entry_views_handler_field_pos extends views_handler_field {
  function init(&$view, &$options) {
    parent::init($view, $options);
    $this->additional_fields['language'] = 'language';
  }

  function render($values) {
    $value = $values->{$this->field_alias};
    if (!empty($value)) {
      return LingwoEntry::getPosOptions($values->{$this->aliases['language']}, TRUE, $value);
    }
  }
}

