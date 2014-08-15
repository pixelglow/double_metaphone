**double_metaphone** implements a fast version of Lawrence Philips' [Double Metaphone Search Algorithm](http://www.drdobbs.com/the-double-metaphone-search-algorithm/184401251?pgno=2), a type of "sound alike" algorithm. This algorithm converts a word into phonetic keys, which should be the same for words pronounced similarly. Double Metaphone is widely used in spell checkers, search engines and genealogy sites.

I've implemented this search in modern C++11, with a focus on usability, testability and speed. Running on a 2.26GHz Intel Core 2 Duo, the search reaches 2,000,000 words per second.

Use
---

Header includes:

    #include "double_metaphone.h"

Phonetic keys for words:

    auto keys = dm::double_metaphone("Angier");
    assert(keys.first == "ANJ"); // main key
    assert(keys.second == "ANJR"); // alternate key

Phonetic key parts and their corresponding ranges in the original word:

    int index = 0;
    dm::double_metaphone(
      "Angier",
      [&](const char* metaph1, const char* metaph2, int start, int len)
      {
        switch (index++)
        {
          case 0:
            assert(strcmp(metaph1, "A") == 0 && strcmp(metaph2, "A") == 0);
            assert(start == 0 && len == 1); // corresponds to "A" in "Angier"
            break;
          case 1:
            assert(strcmp(metaph1, "N") == 0 && strcmp(metaph2, "N") == 0);
            assert(start == 1 && len == 1); // corresponds to "n" in "Angier"
            break;
          case 2:
            assert(strcmp(metaph1, "J") == 0 && strcmp(metaph2, "J") == 0);
            assert(start == 2 && len == 1); // corresponds to "g" in "Angier"
            break;
          case 3:
            assert(metaph1 == nullptr && metaph2 == nullptr);
            assert(start == 3 && len == 1); // corresponds to "i" in "Angier"
            break;
          case 4:
            assert(metaph1 == nullptr && metaph2 == nullptr);
            assert(start == 4 && len == 1); // corresponds to "e" in "Angier"
            break;
          case 5:
            assert(metaph1 == nullptr && strcmp(metaph2, "R") == 0);
            assert(start == 5 && len == 1); // corresponds to "r" in "Angier"
            break;
        }
      });

Require
-------

Any C++11 toolchain, e.g. clang 3.3, gcc 4.8.

Install
-------

**double_metaphone** is header only, just copy `double_metaphone.h` into your own sources.

To keep up to date in your git-managed project:

    # once only
    git remote add double_metaphone git@github.com:pixelglow/double_metaphone.git
    git subtree add --prefix=double_metaphone double_metaphone master
    # whenever double_metaphone changes
    git subtree pull --prefix=double_metaphone double_metaphone master

Test
----

On clang:

    clang++ -std=c++11 -stdlib=libc++ test.cpp -otest && ./test

On gcc:

    g++ -std=c++11 test.cpp -otest && ./test

Design
------

The main differences over similar implementations:

* Standard C++11 with modern idioms like namespaces, lambdas, `std::string` and `std::pair`.
* Callback API for phonetic key parts and their corresponding ranges in the original word.
* Each key part is generally either all vowels or all consonants.
* Tuned for speed.

Copyright
---------

Copyright (c) 1998, 1999, Lawrence Philips.

Slightly modified by Kevin Atkinson to fix several bugs and to allow it to give back more than 4 characters.

Copyright (c) 2014, Pixelglow Software. All rights reserved.

This version is derived from [Kevin Atkinson's](http://aspell.net/metaphone/dmetaph.cpp) version, which is in turn derived from Lawrence Philips' original version. The tests are based on the data from [Stephen Woodbridge's](http://swoodbridge.com/DoubleMetaPhone/surnames.txt) version.

License
-------

*double_metaphone* is licensed with the BSD license.

Support
-------

* Follow us on Twitter: [@pixelglow](http://twitter.com/pixelglow).
* Raise an issue on [double_metaphone issues](https://github.com/pixelglow/double_metaphone/issues).

Donate
------

Bitcoin: 1HUFjghYhvrpCWKAYD4x1ihE3fM4VqNL5H
