<?php
// $Id$

/**
 * @file
 * Helper code for implementing AHAH
 */

function _lingwo_ahah_path($form_item=NULL) {
  return 'lingwo/ahah/'. (is_null($form_item) ? '' : implode('][', $form_item));
}

function _lingwo_ahah_callback($form_item=NULL) {
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];

  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);

  $node_form = FALSE;
  if (preg_match('/_node_form$/', $form_id)) {
    $node_form = TRUE;
    module_load_include('inc', 'node', 'node.pages');
  }

  $form_state['post'] = $form['#post'] = $_POST;
  $form['#post']['form_id'] = $form_id;
  $form['#programmed'] = $form['#redirect'] = FALSE;

  // prevents submit, not sure what other consequences this has
  $form_state['submitted'] = TRUE;

  if (!isset($_POST['op'])) {
    unset(
      $form['#validate'],
      $form['#submit'],
      $form_state['submit_handlers'],
      $form_state['validate_handlers']
    );

    // Disable #required and #element_validate validation.
    _lingwo_ahah_disable_validation($form);
  }

  drupal_process_form($form_id, $form, $form_state);
  if ($node_form) {
    $node = _lingwo_form_to_node($form, $form_state);
  }

  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

  $element = _lingwo_ahah_get_form_item($form, $form_item);
  unset($element['#prefix'], $element['#suffix']);

  $output = theme('status_messages') . drupal_render($element);
  drupal_json(array('status' => TRUE, 'data' => $output));
}

function _lingwo_ahah_disable_validation(&$form) {
  foreach (element_children($form) as $child) {
    $form[$child]['#validated'] = TRUE;
    _lingwo_ahah_disable_validation($form[$child]);
  }
}

function _lingwo_ahah_get_form_item($form, $form_item) {
  if (is_null($form_item)) {
    return $form;
  }

  $parents = explode('][', $form_item);
  foreach($parents as $parent) {
    if (isset($form[$parent])) {
      $form = $form[$parent];
    }
    else {
      return NULL;
    }
  }

  return $form;
}

