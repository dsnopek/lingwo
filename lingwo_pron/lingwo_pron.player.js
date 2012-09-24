
// Right now, we only do ogg/vorbis via HTML5
Drupal.behaviors.lingwo_pron_player = function (context) {
  $('a.lingwo-pron-audio', context).click(function (evt) {
    var a = document.createElement('audio');

    // prevent this from happening twice
    $(this).unbind('click');

    if (!!a.canPlayType && a.canPlayType('audio/ogg; codecs="vorbis"').replace(/no/, '')) {
      // build up audio element
      $(a)
          .attr('src', this.href)
          .attr('controls', 'controls')
          .css('display', 'block')
          .appendTo(this);
      
      // replace the current element
      $(this).replaceWith(a);

      // start the audio
      a.play();

      // prevent the link from opening
      evt.preventDefault();
      return false;
    }
  });
};

