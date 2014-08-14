**double_metaphone** implements a fast version of Lawrence Philips' [Double Metaphone Search Algorithm](http://www.drdobbs.com/the-double-metaphone-search-algorithm/184401251?pgno=2), a type of "sound alike" algorithm. This algorithm converts a word into phonetic keys, which should be the same for any words pronounced similarly. I've implemented this search in modern C++11, with a focus on usability, testability and speed.

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

Any C++11 toolchain.

Install
-------

**double_metaphone** is header only, just copy `double_metaphone.h` into your own sources.

Test
----

On Mac OS X:

    clang++ -std=c++11 -stdlib=libc++ test.cpp -otest && ./test

Design
------

The main differences and improvements over the original and similiar implementations:

* Standard C++11 with modern idioms like lambdas, `std::string` and `std::pair`.
* Callback API for phonetic key parts and their corresponding ranges in the original word.
* Each key part is generally either all vowels or all consonants.
* Tuned for speed. Running on 2.26GHz Intel Core 2 Duo, it reaches 2,000,000 words per second.

Copyright
---------

Copyright (c) 2014, Pixelglow Software. All rights reserved.

*double_metaphone* is derived from:
* [Kevin Atkinson's](http://aspell.net/metaphone/dmetaph.cpp) version which fixes several bugs and gives back more than 4 char keys.
  Like the original it is written in Visual C++ and not in Standard C++.
* Tests are based on [Stephen Woodbridge's](http://swoodbridge.com/DoubleMetaPhone/surnames.txt) version.

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
