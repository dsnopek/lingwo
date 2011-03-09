<?php
// $Id$

/**
 * @file
 * For the admin pages.
 */

function lingwo_fields_admin_settings_form() {
  $settings =& LingwoFields::$settings;
  $form = _lingwo_language_form('_lingwo_fields_lingwo_language_form');
  return system_settings_form($form);
}

function _lingwo_fields_lingwo_language_form($language) {
  $v = $language->getValue('lingwo_fields_fields', LingwoFields::$settings->fields);

  $form = array(
    '#submit' => array('lingwo_fields_lingwo_language_form_submit'),
    '#validate' => array('lingwo_fields_lingwo_language_form_validate'),
  );
  $form['lingwo_fields_fields'] = array(
    '#title' => t('Fields'),
    '#type' => 'fieldset',
    '#tree' => TRUE,
  );

  // TODO: special permissions should be required for this!
  $form['lingwo_fields_fields']['javascript'] = array(
    '#title' => t('JavaScript'),
    '#type' => 'textarea',
    '#default_value' => $v['javascript'],
    '#description' => t('JavaScript code for the language definition.  This should atleast define the language\'s alphabet!'),
  );

  // make a list of all the pos/field collections
  $pos = array();
  $pos_labels = array();
  $pos_options = LingwoEntry::getPosOptions($language, TRUE);
  foreach ($pos_options as $key => $value) {
    if (isset($v['pos'][$key])) {
      $pos[$key] = $v['pos'][$key];
      unset ($v['pos'][$key]);
    }
    $pos_labels[$key] = $value;
  }
  $pos = $pos + (array)$v['pos'];

  // loop through the pos/field collections and build the form
  foreach ($pos as $pos_name => $fields) {
    $form['lingwo_fields_fields']['pos'][$pos_name] = array(
      '#type' => 'fieldset',
      '#title' => isset($pos_labels[$pos_name]) ? $pos_labels[$pos_name] : $pos_name,
    );
    foreach ($fields as $name => $field) {
      $form['lingwo_fields_fields']['pos'][$pos_name][$name] = _lingwo_fields_admin_field_form($name, $field);
    }
  }

  // form for adding a new field
  $add_field = _lingwo_fields_admin_field_form(t('Add field'), array());
  $add_field['#parents'] = array('_lingwo_fields_add_form');
  $add_field['#tree'] = TRUE;
  $add_field['pos'] = array(
    '#type' => 'select',
    '#title' => t('Part of Speech'),
    '#options' => $pos_options,
    '#weight' => -11,
  );
  $add_field['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#weight' => -10,
  );
  $add_field['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add field'),
    '#weight' => 10,
  );
  $form['lingwo_fields_fields']['add_field'] = $add_field;
  
  return $form;
}

function _lingwo_fields_admin_field_form($title, $field) {
  $element = array(
    '#type' => 'fieldset',
    '#title' => $title,
  );
  $element['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#default_value' => $field['label'],
  );
  $element['type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => array(
      'form'   => 'Form',
      'option' => 'Option',
      'class'  => 'Class',
    ),
    '#default_value' => $field['type'],
  );
  $element['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => $field['description'],
    '#description' => t('Help text to show the user when they are creating an entry with this form.'),
  );
  $element['options'] = array(
    '#type' => 'textarea',
    '#title' => t('Options'),
    '#default_value' => $field['options'],
  );
  // TODO: should only be shown with a special permission
  $element['automatic'] = array(
    '#type' => 'textarea',
    '#title' => t('Automatic JavaScript function'),
    '#default_value' => $field['automatic']
  );
  // TODO: should only be shown with a special permission
  $element['show'] = array(
    '#type' => 'textarea',
    '#title' => t('Show JavaScript function'),
    '#default_value' => $field['show']
  );

  return $element;
}

function lingwo_fields_lingwo_language_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#value'] == t('Add field')) {
    $values = $form_state['values']['_lingwo_fields_add_form'];

    $pos =  $values['pos'];
    $name = $values['name'];
    unset($values['pos']);
    unset($values['name']);

    $form_state['values']['lingwo_fields_fields']['pos'][$pos][$name] = $values;
  }

  unset($form_state['values']['_lingwo_fields_add_form']);
}

function lingwo_fields_lingwo_language_form_validate($form, &$form_state) {
  if ($form_state['clicked_button']['#value'] == t('Add field')) {
    $values = $form_state['values']['_lingwo_fields_add_form'];
    $name = $values['name'];
    $pos = $values['pos'];

    if (empty($name)) {
      form_set_error('_lingwo_field_add_form][name', t('!name field is required.',
        array('!name' => t('Name'))));
    }
    elseif (isset($form_state['values']['lingwo_fields_fields']['pos'][$pos][$name])) {
      form_set_error('_lingwo_field_add_form][name', t('We already have a field named @name for @pos',
        array('@name' => $name, '@pos' => $pos)));
      form_set_error('_lingwo_field_add_form][pos');
    }
  }
}

