
Drupal.behaviors.lingwo_pron = function (context) {
  // bind to language and remove checkboxes to trigger AHAH
  $('#edit-language, .lingwo-pron-remove', context).change(function () {
    var id = 'edit--lingwo-pron-refresh',
      settings = Drupal.settings.ahah[id],
      event_name = settings['event'],
      selector = settings['selector'];
    $(selector).trigger(event_name);
  });
};

