<?php
// $Id: $

/**
 * @file
 * Theme functions for lingwo_senses
 */

function theme_lingwo_senses_original_form($form) {
  drupal_add_tabledrag('lingwo-definition-table', 'order', 'sibling', 'sense-weight');

  $elements = element_children($form);
  if (empty($elements)) {
    return '';
  }

  $header = array(
    '',
    array(
      'data' => t('Sense'),
      'width' => '100%',
    ),
    t('Weight'),
  );

  if (count($elements) > 1) {
    $header[] = t('Operations');
  }

  $rows = array();
  foreach (element_children($form) as $key) {
    $element = &$form[$key];
    $element['weight']['#attributes']['class'] = 'sense-weight';

    $weight = drupal_render($element['weight']);
    $delete = drupal_render($element['remove_sense']);

    $row = array();
    $row[] = '';
    $row[] = drupal_render($element);
    $row[] = $weight;

    if (count($elements) > 1) {
      $row[] = $delete;
    }

    $row = array('data' => $row);
    $row['class'] = !empty($row['class']) ? $row['class'] .' draggable' : 'draggable';
    $rows[] = $row;
  }

  return theme('table', $header, $rows, array('id' => 'lingwo-definition-table'));
}

function theme_lingwo_senses_translation_form($form) {
  $header = array(
    t('No.'),
    t('Sense'),
  );

  $rows = array();
  $elements = element_children($form);
  foreach ($elements as $key) {
    $row = array();
    $row[] = ($key + 1);
    $row[] = drupal_render($form[$key]);

    $rows[] = $row;
  }

  return theme('table', $header, $rows);
}

function theme_lingwo_senses_original($node) {
  $items = $node->lingwo_senses;
  if (!empty($items)) {
    $output .= '<ol class="lingwo-senses">';
    foreach ($items as $sense) {
      $sense_id = $sense['id'];
      $sense = $sense['data'];

      $output .= '<li class="lingwo-sense lingwo-sense-id-'. $sense_id .'">';
      if (!empty($sense['is']['relationship'])) {
        $output .= '<span class="lingwo-sense-relationship">'. theme('lingwo_senses_relationship', $node, $sense) .'</span>';
      }
      elseif ($sense['difference']) {
        $output .= '<span class="lingwo-sense-difference">'. check_plain($sense['difference']) .'</span>';
      }
      else {
        $output .= '<span class="lingwo-sense-no-difference">(';
        $output .= t('No difference given');
        $output .= ')</span>';
      }
      if ($sense['example'] && LingwoSenses::$settings->example != 'none') {
        $output .= ': <blockquote class="lingwo-sense-example">'. (LingwoSenses::$settings->example == 'multiple' ? str_replace("\n", "<br />", check_plain($sense['example'])) : check_plain($sense['example'])) .'</blockquote>';
      }
      $output .= '</li>';
    };
    $output .= '</ol>';
  }
  else {
    $output .= '<p>'. t('No sense distinctions.') .'</p>';
  }

  drupal_add_css(drupal_get_path('module', 'lingwo_senses') .'/lingwo_senses.css');
  return $output;
}

