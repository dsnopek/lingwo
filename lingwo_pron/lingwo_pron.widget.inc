<?php

/**
 * @file
 * The "widget" (form chucklet that goes on the node form)
 */

function _lingwo_pron_widget($entry) {
  $element = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
  );

  $element['wrapper'] = array(
    '#prefix' => '<div id="edit-lingwo_senses-wrapper">',
    '#suffix' => '</div>',
  );

  $element['wrapper']['pron_list'] = array(
    //'#theme' => 'lingwo_senses_original_form',
    '#parents' => array('lingwo_pron'),
  );

  $pron_list = $entry->pron;
  if (count($pron_list) == 0) {
    $pron_list = array(array());
  }

  $enabled_fields = array();
  foreach (LingwoPron::$settings->enabled_fields as $name) {
    $enabled_fields[$name] = TRUE;
  }

  foreach ($pron_list as $pron) {
    $item = array();
    if ($enabled_fields['ipa']) {
      $item['ipa'] = array(
        '#type' => 'textfield',
        '#title' => t('IPA'),
        '#default_value' => $pron['ipa'],
      );
    }
    if ($enabled_fields['tag']) {
      $item['tag'] = array(
        '#type' => 'select',
        '#title' => t('Tag'),
        '#options' => array('' => t('None')) + LingwoPron::getTagOptions($entry->language, TRUE),
        '#default_value' => $pron['tag'],
      );
    }

    $element['wrapper']['pron_list'][] = $item;
  }

  return $element;
}

