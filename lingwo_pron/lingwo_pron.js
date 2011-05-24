
Drupal.behaviors.lingwo_pron = function (context) {
  function triggerAhah(evt) {
    evt.preventDefault();
    var id = 'edit--lingwo-pron-refresh',
      settings = Drupal.settings.ahah[id],
      event_name = settings['event'],
      selector = settings['selector'];
    $(selector).trigger(event_name);
    return false;
  }

  // bind to language and remove checkboxes to trigger AHAH
  $('#edit-language, .lingwo-pron-remove', context).change(triggerAhah);
  // bind to the upload button as well
  $('.lingwo-pron-upload', context).click(triggerAhah);
};

// Replace Drupal.ahahError to give a sane message to users
Drupal.ahahError = function (xmlhttp, uri) {
  return xmlhttp.responseText;
}

