<?php

/**
 * @file
 * The "widget" (form chucklet that goes on the node form)
 */

function _lingwo_senses_widget_original($node) {
  // TODO: should we just pass in an entry?
  $entry = LingwoEntry::fromNode($node);

  $difference = LingwoSenses::$settings->difference;

  $element = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
  );

  $element['wrapper'] = array(
    '#prefix' => '<div id="edit-lingwo_senses-wrapper">',
    '#suffix' => '</div>',
  );

  $element['wrapper']['senses'] = array(
    '#theme' => 'lingwo_senses_original_form',
    '#parents' => array('lingwo_senses'),
  );

  // we count it as empty if there is less than 2 senses!
  $index = 0;
  foreach ($entry->senses as $sense) {
    $row['id'] = array(
      '#type' => 'hidden',
      '#value' => $sense->id,
    );

    if ($difference != 'morethanone' || count($entry->senses) > 1) {
      // if our difference mode is 'morethanone', only edit the
      // difference when there are multiple senses
      $row['data']['difference'] = array(
        '#title' => $difference == 'definition' ? t('Definition') : t('Difference'),
        '#type' => 'textfield',
        '#default_value' => $sense->difference,
        '#maxlength' => 255,
      );
    }
    else {
      // but preserve the value for if we delete down to one sense
      if ($sense->difference) {
        $row['data']['difference'] = array(
          '#title' => t('Difference'),
          '#type' => 'item',
          '#value' => $sense->difference,
        );
      }
    }

    if (LingwoSenses::$settings->example != 'none') {
      $row['data']['example'] = array(
        '#title' => t('Example'),
        '#type' => (LingwoSenses::$settings->example == 'multiple') ? 'textarea' : 'textfield',
        '#default_value' => implode("\n", $sense->example),
        '#maxlength' => 255,
      );
      if (LingwoSenses::$settings->example == 'multiple') {
        $row['data']['example']['#description'] = t('You either put multiple examples, one on each line.  Or a single multi-line example.');
      }
    }

    $relationship_options = LingwoSenses::getRelationshipOptions($node->language, TRUE);
    if (!empty($relationship_options)) {
      // always have the ability to say it "is" a relationship to another entry
      $row['data']['is'] = array(
        '#type' => 'fieldset',
        '#title' => t('Relationship'),
        '#collapsible' => TRUE,
        '#collapsed' => empty($sense->is['relationship']),
        //'#prefix' => '<div class="inline-controls clear-block">',
        //'#suffix' => '</div>',
      );
      $is_relationship_options = array('' => '-') + $relationship_options;
      $row['data']['is']['relationship'] = array(
        '#title' => 'is',
        '#type'  => 'select',
        // TODO: should come from lingwo_language (or indirectly from a hook)
        '#options' => $is_relationship_options,
        '#default_value' => $sense->is['relationship'],
        '#attributes' => array('class' => 'is-relationship'),
      );
      $row['data']['is']['headword'] = array(
        '#title' => 'to',
        '#type'  => 'textfield',
        '#default_value' => $sense->is['headword'],
      );
      $row['data']['is']['pos'] = array(
        '#type'  => 'select',
        '#options' => LingwoEntry::getPosOptions($node->language, TRUE),
        '#default_value' => $sense->is['pos'],
      );
    }

    $row['weight'] = array(
      '#type' => 'weight',
      // TODO: Delta should probably be equal to the number of definitions?
      '#delta' => 50,
      '#default_value' => $index,
    );

    if (count($entry->senses) > 1) {
      $row['remove_sense'] = array(
        '#type' => 'submit',
        '#value' => t('Remove Sense #@num', array('@num' => $index + 1)),
        '#submit' => array('_lingwo_senses_remove_submit'),
      );
    }
    else {
      $row['message'] = array(
        '#value' => "<em>* " . t('This entry has only one sense.') . "</em>",
      );
    }

    $element['wrapper']['senses'][$index] = $row;
    $index ++;
  }
  $element['wrapper']['add_sense'] = array(
    '#type' => 'submit',
    '#value' => count($entry->senses) > 1 ? t('Add another sense') : t('Setup multiple senses'),
    '#submit' => array('_lingwo_senses_add_submit'),
  );
  // for reloading the current values
  if (!$node->nid) {
    $element['wrapper']['refresh'] = array(
      '#type' => 'submit',
      '#value' => t('Refresh'),
      '#submit' => array('_lingwo_form_to_node'),
      '#ahah' => array(
        'path' => _lingwo_ahah_path(array('lingwo_senses', 'wrapper')),
        'wrapper' => 'edit-lingwo_senses-wrapper',
        'effect' => 'none',
        'progress' => array(
          'type' => 'none',
        ),
      ),
      '#parents' => array('_lingwo_senses', 'refresh'),
      '#attributes' => array('class' => 'no-js'),
      '#ahah_disable_validation' => TRUE,
    );
  };

  return $element;
}

