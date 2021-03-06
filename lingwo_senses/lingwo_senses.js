
Drupal.behaviors.lingwo_senses = function (context) {
  function triggerAhah() {
    var id = 'edit--lingwo-senses-refresh',
      settings = Drupal.settings.ahah[id],
      event_name = settings['event'],
      selector = settings['selector'];
    $(selector).trigger(event_name);
  };

  // bind to language to trigger AHAH
  $('#edit-language', context).change(triggerAhah);

  // bind remove checkbox to trigger AHAH
  $('.lingwo-senses-remove', context).change(function () {
    var id = (''+this.id).replace(/-remove$/, '-new');
    if ($('#'+id).val() == '1') {
      triggerAhah();
    }
  });

  /*
   * Setup our UI toggles (Same As, No equivalent, Relationship)
   */

  function makeToggleFunc(checkFunc, togglePart) {
    return function (node, anim) {
      var show = checkFunc(node),
        match, toggleSelector;

      // get the full id
      if (match = /^edit-lingwo-senses-(\d+)/.exec(node.id+'')) {
        toggleSelector = '#edit-lingwo-senses-'+match[1]+'-data-'+togglePart+'-wrapper';
        $(toggleSelector)[show ? 'show' : 'hide'](anim ? 'fast' : null);
      }
    };
  }

  function checkSelectHasValue(node) {
    return $(':selected', node).val() != '';
  }

  function checkIsChecked(node) {
    return $(node).is(':checked');
  }

  function not(func) {
    return function () {
      return !func.apply(null, arguments);
    };
  }

  var toggles = {
    '.same-as-select': makeToggleFunc(not(checkSelectHasValue), 'translation'),
    '.no-equivalent-checkbox': makeToggleFunc(not(checkIsChecked), 'translation-trans'),
    '.is-relationship': makeToggleFunc(not(checkSelectHasValue), 'difference')
  }, selector;

  for(selector in toggles) {
    (function (selector, toggle) {
      $(selector).change(function (evt) {
        console.log('change');
        toggle(evt.target, true);
      });
      $(selector, context).each(function () { toggle(this); });
    })(selector, toggles[selector]);
  }
};

