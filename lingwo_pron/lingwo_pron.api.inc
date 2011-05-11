<?php

/**
 * @file
 * API for lingwo_pron
 */

require_once(dirname(__FILE__) . '/../includes/settings.inc');

class LingwoPronSettings extends LingwoSettings {
  protected $base_name = 'lingwo_pron';

  private $defaults = array(
    'enabled_fields' => array('ipa', 'tag', 'audio'),
    'max_values' => -1,
    'pron_tag_allowed_values' => '',
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

class LingwoPron {
  /*
   *
   * Static properties/methods
   * =========================
   *
   */

  public static $settings;

  public static function getTagOptions($language, $translate = FALSE, $option_name = NULL) {
    return _lingwo_allowed_values(self::$settings, 'pron_tag', $language, $translate, $option_name);
  }

  /*
   *
   * Instance properties/methods.
   * ===========================
   *
   */

  private $entry;
  private $node;

  public function __construct($entry) {
    $this->entry = $entry;
    $this->node =& $entry->getNode();
  }

}

LingwoPron::$settings = LingwoPronSettings::get();

