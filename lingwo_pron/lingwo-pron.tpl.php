<?php

if ($source_node = $entry->getTranslationSource()) {
  // display the pron as if this were the source node
  $entry = LingwoEntry::fromNode($source_node);
}
?>
<div class="lingwo-pron-list">
<?php foreach ($entry->pron as $pron): ?>
  <div class="lingwo-pron-item">
  <?php if (!empty($pron['tag'])): ?>
    <span class="lingwo-pron-tag">(<?php print $pron['tag']; ?>)</span>
  <?php endif; ?>
  <?php if (!empty($pron['ipa'])): ?>
    <span class="lingwo-pron-ipa">/<?php print $pron['ipa']; ?>/</span>
  <?php endif; ?>
  <?php if (!empty($pron['audio'])): ?>
    <a href="<?php print $pron['audioUrl']; ?>" class="lingwo-pron-audio">Listen</a>
  <?php endif; ?>
  </div>
<?php endforeach; ?>
</div>
