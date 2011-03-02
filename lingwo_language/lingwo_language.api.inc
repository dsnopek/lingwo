<?php
// $Id$

/**
 * @file
 * API functions for dealing with language definitions
 */

require_once(dirname(__FILE__) .'/../includes/settings.inc');

class LingwoLanguageSettings extends LingwoSettings {
  protected $base_name = 'language_language';

  private $defaults = array(
    'content_type' => 'lingwo_language',
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

class LingwoLanguage {
  public static $settings;

  public static function getValue($language, $name) {
    $res = db_query("SELECT llr.value FROM {lingwo_language} ll, {node} n, {lingwo_language_revisions} llr WHERE llr.vid = n.vid AND n.nid = ll.nid AND ll.language = '%s' AND llr.name = '%s'", $language, $name);
    if ($obj = db_fetch_object($res)) {
      return unserialize($obj->value);
    }
  }
}

LingwoLanguage::$settings = LingwoLanguageSettings::get();

