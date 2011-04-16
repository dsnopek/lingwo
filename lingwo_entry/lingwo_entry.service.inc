<?php

/**
 * @file
 * Contains all of our service callbacks
 */

function lingwo_entry_search_service($text, $options=array()) {
  return LingwoEntry::search($text, $options);
}

