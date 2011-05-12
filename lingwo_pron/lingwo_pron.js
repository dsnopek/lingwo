
Drupal.behaviors.lingwo_pron = function (context) {
  // bind to language to trigger AHAH
  $('#edit-language', context).change(function () {
    var id = 'edit--lingwo-pron-refresh',
      settings = Drupal.settings.ahah[id],
      event_name = settings['event'],
      selector = settings['selector'];
    $(selector).trigger(event_name);
  });
};

