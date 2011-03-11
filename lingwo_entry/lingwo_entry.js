
Drupal.behaviors.lingwo_entry = function (context) {
    $('#edit-language', context).change(function () {
        var id = 'edit--lingwo-entry-reload-pos',
            settings = Drupal.settings.ahah[id],
            event_name = settings['event'],
            selector = settings['selector'];
        $(selector).trigger(event_name);
    });
};

