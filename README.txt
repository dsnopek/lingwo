Lingwo - Online Dictionary modules
----------------------------------

Lingwo is a small family of modules for creating on online dictionary, which adheres as much as 
possible to professional lexicographical practices.  It focuses on translating dictionaries
(ex. Spanish-English dictionary) which are maintained by a community like a wiki.

This is the Open Source version of the code behind BiblioBird.com.  I am currently working on moving
the rest of the code from BiblioBird, such that eventually all of it will be Open Source.

Installation
------------

 * First, enable the "lingwo_entry" module.

 * Next, if you want to implement a translating dictionary:

   o Enable the "locale" and "translation" modules.

   o Go to "Admin >> Site configuration >> Languages" and add any languages you need.

   o Next, we need to make the "Dictionary entry" content type translatable:

     - Go to "Admin >> Content management >> Content types"

	 - Click "edit" by the "Dictionary entry" type

	 - Expand "Workflow settings" and change "Multilingual support" to "Enabled, with translation"

	 - Click "Save content type"

   o If you are using the views module, you may want to consider disabling the "dictionary" view
     and enabling the "dictionary_multiple" view.

 * If you want to using Lingwo to manage multiple senses on each entry, enable the "lingwo_senses"
   module.

Now, you can create content of the "Entry" type and translate it using Drupal's built-in translation
mechanisms.

Configuration
-------------

You can find configuration for all the Lingwo modules at:

  Admin >> Site configuration >> Lingwo dictionary settings

I recommend taking a look at what is configurable right away.

Modules
-------

lingwo_entry:

 * Core module on which all the other modules build

 * Adds a "Part of Speech" field to all entries and makes sure that all entries are unique according
   to language, part of speech and headword.
 
 * Makes dictionary entries accesible through a lookup path.  For example:

     http://example.com/lookup/es-en/coche (noun)

   (Links to the Spanish-English dictionary translation of the noun "coche".  This example uses
    the default lookup path "lookup", which can be altered in admin)
 
 * Provides a search block for dictionary entries

lingwo_senses:

 * Adds a list of multiple "senses" to each entry

 * On source entries (ex. the Spanish entry in a Spanish-English dictionary) each sense gets:

   o "Difference" - Describes how this sense is different than the others (can be configured to
      be a "Definition" instead)

   o "Example" - A single or multi-line example (configured in admin) of the word in use
 
   o "Relationship" - The relationship between this sense and another entry.
 
 * On translation entries (ex. the English translation of a Spanish entry) each sense gets:

   o "Translation of Example" - Allows you to translate the example into the translation language.

   o "Translations" - A list of translations for the entry

   o "Same As" - Lets you mark this sense as having the same translation as another sense

   o "No equivalent" - Let you mark this senses as having no equivalent in this language (normally
     only necessary for grammar words in unrelated languages)

There are several module not yet ported from BiblioBird.com to the Open Source versions here.  This 
is a brief list of them:

 * lingwo_fields: Allows entries to have configured or arbitrary "fields" based on language and 
   part of speech.  Its main purpose is to store additional forms, for example: plural of nouns
   or conjugations of verbs.

 * lingwo_corpus: For maintaining a linguistic corpus.  Allows users to tag words in a corpus
   text as being entries in the dictionary.

 * lingwo_language: Allows languages to have seperate definitions of different core concepts
   inside of Lingwo.
 
 * lingwo_import: Imports data from various sources including the Wiktionary and Open Dictionaries.

Copyright
---------

Copyright (C) 2010, 2011 David Snopek

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

