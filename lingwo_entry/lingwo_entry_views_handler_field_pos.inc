<?php
// $Id: $

/**
 * @file
 * Views handler intergration: displays the pos field in a human readable way
 */

class lingwo_entry_views_handler_field_pos extends views_handler_field {
  function render($values) {
    $value = $values->{$this->field_alias};
    return LingwoEntry::getPosOptions(TRUE, $value);
  }
}

