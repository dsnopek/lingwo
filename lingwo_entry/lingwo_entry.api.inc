<?php
// $Id$

/**
 * @file
 * API functions for dealing with entries
 */

require_once(dirname(__FILE__) .'/../includes/settings.inc');

class LingwoEntrySettings extends LingwoSettings {
  protected $base_name = 'lingwo_entry';

  private $defaults = array(
    'lookup_path' => 'dictionary',
    'node_title_munging' => array('language' => TRUE, 'pos' => TRUE),
    'redirect_node' => TRUE,
    'content_type' => 'lingwo_entry',
    'pos_allowed_values' => "noun|Noun\nadjective|Adjective\nverb|Verb\nadverb|Adverb\nother|Other",
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

class LingwoEntry {
  /*
   * 
   * Static properties/methods
   * =========================
   *
   */

  public static $settings;
  private static $MAP_NODE = array(
    'headword'       => 'title',
    'pos'            => 'pos',
    'nid'            => 'nid',
    'language'       => 'language',
    'targetLanguage' => 'language',
  );
  private static $MAP_FUNC = array(
    'node'           => 'getNode',
    'sourceLanguage' => 'getSourceLanguage',
    'sourceNode'     => 'getTranslationSource',
  );
  private static $MAP_EXTS = NULL;

  public static function isEntryNode($node) {
    return $node->type == self::$settings->content_type;
  }

  private static function getTranslationForNid($nid, $target_lang) {
    if (module_exists('translation')) {
      $trans = translation_node_get_translations($nid);
      if ($trans[$target_lang]) {
        return $trans[$target_lang]->nid;
      }
    }
  }

  public static function lingwo2nid($id) {
    list($source_lang, $headword) = explode('/', $id);
    list($source_lang, $target_lang) = explode('-', $source_lang);
    if (preg_match('/([^\(]+)\(([^\)]+)\)/', $headword, $matches)) {
      $headword = trim($matches[1]);
      $pos  = $matches[2];
    }

    $nid = self::key2nid($source_lang, $pos, $headword);

    if ($target_lang) {
      $nid = self::getTranslationForNid($nid, $target_lang);
    }

    return $nid;
  }

  public static function hash2nid($hash, $target_lang=NULL, $fallback=FALSE) {
    $res = db_query("SELECT nid FROM {lingwo_entry} WHERE entry_hash = '%s'", $hash);
    if ($obj = db_fetch_object($res)) {
      $nid = $obj->nid;
      if (!is_null($target_lang)) {
        $tnid = self::getTranslationForNid($nid, $target_lang);
        $nid = empty($tnid) ? ($fallback ? $nid : NULL) : $tnid;
      }
      return $nid;
    }
  }

  public static function key2nid($language, $pos, $headword) {
    $res = db_query("SELECT nid FROM {lingwo_entry} ".
      "WHERE language = '%s' AND pos = '%s' AND headword = '%s'",
      $language, $pos, $headword);
    if ($obj = db_fetch_object($res)) {
      return $obj->nid;
    }
    return NULL;
  }

  public static function searchHeadword($text, $options=array()) {
    $sql = "SELECT nid, headword, pos, language FROM {lingwo_entry}";
    $where = " WHERE headword = '%s'";
    $args = array($text);

    if (!empty($options['language'])) {
      $where .= " AND language = '%s'";
      $args[] = $options['language'];
    }
    if (!empty($options['pos'])) {
      $where .= " AND pos = '%s'";
      $args[] = $options['pos'];
    }

    $results = array();
    $res = db_query($sql . $where, $args);
    while ($obj = db_fetch_object($res)) {
      $obj->exact = TRUE;
      $results[] = $obj;
    }

    return $results;
  }

  public static function search($text, $options=array()) {
    // first get all the headword matches
    $results = self::searchHeadword($text, $options);

    // then get all the fields matches
    if (module_exists('lingwo_fields')) {
      $ret = LingwoFields::searchForms($text, $options);
      $results = array_merge($results, $ret);
    }

    return $results;
  }

  public static function getPosOptions($language, $translate=FALSE, $option_name=NULL) {
    return _lingwo_allowed_values(self::$settings, 'pos', $language, $translate, $option_name);
  }

  /*
   * 
   * Constructors
   * ============
   *
   */

  protected function __construct(&$node) {
    $this->node =& $node;
  }

  public static function fromNode(&$node) {
    if (!$node || !self::isEntryNode($node)) {
      return NULL;
    }
    if (is_a($node, 'LingwoEntry')) {
      return $node;
    }
    return new LingwoEntry($node);
  }

  public static function fromNid($nid) {
    if ($nid) {
      return self::fromNode(node_load($nid));
    }
  }

  public static function fromLingwoId($id) {
    return self::fromNid(self::lingwo2nid($id));
  }

  public static function fromKey($language, $pos, $headword) {
    return self::fromNid(self::key2nid($language, $pos, $headword));
  }

  public static function fromHash($hash, $target_lang=NULL, $fallback=FALSE) {
    return self::fromNid(self::hash2nid($hash, $target_lang, $fallback));
  }

  /*
   *
   * Instance properties/methods.
   * ===========================
   *
   */

  private $node;
  private $sourceNode;
  private $exts = array();
 
  public function &getNode() {
    return $this->node;
  }

  public function __get($name) {
    $x = self::$MAP_NODE[$name];
    if (!empty($x)) {
      return $this->node->$x;
    }

    $x = self::$MAP_FUNC[$name];
    if (!empty($x)) {
      return call_user_func(array($this, $x));
    }

    if (is_null(self::$MAP_EXTS)) {
      self::$MAP_EXTS = module_invoke_all('lingwo_entry_properties');
    }
    $x = self::$MAP_EXTS[$name];
    if (!empty($x)) {
      if (!isset($this->exts[$name])) {
        if (class_exists($x['class'])) {
          $x = $x['class'];
          $this->exts[$name] = new $x($this);
        }
        elseif (is_callable($x['callable'])) {
          // we don't cache callables!  They might need to be calculated every time
          return call_user_func($x['callable'], $this);
        }
      }
      return $this->exts[$name];
    }
  }

  public function isTranslation() {
    return _lingwo_is_translation($this->node);
  }

  public function getTranslationSource() {
    if (isset($this->sourceNode)) {
      return $this->sourceNode;
    }

    $node = $this->node;
    if (isset($node->translation_source)) {
      return $node->translation_source;
    }

    if ($this->isTranslation()) {
      $this->sourceNode = node_load($node->tnid);
      return $this->sourceNode;
    }

    return NULL;
  }

  public function getTranslation($target_lang) {
    if (!$this->isTranslation()) {
      return self::fromNid(self::getTranslationForNid($this->node->nid, $target_lang));
    }
  }

  public function getLanguages($for_humans=FALSE) {
    $node = $this->node;
    if ($source_node = $this->getTranslationSource()) {
      $res = array($source_node->language, $node->language);
    }
    else {
      $res = array($node->language);
    }

    if ($for_humans) {
      $languages = language_list();
      for ($i = 0; $i < count($res); $i++) {
        // TODO: we need to translate the names to the current interface!
        $res[$i] = $languages[$res[$i]]->name;
      }
    }

    return $res;
  }

  public function getSourceLanguage($for_humans=FALSE) {
    $languages = $this->getLanguages($for_humans);
    return $languages[0];
  }

  public function getPos($for_humans=FALSE) {
    $node = $this->node;
    if ($for_humans) {
      return self::getPosOptions($this->getSourceLanguage(), TRUE, $node->pos);
    }

    return $node->pos;
  }

  public function getLingwoId() {
    return implode('/', array(
      implode('-', $this->getLanguages()),
      $this->node->title)) .'('. $this->getPos() .')';
  }

  public function getPath() {
    $node = $this->node;

    $lookup_path = self::$settings->lookup_path;
    if (!empty($lookup_path)) {
      return implode('/', array(
        $lookup_path,
        $this->getLingwoId(),
      ));
    }

    return isset($node->alias) ? $node->alias : ('node/'. $node->nid);
  }
};

LingwoEntry::$settings = LingwoEntrySettings::get();

/*
 *
 * Internal functions.
 *
 */

class _LingwoLanguageNull {
  public function hasValue($name) {
    return FALSE;
  }
  public function getValue($name, $default=NULL) {
    return $default;
  }
}

// Gets the form from hook_lingwo_language for the given module
function _lingwo_language_form($func=NULL) {
  $lang = new _LingwoLanguageNull();
  $form = $func($lang);

  // if we are using lingwo_language, tell the user these are just defaults
  if (module_exists('lingwo_language')) {
    foreach(array_keys($form) as $key) {
      if (isset($form[$key]['#type']) && $form[$key]['#type'] != 'markup') {
        if (!empty($form[$key]['#description'])) {
          $form[$key]['#description'] .= ' ';
        }
        $form[$key]['#description'] .= '<strong>'. t('Configured per language in its definition.  These are the defaults for new language definitions.') .'</strong>';
      }
    }
  }

  return $form;
}

function _lingwo_language_value($settings, $name, $language) {
  $value = NULL;
  if (module_exists('lingwo_language') && !empty($language)) {
    if (is_string($language)) {
      $language = LingwoLanguage::fromCode($language);
    }
  }
  if (!empty($language) && !is_a($language, '_LingwoLanguageNull')) {
    $value = $language->getValue($settings->name($name));
  }
  if (is_null($value)) {
    $value = $settings->__get($name);
  }
  return $value;
}

function _lingwo_name_value_list($text) {
  $list = explode("\n", $text);
  $list = array_map('trim', $list);
  $list = array_filter($list, 'strlen');
  $options = array();
  foreach ($list as $opt) {
    if (strpos($opt, '|') !== FALSE) {
      list ($key, $value) = explode('|', $opt);
    }
    else {
      $key = $value = $opt;
    }
    $options[$key] = $value;
  }
  return $options;
}

function _lingwo_allowed_values($settings, $name, $language, $translate=FALSE, $option_name=NULL) {
  $list = _lingwo_name_value_list(_lingwo_language_value($settings, $name .'_allowed_values', $language));

  // Only translate if we have i18nstrings
  $translate = $translate && module_exists('i18nstrings');

  $options = array();
  foreach ($list as $key => $value) {
    // if we are only looking for one key, skip all the rest
    if (!is_null($option_name) && $option_name != $key) {
      continue;
    }

    if ($translate) {
      $value = i18nstrings('lingwo:field:'. $name .':option_'. $key, $value);
    }

    if ($option_name == $key) {
      return $value;
    }

    $options[$key] = $value;
  }

  if (!is_null($option_name)) {
    // we requested an option but it isn't here!  Return the original.
    return $option_name;
  }

  asort($options);

  return $options;
}

// Returns the top-level project path
function _lingwo_project_path() {
  return dirname(__FILE__) .'/../';
}

// Helper for generating UUIDs
function _lingwo_make_uuid() {
  static $uuid = NULL;
  
  // use the OSSP uuid module (we have to test for uuid_make because the PECL uuid
  // module also defines the uuid_create function)
  if (is_null($uuid) && function_exists('uuid_make')) {
    uuid_create(&$uuid);
  }
  if (!is_null($uuid)) {
    uuid_make($uuid, UUID_MAKE_V4);
    uuid_export($uuid, UUID_FMT_STR, &$uuidstring);
    return trim($uuidstring);
  }

  // use the PECL uuid module
  if (function_exists('uuid_create')) {
    return uuid_create(UUID_TYPE_DEFAULT);
  }

  // use the database to generate a UUID
  if ($GLOBALS['db_type'] != 'pgsql') {
    return db_result(db_query('SELECT UUID()'));
  }

  // Copied graciously from the 'uuid' module.
  // Fall back to generating a Universally Unique IDentifier version 4.
  // Base on http://php.net/manual/en/function.uniqid.php#65879
  // The field names refer to RFC 4122 section 4.1.2.
  return sprintf('%04x%04x-%04x-%03x4-%04x-%04x%04x%04x',
    // 32 bits for "time_low".
    mt_rand(0, 65535), mt_rand(0, 65535),
    // 16 bits for "time_mid".
    mt_rand(0, 65535),
    // 12 bits before the 0100 of (version) 4 for "time_hi_and_version".
    mt_rand(0, 4095),
    bindec(substr_replace(sprintf('%016b', mt_rand(0, 65535)), '01', 6, 2)),
    // 8 bits, the last two of which (positions 6 and 7) are 01, for "clk_seq_hi_res"
    // (hence, the 2nd hex digit after the 3rd hyphen can only be 1, 5, 9 or d)
    // 8 bits for "clk_seq_low" 48 bits for "node".
    mt_rand(0, 65535), mt_rand(0, 65535), mt_rand(0, 65535)
  );
}

// Helper for implementing AHAH
function &_lingwo_form_to_node(&$form, &$form_state) {
  $form['#submit'] = array('');
  $node = node_form_submit_build_node($form, $form_state);

  // hack to stop taxonomy from resetting when the form is rebuilt
  //$form_state['node']['taxonomy'] = taxonomy_preview_terms($node);

  return $form_state['node'];
}

// Helper: returns TRUE if the given node is a translation
function _lingwo_is_translation($node) {
  return isset($node->translation_source) ||
    ($node->nid && $node->tnid != 0 && $node->nid != $node->tnid);
}

