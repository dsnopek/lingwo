<?php
// $Id: $

/**
 * @file
 * API for lingwo_senses
 */

require_once(dirname(__FILE__) .'/../includes/settings.inc');

class LingwoSensesSettings extends LingwoSettings {
  protected $base_name = 'lingwo_senses'; 

  private $defaults = array(
    'difference' => 'definition',
    'example'    => 'single',
    'relationship_allowed_values' => '',
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

class LingwoSenses {
  /*
   * 
   * Static properties/methods
   * =========================
   *
   */

  public static $settings;

  public static function getRelationshipOptions($translate=FALSE, $option_name=NULL) {
    return _lingwo_allowed_values(self::$settings, 'relationship', $translation, $option_name);
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

    if (!isset($this->node->lingwo_senses)) {
      $this->node->lingwo_senses = array();
    }
  }

  /* Takes an entry and a trans and makes a list of trans senses in the appropriate
   * order, connected to the entry senses if one exists.
   */
  public function getTranslations() {
    $orig = $this->entry->sourceNode;
    // TODO: does this make sense?  Why support not having a translation source??
    $orig_senses = isset($orig) ? $orig->lingwo_senses : array();

    $trans_map = array();
    $trans_senses = $this->node->lingwo_senses;

    // build our internal map of id => sense
    if (!empty($trans_senses)) {
      foreach ($trans_senses as $trans_sense) {
        $trans_map[$trans_sense['id']] = $trans_sense;
      }
    }

    $senses = array();

    foreach ($orig_senses as $sense) {
      $trans_sense = $trans_map[$sense['id']];
      if (!isset($trans_sense)) {
        $trans_sense = array('id' => $sense['id']);
      }
      $trans_sense['orig_sense'] = $sense;
      
      $senses[] = $trans_sense;
      unset($trans_map[$sense['id']]);
    }

    // TODO: do we still need this?  Source entries without senses is no longer our thing.
    if (count($senses) == 0) {
      if ($trans_map && isset($trans_map['*'])) {
        $only_sense = $trans_map['*'];
      }
      else {
        $only_sense = array('id' => '*', 'data' => array());
      }
      $senses[] = $only_sense;
      unset($trans_map['*']);
    }

    foreach ($trans_map as $id => $trans_sense) {
      $senses[] = $trans_sense;
    }

    return $senses;
  }
}

LingwoSenses::$settings = LingwoSensesSettings::get();