function theme_lingwo_senses_translation($node, $items=NULL) {
  $output = '';
  $output .= '<ol class="lingwo-senses">';

  if (is_null($items)) {
    $items = LingwoEntry::fromNode($node)->senses->getTranslations();
  }

  # group the ids together
  $ids = array();
  foreach ($items as $sense) {
    if ($sense['data']['same_as']) {
      $ids[$sense['data']['same_as']][] = $sense['id'];
    }
    else {
      $ids[$sense['id']][] = $sense['id'];
    }
  }

  $without_translations = 0;
  foreach ($items as $sense) {
    if (!empty($sense['data']['same_as'])) {
      continue;
    }
    $orig_sense = $sense['orig_sense'];
    $sense_id = $sense['id'];
    $sense = $sense['data'];

    $values = array();
    if ($sense['no_equivalent']) {
      $values[] = '<strong><span class="lingwo-sense-no-equivalent">'. t('No equivalent in this language') .'</span></strong>';
    }
    elseif (!empty($orig_sense['data']['is']['relationship'])) {
      $values[] = theme('lingwo_senses_relationship', $node, $orig_sense['data']);
    }
    elseif (!empty($sense['trans'])) {
      foreach ($sense['trans'] as $value) {
        if (!empty($value)) {
          // TODO: re-instate this!
          //$values[] = l($value, 'entry/'. $trans->target_lang .'/'. $entry->pos .'/'. $value);
          $values[] = check_plain($value);
        }
      }
    }
    if (empty($values)) {
      //$output .= t('No translation');
      $without_translations++;
      continue;
    }

    $output_ids = array();
    foreach ($ids[$sense_id] as $id) {
      $output_ids[] = 'lingwo-sense-id-'. $id;
    }
    $output_ids = implode(' ', $output_ids);

    $output .= '<li class="lingwo-sense '. $output_ids .'">';
    if ($sense['clue']) {
      $output .= '<span class="lingwo-sense-clue">('. check_plain($sense['clue']) .')</span> ';
    }

    $output .= join(', ', $values);

    if (!empty($orig_sense['data']['example']) && LingwoSenses::$settings->example != 'none') {
      $output .= '<blockquote class="lingwo-sense-example">';
      if (LingwoSenses::$settings->example == 'single' && empty($sense['example_translation'])) {
        $output .= check_plain($orig_sense['data']['example']);
      }
      else {
        $example_lines = array_map(NULL, explode("\n", $orig_sense['data']['example']), explode("\n", $sense['example_translation']));
        $output .= '<table>';
        foreach ($example_lines as $line) {
          $output .= '<tr><td width="50%">'. check_plain($line[0]) . '</td><td width="50%">'. check_plain($line[1]) .'</td></tr>';
        }
        $output .= '</table>';
      }
      $output .= '</blockquote>';
    }
    $output .= '</li>';
  };
  $output .= '</ol>';

  if ($without_translations > 0 && node_access('update', $node)) {
    $output .= '<p><strong>';
    $output .= t('There are @count senses without translations.', array('@count' => $without_translations));
    $output .= '  ';
	$output .= t('!edit and contribute!', array('!edit' => l(t('Edit this entry'), 'node/'. $node->nid .'/edit')));
    $output .= '</strong></p>';
  }

  drupal_add_css(drupal_get_path('module', 'lingwo_senses') .'/lingwo_senses.css');
  return $output;
}

function theme_lingwo_senses_relationship($node, $sense) {
  if (_lingwo_is_translation($node)) {
    $source_lang = LingwoEntry::fromNode($node)->getSourceLanguage();
    $target_lang = $node->language;
    $spec = "$source_lang-$target_lang";
  }
  else {
    $spec = $source_lang = $target_lang = $node->language;
  }

  $pos = $sense['is']['pos'];
  $headword = $sense['is']['headword'];

  // TODO: this should be a function!
  // TODO: this should operate somehow, when we don't have a lookup path!
  $path = implode('/', array(
    LingwoEntry::$settings->lookup_path,
    $spec,
    $headword .'('. $pos .')'
  ));

  $entry_text = $headword .' ('. LingwoEntry::getPosOptions($node->language, TRUE, $pos) .')';
  // TODO: shouldn't be a link if there is no lookup path and this node doesn't exist!
  $entry_text = l($entry_text, $path, array(
    // setup for working in the korpus reader
    'attributes' => array(
      // TODO: hashing should be a function call!
      'data-entry' => sha1(join(':', array($source_lang, $pos, $headword))),
      'class' => 'anno'
    )
  ));

  return t('@rel of !entry', array(
    '@rel' => LingwoSenses::getRelationshipOptions($source_lang, TRUE, $sense['is']['relationship']),
    '!entry' => $entry_text
  ));
}

