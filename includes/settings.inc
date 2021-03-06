<?php

/**
 * @file
 * Helper code for managing module variables
 */

class LingwoSettings {
  protected $base_name = '';

  function __construct() {
    if (empty($this->base_name)) {
      throw new Exception('You must sub-class LingwoSettings and change $base_name!');
    }
  }

  public function __get($name) {
    $res = variable_get($this->name($name), NULL);
    if (is_null($res)) {
      $res = $this->getDefault($name);
    }
    return $res;
  }

  public function __set($name, $value) {
    variable_set($this->name($name), $value);
  }

  public function __isset($name) {
    return !is_null(variable_get($this->name($name), NULL));
  }

  public function __unset($name) {
    variable_del($this->name($name));
  }

  public function name($name) {
    if (!$this->isValid($name)) {
      throw new Exception('Invalid setting name: ' . $name);
    }
    return $this->base_name . '_' . $name;
  }

  public function getDefault($name) {
    return NULL;
  }

  public function isValid($name) {
    return TRUE;
  }
}

