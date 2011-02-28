<?php
// $Id$

/**
 * @file
 * Admin pages.
 */


/**
 * Settings page
 */
function lingwo_entry_admin_settings() {
  $form = array();

  $settings = LingwoEntrySettings::get();

  foreach (node_get_types() as $type) {
    $type_options[$type->type] = $type->name;
  }

  $form[$settings->name('pos_allowed_values')] = array(
    '#type' => 'textarea',
    '#title' => t('Part of Speech Allowed Values'),
    '#default_value' => $settings->pos_allowed_values,
    '#description' => t('A list of allowed values for the "Part of Speech" field.  Put one part of speech on each line.  Use the pipe symbol if you want the part of speech to have an alternate display name, for example: <em>n|Noun</em>'),
    '#required' => TRUE,
  );

  $form[$settings->name('node_title_munging')] = array(
    '#type' => 'checkbox',
    '#title' => t('Add language and part of speech information to entry node titles'),
    '#default_value' => $settings->node_title_munging,
    '#description' => t('If you activate this option, language and part of speech information will be added to entry node titles.  To include the same information in more customized way, disable this option and add it in your theme.'),
  );

  $form[$settings->name('lookup_path')] = array(
    '#type' => 'textfield',
    '#title' => t('Lookup path'),
    '#default_value' => $settings->lookup_path,
    '#description' => t('The drupal path for dictionary lookup. Do not include a trailing slash. Leave empty to disable the lookup path.'),
    '#field_prefix' => url(NULL, array('absolute' => FALSE)) . (variable_get('clean_url', 0) ? '' : '?q=')
  );

  $form[$settings->name('redirect_node')] = array(
    '#type' => 'checkbox',
    '#title' => t('Redirect entry nodes to lookup path'),
    '#default_value' => $settings->redirect_node,
    '#description' => t('If you activate this option, by going to an entry nodes <em>node/nid</em> page, you will be redirected to the same page as <em>&lt;Lookup path&gt;/Headword</em>.'),
  );

  $form = system_settings_form($form);
  // Rebuild the menu after updating the settings.
  $form['#submit'][] = 'menu_rebuild';
  return $form;
}

