//
//  double_metaphone.h
//
//  Copyright (c) 2014, Pixelglow Software. All rights reserved.
//
//  Original is copyright (c) 1998, 1999, Lawrence Philips.
//  Slightly modified by Kevin Atkinson to fix several bugs and
//  to allow it to give back more than 4 characters.
//

//  Redistribution and use in source and binary forms, with or without modification,
//  are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
//  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
//  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
//  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//  THE POSSIBILITY OF SUCH DAMAGE.

#ifndef DM_DOUBLE_METAPHONE_H
#define DM_DOUBLE_METAPHONE_H

#include <string>
#include <utility>

namespace dm
{
  template <typename F> inline void double_metaphone(std::string str, F callback)
  {
    size_t current = 0;

    /* we need the real length and last prior to padding */
    const auto length = str.length();
    const auto last = length - 1;

    auto IsVowel = [&](size_t at)
    {
      if (at < length)
        switch (str[at])
      {
        case 'A':
        case 'E':
        case 'I':
        case 'O':
        case 'U':
        case 'Y':
          return true;
      }
      return false;
    };

    auto SlavoGermanic = [&]()
    {
      return str.find_first_of("WK") != std::string::npos ||
        str.find("CZ") != std::string::npos ||
        str.find("WITZ") != std::string::npos;
    };

    auto StringAt = [&](size_t start, size_t len, std::initializer_list<const char*> sstrings)
    {
      if (start < length)
      {
        auto strStart = str.begin() + start;
        auto strFinish = strStart + len;
        for (auto sstring : sstrings)
          if (std::equal(strStart, strFinish, sstring))
            return true;
      }
      return false;
    };

    auto CharAt = [&](size_t start, std::initializer_list<char> cchars)
    {
      if (start < length)
      {
        auto ch = str[start];
        for (auto cchar : cchars)
          if (ch == cchar)
            return true;
      }
      return false;
    };

    /* uppercase string */
    bool other_latin_prefix = false;
    for (auto& ch : str)
    {
      if ((ch >= 'a' && ch <= 'z') ||
          (other_latin_prefix && ch >= '\xa0' && ch <= '\xbe'))
        ch -= 0x20;
      other_latin_prefix = ch == '\xc3';
    }

    /* pad original so we can index beyond end */
    str.append(5, ' ');

    /* skip these when at start of word */
    if (StringAt(0, 2, {"GN", "KN", "PN", "WR", "PS"}))
    {
      callback(nullptr, nullptr, 0, 1);
      ++current;
    }

    /* Initial 'X' is pronounced 'Z' e.g. 'Xavier' */
    if (str[0] == 'X')
    {
      callback("S", "S", 0, 1); /* 'Z' maps to 'S' */
      ++current;
    }

    /* main loop */
    while (current < length)
    {
      const char* metaph1 = nullptr;
      const char* metaph2 = nullptr;
      int inc = 0;

      switch (str[current])
      {
        case 'A':
        case 'E':
        case 'I':
        case 'O':
        case 'U':
        case 'Y':
          if (current == 0)
          {
            /* all init vowels now map to 'A' */
            metaph1 = "A";
            metaph2 = "A";
          }
          inc = 1;
          break;

        case 'B':

          /* "-mb", e.g", "dumb", already skipped over... */
          metaph1 = "P";
          metaph2 = "P";

          if (str[current + 1] == 'B')
            inc = 2;
          else
            inc = 1;
          break;

          /*
           case 'Ç':
           metaph1 = "S";
           metaph2 = "S";
           inc = 1;
           break;
           */

        case 'C':
          /* various germanic */
          if (current > 1
              && !IsVowel(current - 2)
              && StringAt(current - 1, 3, {"ACH"})
              && str[current + 2] != 'I'
              && (str[current + 2] != 'E' || StringAt(current - 2, 6, {"BACHER", "MACHER"})))
          {
            metaph1 = "K";
            metaph2 = "K";
            inc = 2;
            break;
          }

          /* special case 'caesar' */
          if (current == 0
              && StringAt(current, 6, {"CAESAR"}))
          {
            metaph1 = "S";
            metaph2 = "S";
            inc = 1;
            break;
          }

          /* italian 'chianti' */
          if (StringAt(current, 4, {"CHIA"}))
          {
            metaph1 = "K";
            metaph2 = "K";
            inc = 2;
            break;
          }

          if (StringAt(current, 2, {"CH"}))
          {
            /* find 'michael' */
            if (current > 0
                && StringAt(current, 4, {"CHAE"}))
            {
              metaph1 = "K";
              metaph2 = "X";
              inc = 2;
              break;
            }

            /* greek roots e.g. 'chemistry', 'chorus' */
            if (current == 0
                && (StringAt(current + 1, 5, {"HARAC", "HARIS"}) || StringAt(current + 1, 3, {"HOR", "HYM", "HIA", "HEM"}))
                && !StringAt(0, 5, {"CHORE"}))
            {
              metaph1 = "K";
              metaph2 = "K";
              inc = 2;
              break;
            }

            /* germanic, greek, or otherwise 'ch' for 'kh' sound */
            if ((StringAt(0, 4, {"VAN ", "VON "}) || StringAt(0, 3, {"SCH"}))
                /*  'architect but not 'arch', 'orchestra', 'orchid' */
                || StringAt(current - 2, 6, {"ORCHES", "ARCHIT", "ORCHID"})
                || CharAt(current + 2, {'T', 'S'})
                || ((CharAt(current - 1, {'A', 'O', 'U', 'E'}) || current == 0)
                    /* e.g., 'wachtler', 'wechsler', but not 'tichner' */
                    && CharAt(current + 2, {'L', 'R', 'N', 'M', 'B', 'H', 'F', 'V', 'W', ' '})))
            {
              metaph1 = "K";
              metaph2 = "K";
            }
            else
            {
              if (current > 0)
              {
                if (StringAt(0, 2, {"MC"}))
                {
                  /* e.g., "McHugh" */
                  metaph1 = "K";
                  metaph2 = "K";
                }
                else
                {
                  metaph1 = "X";
                  metaph2 = "K";
                }
              }
              else
              {
                metaph1 = "X";
                metaph2 = "X";
              }
            }
            inc = 2;
            break;
          }
          /* e.g, 'czerny' */
          if (StringAt(current, 2, {"CZ"})
              && !StringAt(current - 2, 4, {"WICZ"}))
          {
            metaph1 = "S";
            metaph2 = "X";
            inc = 2;
            break;
          }

          /* e.g., 'focaccia' */
          if (StringAt(current + 1, 3, {"CIA"}))
          {
            metaph1 = "X";
            metaph2 = "X";
            inc = 2;
            break;
          }

          /* double 'C', but not if e.g. 'McClellan' */
          if (StringAt(current, 2, {"CC"})
              && !(current == 1 && str[0] == 'M'))
          {
            /* 'bellocchio' but not 'bacchus' */
            if (CharAt(current + 2, {'I', 'E', 'H'})
                && !StringAt(current + 2, 2, {"HU"}))
            {
              /* 'accident', 'accede' 'succeed' */
              if ((current == 1 && str[current - 1] == 'A')
                  || StringAt(current - 1, 5, {"UCCEE", "UCCES"}))
              {
                metaph1 = "KS";
                metaph2 = "KS";
                /* 'bacci', 'bertucci', other italian */
              }
              else
              {
                metaph1 = "X";
                metaph2 = "X";
              }
              inc = IsVowel(current + 2) ? 2 : 3;
              break;
            }
            else
            {	  /* Pierce's rule */
              metaph1 = "K";
              metaph2 = "K";
              inc = 2;
              break;
            }
          }

          if (StringAt(current, 2, {"CK", "CG", "CQ"}))
          {
            metaph1 = "K";
            metaph2 = "K";
            inc = 2;
            break;
          }

          if (StringAt(current, 2, {"CI", "CE", "CY"}))
          {
            /* italian vs. english */
            if (StringAt(current, 3, {"CIO", "CIE", "CIA"}))
            {
              metaph1 = "S";
              metaph2 = "X";
            }
            else
            {
              metaph1 = "S";
              metaph2 = "S";
            }
            inc = 1;
            break;
          }

          /* else */
          metaph1 = "K";
          metaph2 = "K";

          /* name sent in 'mac caffrey', 'mac gregor */
          if (StringAt(current + 1, 2, {" C", " Q", " G"}))
            inc = 3;
          else
            if (CharAt(current + 1, {'C', 'K', 'Q'})
                && !StringAt(current + 1, 2, {"CE", "CI"}))
              inc = 2;
            else
              inc = 1;
          break;

        case 'D':
          if (StringAt(current, 2, {"DG"}))
          {
            if (CharAt(current + 2, {'I', 'E', 'Y'}))
            {
              /* e.g. 'edge' */
              metaph1 = "J";
              metaph2 = "J";
            }
            else
            {
              /* e.g. 'edgar' */
              metaph1 = "TK";
              metaph2 = "TK";
            }
            inc = 2;
            break;
          }

          if (StringAt(current, 2, {"DT", "DD"}))
          {
            metaph1 = "T";
            metaph2 = "T";
            inc = 2;
            break;
          }

          /* else */
          metaph1 = "T";
          metaph2 = "T";
          inc = 1;
          break;

        case 'F':
          if (str[current + 1] == 'F')
            inc = 2;
          else
            inc = 1;
          metaph1 = "F";
          metaph2 = "F";
          break;

        case 'G':
          if (str[current + 1] == 'H')
          {
            if (current > 0 && !IsVowel(current - 1))
            {
              metaph1 = "K";
              metaph2 = "K";
              inc = 2;
              break;
            }

            if (current < 3)
            {
              /* 'ghislane', ghiradelli */
              if (current == 0)
              {
                if (str[current + 2] == 'I')
                {
                  metaph1 = "J";
                  metaph2 = "J";
                }
                else
                {
                  metaph1 = "K";
                  metaph2 = "K";
                }
                inc = 2;
                break;
              }
            }
            /* Parker's rule (with some further refinements) - e.g., 'hugh' */
            if ((current > 1 && CharAt(current - 2, {'B', 'H', 'D'}))
                /* e.g., 'bough' */
                || (current > 2 && CharAt(current - 3, {'B', 'H', 'D'}))
                /* e.g., 'broughton' */
                || (current > 3 && CharAt(current - 4, {'B', 'H'})))
            {
              inc = 2;
              break;
            }
            else
            {
              /* e.g., 'laugh', 'McLaughlin', 'cough', 'gough', 'rough', 'tough' */
              if (current > 2
                  && str[current - 1] == 'U'
                  && CharAt(current - 3, {'C', 'G', 'L', 'R', 'T'}))
              {
                metaph1 = "F";
                metaph2 = "F";
              }
              else if (current > 0 && str[current - 1] != 'I')
              {
                metaph1 = "K";
                metaph2 = "K";
              }

              inc = 2;
              break;
            }
          }

          if (str[current + 1] == 'N')
          {
            if (current == 1
                && IsVowel(0)
                && !SlavoGermanic())
            {
              metaph1 = "KN";
              metaph2 = "N";
            }
            else
            /* not e.g. 'cagney' */
              if (!StringAt(current + 2, 2, {"EY"})
                  && str[current + 1] != 'Y'
                  && !SlavoGermanic())
              {
                metaph1 = "N";
                metaph2 = "KN";
              }
              else
              {
                metaph1 = "KN";
                metaph2 = "KN";
              }
            inc = 2;
            break;
          }

          /* 'tagliaro' */
          if (StringAt(current + 1, 2, {"LI"})
              && !SlavoGermanic())
          {
            metaph1 = "KL";
            metaph2 = "L";
            inc = 2;
            break;
          }

          /* -ges-,-gep-,-gel-, -gie- at beginning */
          if (current == 0
              && (str[current + 1] == 'Y' || StringAt(current + 1, 2, {"ES", "EP", "EB", "EL", "EY", "IB", "IL", "IN", "IE", "EI", "ER"})))
          {
            metaph1 = "K";
            metaph2 = "J";
            inc = 1;
            break;
          }

          /*  -ger-,  -gy- */
          if ((StringAt(current + 1, 2, {"ER"}) || str[current + 1] == 'Y')
              && !StringAt(0, 6, {"DANGER", "RANGER", "MANGER"})
              && !CharAt(current - 1, {'E', 'I'})
              && !StringAt(current - 1, 3, {"RGY", "OGY"}))
          {
            metaph1 = "K";
            metaph2 = "J";
            inc = 1;
            break;
          }

          /*  italian e.g, 'biaggi' */
          if (CharAt(current + 1, {'E', 'I', 'Y'})
              || StringAt(current - 1, 4, {"AGGI", "OGGI"}))
          {
            /* obvious germanic */
            if (StringAt(0, 4, {"VAN ", "VON "})
                || StringAt(0, 3, {"SCH"})
                || StringAt(current + 1, 2, {"ET"}))
            {
              metaph1 = "K";
              metaph2 = "K";
            }
            else
            {
              /* always soft if french ending */
              if (StringAt(current + 1, 4, {"IER "}))
              {
                metaph1 = "J";
                metaph2 = "J";
              }
              else
              {
                metaph1 = "J";
                metaph2 = "K";
              }
            }
            inc = IsVowel(current + 1) ? 1 : 2;
            break;
          }

          if (str[current + 1] == 'G')
            inc = 2;
          else
            inc = 1;
          metaph1 = "K";
          metaph2 = "K";
          break;

        case 'H':
          /* only keep if first & before vowel or btw. 2 vowels */
          if ((current == 0 || IsVowel(current - 1))
              && IsVowel(current + 1))
          {
            metaph1 = "H";
            metaph2 = "H";
            inc = 1;
          }
          else		/* also takes care of 'HH' */
            inc = 1;
          break;

        case 'J':
          /* obvious spanish, 'jose', 'san jacinto' */
          if (StringAt(current, 4, {"JOSE"})
              || StringAt(0, 4, {"SAN "}))
          {
            if ((current == 0 && str[current + 4] == ' ')
                || StringAt(0, 4, {"SAN "}))
            {
              metaph1 = "H";
              metaph2 = "H";
            }
            else
            {
              metaph1 = "J";
              metaph2 = "H";
            }
            inc = 1;
            break;
          }

          if (current == 0
              && !StringAt(current, 4, {"JOSE"}))
          {
            metaph1 = "J";	/* Yankelovich/Jankelowicz */
            metaph2 = "A";
          }
          else
          {
            /* spanish pron. of e.g. 'bajador' */
            if (IsVowel(current - 1)
                && !SlavoGermanic()
                && (CharAt(current + 1, {'A', 'O'})))
            {
              metaph1 = "J";
              metaph2 = "H";
            }
            else
            {
              if (current == last)
                metaph1 = "J";
              else
              {
                if (!CharAt(current + 1, {'L', 'T', 'K', 'S', 'N', 'M', 'B', 'Z'})
                    && !CharAt(current - 1, {'S', 'K', 'L'}))
                {
                  metaph1 = "J";
                  metaph2 = "J";
                }
              }
            }
          }

          if (str[current + 1] == 'J')	/* it could happen! */
            inc = 2;
          else
            inc = 1;
          break;

        case 'K':
          if (str[current + 1] == 'K')
            inc = 2;
          else
            inc = 1;
          metaph1 = "K";
          metaph2 = "K";
          break;

        case 'L':
          if (str[current + 1] == 'L')
          {
            /* spanish e.g. 'cabrillo', 'gallegos' */
            if ((current == length - 3 && StringAt(current - 1, 4, {"ILLO", "ILLA", "ALLE"}))
                || ((StringAt(last - 1, 2, {"AS", "OS"}) || CharAt(last, {'A', 'O'}))
                    && StringAt(current - 1, 4, {"ALLE"})))
            {
              metaph1 = "L";
              inc = 2;
              break;
            }
            inc = 2;
          }
          else
            inc = 1;
          metaph1 = "L";
          metaph2 = "L";
          break;

        case 'M':
          if ((StringAt(current - 1, 3, {"UMB"}) && (current + 1 == last || StringAt(current + 2, 2, {"ER"})))
              /* 'dumb','thumb' */
              || str[current + 1] == 'M')
            inc = 2;
          else
            inc = 1;
          metaph1 = "M";
          metaph2 = "M";
          break;

        case 'N':
          if (str[current + 1] == 'N')
            inc = 2;
          else
            inc = 1;
          metaph1 = "N";
          metaph2 = "N";
          break;

          /*
           case 'Ñ':
           inc = 1;
           metaph1 = "N";
           metaph2 = "N";
           break;
           */

        case 'P':
          if (str[current + 1] == 'H')
          {
            metaph1 = "F";
            metaph2 = "F";
            inc = 2;
            break;
          }

          /* also account for "campbell", "raspberry" */
          if (CharAt(current + 1, {'P', 'B'}))
            inc = 2;
          else
            inc = 1;
          metaph1 = "P";
          metaph2 = "P";
          break;

        case 'Q':
          if (str[current + 1] == 'Q')
            inc = 2;
          else
            inc = 1;
          metaph1 = "K";
          metaph2 = "K";
          break;

        case 'R':
          /* french e.g. 'rogier', but exclude 'hochmeier' */
          if (current == last
              && !SlavoGermanic()
              && StringAt(current - 2, 2, {"IE"})
              && !StringAt(current - 4, 2, {"ME", "MA"}))
            metaph2 = "R";
          else
          {
            metaph1 = "R";
            metaph2 = "R";
          }

          if (str[current + 1] == 'R')
            inc = 2;
          else
            inc = 1;
          break;

        case 'S':
          /* special cases 'island', 'isle', 'carlisle', 'carlysle' */
          if (StringAt(current - 1, 3, {"ISL", "YSL"}))
          {
            inc = 1;
            break;
          }

          /* special case 'sugar-' */
          if (current == 0
              && StringAt(current, 5, {"SUGAR"}))
          {
            metaph1 = "X";
            metaph2 = "S";
            inc = 1;
            break;
          }

          if (StringAt(current, 2, {"SH"}))
          {
            /* germanic */
            if (StringAt(current + 1, 4, {"HEIM", "HOEK", "HOLM", "HOLZ"}))
            {
              metaph1 = "S";
              metaph2 = "S";
            }
            else
            {
              metaph1 = "X";
              metaph2 = "X";
            }
            inc = 2;
            break;
          }

          /* italian & armenian */
          if (StringAt(current, 3, {"SIO", "SIA"})
              || StringAt(current, 4, {"SIAN"}))
          {
            if (!SlavoGermanic())
            {
              metaph1 = "S";
              metaph2 = "X";
            }
            else
            {
              metaph1 = "S";
              metaph2 = "S";
            }
            inc = 1;
            break;
          }

          /* german & anglicisations, e.g. 'smith' match 'schmidt', 'snider' match 'schneider'
           also, -sz- in slavic language altho in hungarian it is pronounced 's' */
          if ((current == 0
               && CharAt(current + 1, {'M', 'N', 'L', 'W'}))
              || str[current + 1] == 'Z')
          {
            metaph1 = "S";
            metaph2 = "X";
            if (str[current + 1] == 'Z')
              inc = 2;
            else
              inc = 1;
            break;
          }

          if (StringAt(current, 2, {"SC"}))
          {
            /* Schlesinger's rule */
            if (str[current + 2] == 'H')
            {
              /* dutch origin, e.g. 'school', 'schooner' */
              if (StringAt(current + 3, 2, {"OO", "ER", "EN", "UY", "ED", "EM"}))
              {
                /* 'schermerhorn', 'schenker' */
                if (StringAt(current + 3, 2, {"ER", "EN"}))
                {
                  metaph1 = "X";
                  metaph2 = "SK";
                }
                else
                {
                  metaph1 = "SK";
                  metaph2 = "SK";
                }
                inc = 3;
                break;
              }
              else
              {
                if (current == 0 && !IsVowel(3)
                    && (str[3] != 'W'))
                {
                  metaph1 = "X";
                  metaph2 = "S";
                }
                else
                {
                  metaph1 = "X";
                  metaph2 = "X";
                }
                inc = 3;
                break;
              }
            }

            if (CharAt(current + 2, {'I', 'E', 'Y'}))
            {
              metaph1 = "S";
              metaph2 = "S";
              inc = 2;
              break;
            }
            /* else */
            metaph1 = "SK";
            metaph2 = "SK";
            inc = IsVowel(current + 2) || current + 2 >= length ? 2 : 3;
            break;
          }

          /* french e.g. 'resnais', 'artois' */
          if (current == last
              && StringAt(current - 2, 2, {"AI", "OI"}))
            metaph2 = "S";
          else
          {
            metaph1 = "S";
            metaph2 = "S";
          }

          if (CharAt(current + 1, {'S', 'Z'}))
            inc = 2;
          else
            inc = 1;
          break;

        case 'T':
          if (StringAt(current, 4, {"TION"}))
          {
            metaph1 = "X";
            metaph2 = "X";
            inc = 1;
            break;
          }

          if (StringAt(current, 3, {"TIA"}))
          {
            metaph1 = "X";
            metaph2 = "X";
            inc = 1;
            break;
          }

          if (StringAt(current, 3, {"TCH"}))
          {
            metaph1 = "X";
            metaph2 = "X";
            inc = 3;
            break;
          }

          if (StringAt(current, 2, {"TH"})
              || StringAt(current, 3, {"TTH"}))
          {
            /* special case 'thomas', 'thames' or germanic */
            if (StringAt(current + 2, 2, {"OM", "AM"})
                || StringAt(0, 4, {"VAN ", "VON "})
                || StringAt(0, 3, {"SCH"}))
            {
              metaph1 = "T";
              metaph2 = "T";
            }
            else
            {
              metaph1 = "0";
              metaph2 = "T";
            }
            inc = 2;
            break;
          }

          if (CharAt(current + 1, {'T', 'D'}))
            inc = 2;
          else
            inc = 1;
          metaph1 = "T";
          metaph2 = "T";
          break;

        case 'V':
          if (str[current + 1] == 'V')
            inc = 2;
          else
            inc = 1;
          metaph1 = "F";
          metaph2 = "F";
          break;

        case 'W':
          /* can also be in middle of word */
          if (StringAt(current, 2, {"WR"}))
          {
            metaph1 = "R";
            metaph2 = "R";
            inc = 2;
            break;
          }

          if (current == 0
              && (IsVowel(current + 1) || StringAt(current, 2, {"WH"})))
          {
            /* Wasserman should match Vasserman */
            if (IsVowel(current + 1))
            {
              metaph1 = "A";
              metaph2 = "F";
            }
            else
            {
              /* need Uomo to match Womo */
              metaph1 = "A";
              metaph2 = "A";
            }
            inc = 1;
            break;
          }

          /* Arnow should match Arnoff */
          if ((current == last && IsVowel(current - 1))
              || StringAt(current - 1, 5, {"EWSKI", "EWSKY", "OWSKI", "OWSKY"})
              || StringAt(0, 3, {"SCH"}))
          {
            metaph2 = "F";
            inc = 1;
            break;
          }

          /* polish e.g. 'filipowicz' */
          if (StringAt(current, 4, {"WICZ", "WITZ"}))
          {
            metaph1 = "TS";
            metaph2 = "FX";
            inc = 4;
            break;
          }

          /* else skip it */
          inc = 1;
          break;

        case 'X':
          /* french e.g. breaux */
          if (!(current == last
                && (StringAt(current - 3, 3, {"IAU", "EAU"}) || StringAt(current - 2, 2, {"AU", "OU"}))))
          {
            metaph1 = "KS";
            metaph2 = "KS";
          }
          
          if (CharAt(current + 1, {'C', 'X'}))
            inc = 2;
          else
            inc = 1;
          break;
          
        case 'Z':
          /* chinese pinyin e.g. 'zhao' */
          if (str[current + 1] == 'H')
          {
            metaph1 = "J";
            metaph2 = "J";
            inc = 2;
            break;
          }
          else if (StringAt(current + 1, 2, {"ZO", "ZI", "ZA"})
                   || (SlavoGermanic() && current > 0 && str[current - 1] != 'T'))
          {
            metaph1 = "S";
            metaph2 = "TS";
          }
          else
          {
            metaph1 = "S";
            metaph2 = "S";
          }
          
          if (str[current + 1] == 'Z')
            inc = 2;
          else
            inc = 1;
          break;

        case '\xc3':
          /* UTF-8 Latin */
          switch (str[current + 1])
          {
            case '\x87':
              /* Ç */
              metaph1 = "S";
              metaph2 = "S";
              inc = 2;
              break;
            case '\x91':
              /* 'Ñ' */
              metaph1 = "N";
              metaph2 = "N";
              inc = 2;
              break;
            default:
              inc = 1;
              break;
          }
          break;

        default:
          inc = 1;
      }
      callback(metaph1, metaph2, current, inc);
      current += inc;
    }
  }

  inline std::pair<std::string, std::string> double_metaphone(std::string str)
  {
    std::pair<std::string, std::string> result;

    // compose results from callback
    double_metaphone(std::move(str),
                     [&result](const char* metaph1, const char* metaph2, size_t start, size_t len)
                     {
                       if (metaph1)
                         result.first += metaph1;
                       if (metaph2)
                         result.second += metaph2;
                     });
    return result;
  }
}

#endif
