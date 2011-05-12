<?php

/**
 * @file
 * The "widget" (form chucklet that goes on the node form)
 */

function _lingwo_pron_widget(&$form, &$form_state, $entry) {
  $element = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
  );

  $element['wrapper'] = array(
    '#prefix' => '<div id="edit-lingwo_pron-wrapper">',
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
      $tag_options = LingwoPron::getTagOptions($entry->language, TRUE);
      if (!empty($tag_options)) {
        $item['tag'] = array(
          '#type' => 'select',
          '#title' => t('Tag'),
          '#options' => array('' => t('None')) + $tag_options,
          '#default_value' => $pron['tag'],
        );
      }
    }

    $element['wrapper']['pron_list'][] = $item;
  }

  $_lingwo_pron = array(
    '#parents' => array('_lingwo_pron'),
    '#tree' => TRUE,
  );
  $_lingwo_pron['refresh'] = array(
    '#type' => 'button',
    '#value' => t('Refresh'),
    '#submit' => array('_lingwo_form_to_node'),
    '#ahah' => array(
      'path' => _lingwo_ahah_path(array('lingwo_pron', 'wrapper')),
      'event' => 'click',
      'wrapper' => 'edit-lingwo_pron-wrapper',
      'effect' => 'none',
      'progress' => array(
        'type' => 'none',
      ),
    ),
    '#attributes' => array('class' => 'no-js'),
    '#ahah_disable_validation' => TRUE,
  );

  $element['wrapper']['_lingwo_pron'] = $_lingwo_pron;

  drupal_add_js(drupal_get_path('module', 'lingwo_pron') . '/lingwo_pron.js');

  return $element;
}

