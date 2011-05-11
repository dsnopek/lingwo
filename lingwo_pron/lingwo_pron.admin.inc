<?php

/**
 * @file
 * For the admin pages.
 */

function lingwo_pron_admin_settings_form() {
  $settings =& LingwoPronSettings::get();
  $form = array();

  $form[$settings->name('enabled_fields')] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enabled fields'),
    '#options' => array(
      'ipa'  => t('IPA: International Phonetic Alphabet'),
      'tag'  => t('Tag: Marks the pronuncation in some way (for example, dialect: US or UK)'),
      'audio' => t('Audio: An audio file demonstrating the pronuncation'),
    ),
    '#default_value' => $settings->enabled_fields,
  );

  // merge in the relationship values from our hook_lingwo_language_form().
  $form = array_merge($form, _lingwo_language_form('lingwo_pron_lingwo_language_form'));

  return system_settings_form($form);
}

