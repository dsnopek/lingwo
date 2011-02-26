<?php
// $Id$

/**
 * @file
 * For the admin pages.
 */

function lingwo_senses_admin_settings_form() {
  $settings =& LingwoSensesSettings::get();
  $form = array();

  $form[$settings->name('difference')] = array(
    '#type' => 'radios',
    '#title' => t('Sense differentiation'),
    '#description' => t('Controls how to tell senses apart'),
    '#options' => array(
      'definition'  => t('Every sense gets a definition'),
      'always'      => t('Every sense gets a few words describing how it is different than the other senses'),
      'morethanone' => t('Every sense gets a few words describing how it is different than the other senses, <strong>but only if there are two or more senses</strong>')
    ),
    '#default_value' => $settings->difference,
  );

  $form[$settings->name('example')] = array(
    '#type' => 'radios',
    '#title' => t('Examples'),
    '#description' => t('The style of examples for each sense'),
    '#options' => array(
      'single'   => t('Single line example.'),
      'multiple' => t('Multiple line example.  Can be used for multiple examples.'),
      'none'     => t('No example'),
    ),
    '#default_value' => $settings->example,
  );

  $form[$settings->name('relationship_allowed_values')] = array(
    '#type' => 'textarea',
    '#title' => t('Relationship Allowed Values'),
    '#default_value' => $settings->relationship_allowed_values,
    '#description' => t('A list of allowed values for the "Relationship" field.  Leave blank to disable relationships all together.'),
  );

  return system_settings_form($form);
}

