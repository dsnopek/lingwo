<?php
global $language;

if ($source_node = $entry->getTranslationSource()) {
  // display the pron as if this were the source node
  $entry = LingwoEntry::fromNode($source_node);
}

drupal_add_css(drupal_get_path('module', 'lingwo_pron') . '/lingwo_pron.css');
?>
<div class="lingwo-pron-list">
<?php foreach ($entry->pron as $pron): ?>
  <div class="lingwo-pron-item">
  <?php if (!empty($pron['tag'])): ?>
    <span class="lingwo-pron-tag">(<?php print LingwoPron::getTagOptions($language->language, TRUE, $pron['tag']); ?>)</span>
  <?php endif; ?>
  <?php if (!empty($pron['ipa'])): ?>
    <span class="lingwo-pron-ipa">/<?php print $pron['ipa']; ?>/</span>
  <?php endif; ?>
  <?php if (!empty($pron['audio'])): ?>
    <?php print l(t('Listen'), $pron['audio']->filepath, array('attributes' => array('class' => 'lingwo-pron-audio'))); ?>
  <?php endif; ?>
  </div>
<?php endforeach; ?>
</div>
