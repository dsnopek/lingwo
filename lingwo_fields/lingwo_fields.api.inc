<?php
// $Id$

/**
 * @file
 * API functions for dealing with entries
 */

require_once(dirname(__FILE__) .'/../includes/settings.inc');

class LingwoFieldsSettings extends LingwoSettings {
  protected $base_name = 'lingwo_fields';

  private $defaults = array(
    'fields' => array(),
  );

  public function getDefault($name) {
    return $this->defaults[$name];
  }

  public function getNames() {
    return array_map(array($this, 'name'), array_keys($this->defaults));
  }

  public function isValid($name) {
    return array_key_exists($name, $this->defaults);
  }

  // singleton boilerplate
  private static $settings = NULL;
  public static function get() {
    if (is_null(self::$settings)) {
      self::$settings = new self();
    }
    return self::$settings;
  }
}

class LingwoFields implements IteratorAggregate {
  public static $settings;

  private $entry;
  private $node;

  function __construct($entry) {
    $this->entry = $entry;
    $this->node =& $entry->getNode();

    if (!isset($this->node->lingwo_fields)) {
      $this->node->lingwo_fields = array();
    }
  }

  public function getFields() {
    $this->clean();
    $items = $this->node->lingwo_fields;
    $fields = array();
    foreach (lingwo_language_fields($this->node->language, $this->node->pos) as $name => $field) {
      if (isset($items[$name])) {
        $item = $items[$name];
        unset($items[$name]);
      }
      else {
        // setup some defaults from the definition
        $item = array(
          'type'      => $field['type'],
          'name'      => $name,
          'value'     => NULL,
          'automatic' => TRUE
        );
      }

      $item['definition'] = $field;

      $fields[$name] = $item;
    }
    $fields += $items;

    return $fields;
  }

  public function clean() {
    _lingwo_fields_clean($this->node->lingwo_fields);
  }

  public function getIterator() {
    return new ArrayIterator($this->getFields());
  }

  public function getEntry() {
    return $this->entry;
  }

  public function exists($name) {
    return isset($this->node->lingwo_fields[$name]);
  }

  public function getType($name) {
    if (isset($this->node->lingwo_fields[$name])) {
      return $this->node->lingwo_fields[$name]['type'];
    }
  }

  public function getOne($name) {
    if (isset($this->node->lingwo_fields[$name])) {
      return $this->node->lingwo_fields[$name]['value'];
    }
  }

  public function getMany($name) {
    if (isset($this->node->lingwo_fields[$name])) {
      if (isset($this->node->lingwo_fields[$name]['value'])) {
        $values = array($this->node->lingwo_fields[$name]['value']);
      }
      else {
        $values = array();
      }
      if (isset($this->node->lingwo_fields[$name]['alt'])) {
        $values = array_merge($values, $this->node->lingwo_fields[$name]['alt']);
      }
      return $values;
    }
  }

  public function set($name, $type, $value, $automatic=FALSE) {
    if (!isset($this->node->lingwo_fields[$name])) {
      $field = array(
        'name' => $name,
      );
      $this->node->lingwo_senses[$name] = $field;
    }

    $field =& $this->node->lingwo_fields[$name];
    $field['type']      = $type;
    $field['value']     = $value;
    $field['automatic'] = $automatic;
  }

  public function addForm($name, $value, $automatic=FALSE) {
    if (is_null($name)) {
      $name = '_noname_';
      $automatic = FALSE;
    }
    if (isset($this->node->lingwo_fields[$name])) {
      $this->node->lingwo_fields[$name]['alt'][] = $value;
    }
    else {
      $this->node->lingwo_fields[$name] = array(
        'name' => $name,
        'type' => 'form',
        'automatic' => $automatic
      );
      if ($name == '_noname_') {
        $this->node->lingwo_fields[$name]['alt'][] = $value;
      }
      else {
        $this->node->lingwo_fields[$name]['value'] = $value;
      }
    }
  }
}

LingwoFields::$settings = LingwoFieldsSettings::get();

