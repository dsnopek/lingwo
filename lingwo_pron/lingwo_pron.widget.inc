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
    '#theme' => 'lingwo_pron_form',
    '#parents' => array('lingwo_pron'),
  );

  $pron_list = $entry->pron->getItems();
  foreach ($pron_list as $index => $item) {
    // remove the items marked for removal
    if (!empty($form_state['values']['lingwo_pron'][$index]['remove'])) {
      unset($pron_list[$index]);
    }
  }
  if (count($pron_list) == 0) {
    $pron_list = array(array());
  }
  if ($form_state['clicked_button']['#value'] == t('Add another item')) {
    $pron_list[] = array();
  }

  $enabled_fields = array();
  foreach (LingwoPron::$settings->enabled_fields as $name) {
    $enabled_fields[$name] = TRUE;
  }

  if ($enabled_fields['audio']) {
    $form['#attributes']['enctype'] = 'multipart/form-data';
  }

  $ahah = array(
    'path' => _lingwo_ahah_path(array('lingwo_pron', 'wrapper')),
    //'event' => 'click',
    'wrapper' => 'edit-lingwo_pron-wrapper',
    'effect' => 'none',
  );

  foreach ($pron_list as $index => $pron) {
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
    if ($enabled_fields['audio']) {
      if (empty($pron['audio'])) {
        $upload_name = implode('_', array('lingwo_pron', 'audio', $index));
        $item['audio'] = array(
          '#type' => 'file',
          '#title' => t('Audio file'),
          '#name' => "files[$upload_name]",
          '#size' => 22,
        );
        $item['upload'] = array(
          '#type' => 'submit',
          '#value' => t('Upload'),
          '#submit' => array('_lingwo_pron_widget_submit'),
          '#delta' => $index,
          '#upload_name' => $upload_name,
          '#attributes' => array('class' => 'lingwo-pron-upload'),
          //'#ahah' => $ahah,
        );
      }
      else {
        $item['audio'] = array(
          '#type' => 'value',
          '#value' => $pron['audio'],
        );
        $item['audio_display'] = array(
          '#type' => 'item',
          '#title' => t('Audio'),
          '#value' => l(t('Listen'), $pron['audio']->filepath),
        );
      }
    }

    $item['weight'] = array(
      '#title' => t('Weight'),
      '#type' => 'weight',
      '#delta' => 10,
      '#default_value' => $index,
    );
    $item['remove'] = array(
      '#type' => 'checkbox',
      '#title' => t('Remove'),
      '#attributes' => array('class' => 'lingwo-pron-remove'),
    );

    $element['wrapper']['pron_list'][] = $item;
    $index++;
  }

  $_lingwo_pron = array(
    '#parents' => array('_lingwo_pron'),
    '#tree' => TRUE,
  );
  $_lingwo_pron['refresh'] = array(
    '#type' => 'submit',
    '#value' => t('Refresh'),
    '#submit' => array('_lingwo_form_to_node'),
    '#ahah' => $ahah,
    '#attributes' => array('class' => 'no-js'),
    '#ahah_disable_validation' => TRUE,
  );
  $_lingwo_pron['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add another item'),
    '#submit' => array('_lingwo_form_to_node'),
    '#ahah' => $ahah,
    '#ahah_disable_validation' => TRUE,
  );

  $element['wrapper']['_lingwo_pron'] = $_lingwo_pron;

  drupal_add_js(drupal_get_path('module', 'lingwo_pron') . '/lingwo_pron.js');

  return $element;
}

function _lingwo_pron_widget_submit($form, &$form_state) {
  $node = _lingwo_form_to_node($form, $form_state);

  // make the upload into a temporary file
  $upload_name = $form_state['clicked_button']['#upload_name'];
  // TODO: we need to configure where the files are placed and how they are named
  $file = file_save_upload($upload_name, array(), file_directory_path(), FILE_EXISTS_RENAME);

  // save the file to the node
  $delta = $form_state['clicked_button']['#delta'];
  $form_state['node']['lingwo_pron'][$delta]['audio'] = $file;
}