function _lingwo_senses_widget_translation($node) {
  // TODO: should we just pass an entry in?
  $entry = LingwoEntry::fromNode($node);

  $element = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#theme' => 'lingwo_senses_translation_form',
  );

  $index = 0;
  foreach ($entry->senses as $sense) {
    $sense_form = array();
    $sense_form['id'] = array(
      '#type' => 'hidden',
      '#value' => $sense->id,
    );
    if ($sense->hasOriginal()) {
      if ($sense->difference || !empty($sense->is['relationship'])) {
        $sense_form['data']['difference'] = array(
          '#type' => 'item',
          '#title' => LingwoSenses::$settings->difference == 'definition' ? t('Definition') : t('Difference'),
          '#value' => !empty($sense->is['relationship']) ? theme('lingwo_senses_relationship', $node, $sense) : check_plain($sense->difference),
        );
      }
      if (!empty($sense->example)) {
        $sense_form['data']['example'] = array(
          '#type' => 'item',
          '#title' => t('Example'),
          '#value' => implode('<br />', array_filter($sense->example, 'check_plain')),
        );
        $sense_form['data']['example_translation'] = array(
          '#type' => (LingwoSenses::$settings->example == 'multiple') ? 'textarea' : 'textfield',
          '#title' => t('Translation of example'),
          '#default_value' => implode("\n", $sense->example_translation),
          '#maxlength' => 255,
        );
        if (LingwoSenses::$settings->example == 'multiple') {
          $sense_form['data']['example_translation']['#description'] = t('Match each line of the translation with a line in the original.');
        }
      }
    }

    if (empty($sense->is['relationship'])) {
      $sense_form['data']['translation'] = array(
        '#parents' => array('lingwo_senses', $index, 'data'),
        '#prefix' => "<div id=\"edit-lingwo-senses-$index-data-translation-wrapper\">",
        '#suffix' => '</div>',
      );
      if (count($entry->senses) > 1) {
        $sense_form['data']['translation']['clue'] = array(
          '#type' => 'textfield',
          '#title' => t('Clue'),
          '#description' => t('A couple words that will clue the reader to how this sense is different.'),
          '#default_value' => $sense->clue,
          '#maxlength' => 255,
        );
      }

      $sense_form['data']['translation']['trans'] = array(
        '#prefix' => "<div id=\"edit-lingwo-senses-$index-data-translation-trans-wrapper\">",
        '#suffix' => "</div>",
      );
      for ($tindex = 0; $tindex < max(3, count($sense->trans)); $tindex++) {
        $sense_form['data']['translation']['trans'][$tindex] = array(
          '#type' => 'textfield',
          '#default_value' => $sense->trans[$tindex]
        );
      }
      $sense_form['data']['translation']['trans']['0']['#title'] = t('Translation');
      $sense_form['data']['translation']['trans'][$tindex-1]['#description'] = t('Put only <em>1 translation</em> in each field.');

      $sense_form['data']['translation']['no_equivalent'] = array(
        '#title' => t('No equivalent in this language'),
        '#type'  => 'checkbox',
        '#default_value' => $sense->no_equivalent,
        '#attributes' => array('class' => 'no-equivalent-checkbox'),
      );

      if (count($entry->senses) > 1) {
        $same_as_options = array('' => '-');
        $same_as_count = 1;
        foreach ($entry->senses as $sense2) {
          if (!empty($sense2->id) && $sense2->id != $sense->id) {
            $same_as_options[$sense2->id] = t('Sense #@sense_num', array('@sense_num' => $same_as_count));
          }
          $same_as_count++;
        }

        $sense_form['data']['same_as'] = array(
          '#title' => t('Same as'),
          '#type' => 'select',
          '#options' => $same_as_options,
          '#default_value' => !empty($sense->same_as) ? $sense->same_as : '',
          '#attributes' => array('class' => 'same-as-select'),
        );
      }
    }

    if (!$sense->hasOriginal()) {
      $sense_form['old_sense_message'] = array(
        '#prefix' => '<div class="lingwo-sense-old-message">* ',
        '#value' => t('This translation is for a sense that has been removed from the entry.'),
        '#suffix' => '</div>',
      );
      $sense_form['retire_sense'] = array(
        '#type' => 'checkbox',
        '#title' => t('Retire this obsolete sense.'),
      );
    }

    $element[$index++] = $sense_form;
  }

  return $element;
}

function _lingwo_senses_add_submit($form, &$form_state) {
  $node = &_lingwo_form_to_node($form, $form_state);
  array_push($node['lingwo_senses'], array('id' => _lingwo_make_uuid(), 'data' => array()));
}

function _lingwo_senses_remove_submit($form, &$form_state) {
  // We have to remove first, so that changing the order doesn't change
  // which item gets removed.
  $delta = $form_state['clicked_button']['#parents'][1];
  array_splice($form_state['values']['lingwo_senses'], $delta, 1);

  // get the senses onto the $form_state.
  _lingwo_form_to_node($form, $form_state);
}

