/* SWIutfconversions, Unicode conversions */

/****************License************************************************
 * Vocalocity OpenVXI
 * Copyright (C) 2004-2005 by Vocalocity, Inc. All Rights Reserved.
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *  
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * Vocalocity, the Vocalocity logo, and VocalOS are trademarks or 
 * registered trademarks of Vocalocity, Inc. 
 * OpenVXI is a trademark of Scansoft, Inc. and used under license 
 * by Vocalocity.
 ***********************************************************************/

/* -----1=0-------2=0-------3=0-------4=0-------5=0-------6=0-------7=0-------8
 */

#include <stdio.h>
#include <string.h>
#include <SWIutfconversions.h>

#if 0
#include "Encoding.h"
#include <cstring>
#include <cwchar>
#include <vector>
#include <algorithm>

bool initialized = false;


// ---------------------------------------------------------------------------
// Define a registry of decoder functions.
// ---------------------------------------------------------------------------

typedef bool (*DECODERFUNCTION)(const char *,
                                std::basic_string<wchar_t> &);

class EncoderEntry {
public:
  const char * name;
  DECODERFUNCTION function;

  EncoderEntry(const char * x, DECODERFUNCTION y)
    : name(x), function(y) { }

  EncoderEntry(const EncoderEntry & x) : name(x.name), function(x.function) { }

  EncoderEntry & operator=(const EncoderEntry & x)
  { if (this != &x) { name = x.name;  function = x.function; }
    return *this; }
};

bool operator<(const EncoderEntry & x, const EncoderEntry & y)
{ return strcmp(x.name, y.name) < 0; }

// ---------------------------------------------------------------------------

typedef std::vector<EncoderEntry> DECODER_REGISTRY;
DECODER_REGISTRY decoderRegistry;

void InitializeDecoder()
{
  if(initialized)
    return;
  decoderRegistry.push_back(EncoderEntry("utf-8",       DecodeUTF8));
  decoderRegistry.push_back(EncoderEntry("utf8",        DecodeUTF8));

  decoderRegistry.push_back(EncoderEntry("us-ascii",    DecodeASCII));
  decoderRegistry.push_back(EncoderEntry("us_ascii",    DecodeASCII));
  decoderRegistry.push_back(EncoderEntry("usascii",     DecodeASCII));
  decoderRegistry.push_back(EncoderEntry("ascii",       DecodeASCII));

  decoderRegistry.push_back(EncoderEntry("iso8859-1",   DecodeISO8859_1));
  decoderRegistry.push_back(EncoderEntry("iso-8859-1",  DecodeISO8859_1));
  decoderRegistry.push_back(EncoderEntry("iso_8859-1",  DecodeISO8859_1));
  decoderRegistry.push_back(EncoderEntry("latin1",      DecodeISO8859_1));
  decoderRegistry.push_back(EncoderEntry("latin-1",     DecodeISO8859_1));
  decoderRegistry.push_back(EncoderEntry("latin_1",     DecodeISO8859_1));
  decoderRegistry.push_back(EncoderEntry("ibm-819",     DecodeISO8859_1));
  decoderRegistry.push_back(EncoderEntry("ibm819",      DecodeISO8859_1));

  decoderRegistry.push_back(EncoderEntry("iso8859-2",   DecodeISO8859_2));
  decoderRegistry.push_back(EncoderEntry("iso-8859-2",  DecodeISO8859_2));
  decoderRegistry.push_back(EncoderEntry("iso_8859-2",  DecodeISO8859_2));
  decoderRegistry.push_back(EncoderEntry("latin2",      DecodeISO8859_2));
  decoderRegistry.push_back(EncoderEntry("latin-2",     DecodeISO8859_2));
  decoderRegistry.push_back(EncoderEntry("latin_2",     DecodeISO8859_2));

  decoderRegistry.push_back(EncoderEntry("iso8859-3",   DecodeISO8859_3));
  decoderRegistry.push_back(EncoderEntry("iso-8859-3",  DecodeISO8859_3));
  decoderRegistry.push_back(EncoderEntry("iso_8859-3",  DecodeISO8859_3));
  decoderRegistry.push_back(EncoderEntry("latin3",      DecodeISO8859_3));
  decoderRegistry.push_back(EncoderEntry("latin-3",     DecodeISO8859_3));
  decoderRegistry.push_back(EncoderEntry("latin_3",     DecodeISO8859_3));

  decoderRegistry.push_back(EncoderEntry("iso8859-4",   DecodeISO8859_4));
  decoderRegistry.push_back(EncoderEntry("iso-8859-4",  DecodeISO8859_4));
  decoderRegistry.push_back(EncoderEntry("iso_8859-4",  DecodeISO8859_4));
  decoderRegistry.push_back(EncoderEntry("latin4",      DecodeISO8859_4));
  decoderRegistry.push_back(EncoderEntry("latin-4",     DecodeISO8859_4));
  decoderRegistry.push_back(EncoderEntry("latin_4",     DecodeISO8859_4));

  decoderRegistry.push_back(EncoderEntry("iso8859-15",  DecodeISO8859_15));
  decoderRegistry.push_back(EncoderEntry("iso-8859-15", DecodeISO8859_15));
  decoderRegistry.push_back(EncoderEntry("iso_8859-15", DecodeISO8859_15));
  decoderRegistry.push_back(EncoderEntry("latin9",      DecodeISO8859_15));
  decoderRegistry.push_back(EncoderEntry("latin-9",     DecodeISO8859_15));
  decoderRegistry.push_back(EncoderEntry("latin_9",     DecodeISO8859_15));

  std::sort(decoderRegistry.begin(), decoderRegistry.end());

  initialized = true;
}

static bool DoInitialization()
{
  if(initialized)
    return true;
  InitializeDecoder();
  return true;
}
bool do_initialization = DoInitialization();


int DecodeString(const char * encodingName,
                 const char * inputString,
                 std::basic_string<wchar_t> & outputString)
{
  if (!initialized || encodingName == NULL || inputString == NULL)
    return -1;

  // (1) Convert string to lowercase.
  std::basic_string<char> encoding(encodingName);
  for (unsigned int i = 0; i < encoding.length(); ++i)
    if (encoding[i] < 0x5B && encoding[i] > 0x40)
      encoding[i] += 0x20;

  DECODER_REGISTRY::iterator j 
    = std::lower_bound(decoderRegistry.begin(), decoderRegistry.end(),
                       EncoderEntry(encoding.c_str(), NULL));

  if (j == decoderRegistry.end() || encoding != (*j).name) return -1;

  if ((*j).function(inputString, outputString)) return 0;
  return 1;
}


// ---------------------------------------------------------------------------
// Now we define the 'simple' decoder functions
// ---------------------------------------------------------------------------

bool DecodeASCII(const char * in, std::basic_string<wchar_t> & out)
{
  out.erase();

  while (*in != '\0') {
    char c = *in;
    if (c > 0x7f || c < 0) return false;
    out += wchar_t(c);
    ++in;
  }

  return true;
}


bool DecodeISO8859_1(const char * in, std::basic_string<wchar_t> & out)
{
  out.erase();

  while (*in != '\0') {
    out += wchar_t(*in);
    ++in;
  } 
  return true;
}


bool DecodeISO8859_2(const char * in, std::basic_string<wchar_t> & out)
{
  out.erase();
  wchar_t w;

  while (*in != '\0') {
    switch (*in) {
    case 0xA1:  w = 0x0104;  break; // LATIN CAPITAL LETTER A WITH OGONEK
    case 0xA2:  w = 0x02D8;  break; // BREVE
    case 0xA3:  w = 0x0141;  break; // LATIN CAPITAL LETTER L WITH STROKE
    case 0xA5:  w = 0x013D;  break; // LATIN CAPITAL LETTER L WITH CARON
    case 0xA6:  w = 0x015A;  break; // LATIN CAPITAL LETTER S WITH ACUTE
    case 0xA9:  w = 0x0160;  break; // LATIN CAPITAL LETTER S WITH CARON
    case 0xAA:  w = 0x015E;  break; // LATIN CAPITAL LETTER S WITH CEDILLA
    case 0xAB:  w = 0x0164;  break; // LATIN CAPITAL LETTER T WITH CARON
    case 0xAC:  w = 0x0179;  break; // LATIN CAPITAL LETTER Z WITH ACUTE
    case 0xAE:  w = 0x017D;  break; // LATIN CAPITAL LETTER Z WITH CARON
    case 0xAF:  w = 0x017B;  break; // LATIN CAPITAL LETTER Z WITH DOT ABOVE
    case 0xB1:  w = 0x0105;  break; // LATIN SMALL LETTER A WITH OGONEK
    case 0xB2:  w = 0x02DB;  break; // OGONEK
    case 0xB3:  w = 0x0142;  break; // LATIN SMALL LETTER L WITH STROKE
    case 0xB5:  w = 0x013E;  break; // LATIN SMALL LETTER L WITH CARON
    case 0xB6:  w = 0x015B;  break; // LATIN SMALL LETTER S WITH ACUTE
    case 0xB7:  w = 0x02C7;  break; // CARON
    case 0xB9:  w = 0x0161;  break; // LATIN SMALL LETTER S WITH CARON
    case 0xBA:  w = 0x015F;  break; // LATIN SMALL LETTER S WITH CEDILLA
    case 0xBB:  w = 0x0165;  break; // LATIN SMALL LETTER T WITH CARON
    case 0xBC:  w = 0x017A;  break; // LATIN SMALL LETTER Z WITH ACUTE
    case 0xBD:  w = 0x02DD;  break; // DOUBLE ACUTE ACCENT
    case 0xBE:  w = 0x017E;  break; // LATIN SMALL LETTER Z WITH CARON
    case 0xBF:  w = 0x017C;  break; // LATIN SMALL LETTER Z WITH DOT ABOVE
    case 0xC0:  w = 0x0154;  break; // LATIN CAPITAL LETTER R WITH ACUTE
    case 0xC3:  w = 0x0102;  break; // LATIN CAPITAL LETTER A WITH BREVE
    case 0xC5:  w = 0x0139;  break; // LATIN CAPITAL LETTER L WITH ACUTE
    case 0xC6:  w = 0x0106;  break; // LATIN CAPITAL LETTER C WITH ACUTE
    case 0xC8:  w = 0x010C;  break; // LATIN CAPITAL LETTER C WITH CARON
    case 0xCA:  w = 0x0118;  break; // LATIN CAPITAL LETTER E WITH OGONEK
    case 0xCC:  w = 0x011A;  break; // LATIN CAPITAL LETTER E WITH CARON
    case 0xCF:  w = 0x010E;  break; // LATIN CAPITAL LETTER D WITH CARON
    case 0xD0:  w = 0x0110;  break; // LATIN CAPITAL LETTER D WITH STROKE
    case 0xD1:  w = 0x0143;  break; // LATIN CAPITAL LETTER N WITH ACUTE
    case 0xD2:  w = 0x0147;  break; // LATIN CAPITAL LETTER N WITH CARON
    case 0xD5:  w = 0x0150;  break; // LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
    case 0xD8:  w = 0x0158;  break; // LATIN CAPITAL LETTER R WITH CARON
    case 0xD9:  w = 0x016E;  break; // LATIN CAPITAL LETTER U WITH RING ABOVE
    case 0xDB:  w = 0x0170;  break; // LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
    case 0xDE:  w = 0x0162;  break; // LATIN CAPITAL LETTER T WITH CEDILLA
    case 0xE0:  w = 0x0155;  break; // LATIN SMALL LETTER R WITH ACUTE
    case 0xE3:  w = 0x0103;  break; // LATIN SMALL LETTER A WITH BREVE
    case 0xE5:  w = 0x013A;  break; // LATIN SMALL LETTER L WITH ACUTE
    case 0xE6:  w = 0x0107;  break; // LATIN SMALL LETTER C WITH ACUTE
    case 0xE8:  w = 0x010D;  break; // LATIN SMALL LETTER C WITH CARON
    case 0xEA:  w = 0x0119;  break; // LATIN SMALL LETTER E WITH OGONEK
    case 0xEC:  w = 0x011B;  break; // LATIN SMALL LETTER E WITH CARON
    case 0xEF:  w = 0x010F;  break; // LATIN SMALL LETTER D WITH CARON
    case 0xF0:  w = 0x0111;  break; // LATIN SMALL LETTER D WITH STROKE
    case 0xF1:  w = 0x0144;  break; // LATIN SMALL LETTER N WITH ACUTE
    case 0xF2:  w = 0x0148;  break; // LATIN SMALL LETTER N WITH CARON
    case 0xF5:  w = 0x0151;  break; // LATIN SMALL LETTER O WITH DOUBLE ACUTE
    case 0xF8:  w = 0x0159;  break; // LATIN SMALL LETTER R WITH CARON
    case 0xF9:  w = 0x016F;  break; // LATIN SMALL LETTER U WITH RING ABOVE
    case 0xFB:  w = 0x0171;  break; // LATIN SMALL LETTER U WITH DOUBLE ACUTE
    case 0xFE:  w = 0x0163;  break; // LATIN SMALL LETTER T WITH CEDILLA
    case 0xFF:  w = 0x02D9;  break; // DOT ABOVE
    default:
      w = wchar_t(*in);
      break;
    }
    out += w;
    ++in;
  }

  return true;
}


bool DecodeISO8859_3(const char * in, std::basic_string<wchar_t> & out)
{
  out.erase();
  wchar_t w;

  while (*in != '\0') {
    switch (*in) {
    case 0xA1:  w = 0x0126;  break; // LATIN CAPITAL LETTER H WITH STROKE
    case 0xA2:  w = 0x02D8;  break; // BREVE
    case 0xA6:  w = 0x0124;  break; // LATIN CAPITAL LETTER H WITH CIRCUMFLEX
    case 0xA9:  w = 0x0130;  break; // LATIN CAPITAL LETTER I WITH DOT ABOVE
    case 0xAA:  w = 0x015E;  break; // LATIN CAPITAL LETTER S WITH CEDILLA
    case 0xAB:  w = 0x011E;  break; // LATIN CAPITAL LETTER G WITH BREVE
    case 0xAC:  w = 0x0134;  break; // LATIN CAPITAL LETTER J WITH CIRCUMFLEX
    case 0xAF:  w = 0x017B;  break; // LATIN CAPITAL LETTER Z WITH DOT ABOVE
    case 0xB1:  w = 0x0127;  break; // LATIN SMALL LETTER H WITH STROKE
    case 0xB6:  w = 0x0125;  break; // LATIN SMALL LETTER H WITH CIRCUMFLEX
    case 0xB9:  w = 0x0131;  break; // LATIN SMALL LETTER DOTLESS I
    case 0xBA:  w = 0x015F;  break; // LATIN SMALL LETTER S WITH CEDILLA
    case 0xBB:  w = 0x011F;  break; // LATIN SMALL LETTER G WITH BREVE
    case 0xBC:  w = 0x0135;  break; // LATIN SMALL LETTER J WITH CIRCUMFLEX
    case 0xBF:  w = 0x017C;  break; // LATIN SMALL LETTER Z WITH DOT ABOVE
    case 0xC5:  w = 0x010A;  break; // LATIN CAPITAL LETTER C WITH DOT ABOVE
    case 0xC6:  w = 0x0108;  break; // LATIN CAPITAL LETTER C WITH CIRCUMFLEX
    case 0xD5:  w = 0x0120;  break; // LATIN CAPITAL LETTER G WITH DOT ABOVE
    case 0xD8:  w = 0x011C;  break; // LATIN CAPITAL LETTER G WITH CIRCUMFLEX
    case 0xDD:  w = 0x016C;  break; // LATIN CAPITAL LETTER U WITH BREVE
    case 0xDE:  w = 0x015C;  break; // LATIN CAPITAL LETTER S WITH CIRCUMFLEX
    case 0xE5:  w = 0x010B;  break; // LATIN SMALL LETTER C WITH DOT ABOVE
    case 0xE6:  w = 0x0109;  break; // LATIN SMALL LETTER C WITH CIRCUMFLEX
    case 0xF5:  w = 0x0121;  break; // LATIN SMALL LETTER G WITH DOT ABOVE
    case 0xF8:  w = 0x011D;  break; // LATIN SMALL LETTER G WITH CIRCUMFLEX
    case 0xFD:  w = 0x016D;  break; // LATIN SMALL LETTER U WITH BREVE
    case 0xFE:  w = 0x015D;  break; // LATIN SMALL LETTER S WITH CIRCUMFLEX
    case 0xFF:  w = 0x02D9;  break; // DOT ABOVE
    default:
      w = wchar_t(*in);
      break;
    }
    out += w;
    ++in;
  }

  return true;
}


bool DecodeISO8859_4(const char * in, std::basic_string<wchar_t> & out)
{
  out.erase();
  wchar_t w;

  while (*in != '\0') {
    switch (*in) {
    case 0xA1:  w = 0x0104;  break; // LATIN CAPITAL LETTER A WITH OGONEK
    case 0xA2:  w = 0x0138;  break; // LATIN SMALL LETTER KRA
    case 0xA3:  w = 0x0156;  break; // LATIN CAPITAL LETTER R WITH CEDILLA
    case 0xA5:  w = 0x0128;  break; // LATIN CAPITAL LETTER I WITH TILDE
    case 0xA6:  w = 0x013B;  break; // LATIN CAPITAL LETTER L WITH CEDILLA
    case 0xA9:  w = 0x0160;  break; // LATIN CAPITAL LETTER S WITH CARON
    case 0xAA:  w = 0x0112;  break; // LATIN CAPITAL LETTER E WITH MACRON
    case 0xAB:  w = 0x0122;  break; // LATIN CAPITAL LETTER G WITH CEDILLA
    case 0xAC:  w = 0x0166;  break; // LATIN CAPITAL LETTER T WITH STROKE
    case 0xAE:  w = 0x017D;  break; // LATIN CAPITAL LETTER Z WITH CARON
    case 0xB1:  w = 0x0105;  break; // LATIN SMALL LETTER A WITH OGONEK
    case 0xB2:  w = 0x02DB;  break; // OGONEK
    case 0xB3:  w = 0x0157;  break; // LATIN SMALL LETTER R WITH CEDILLA
    case 0xB5:  w = 0x0129;  break; // LATIN SMALL LETTER I WITH TILDE
    case 0xB6:  w = 0x013C;  break; // LATIN SMALL LETTER L WITH CEDILLA
    case 0xB7:  w = 0x02C7;  break; // CARON
    case 0xB9:  w = 0x0161;  break; // LATIN SMALL LETTER S WITH CARON
    case 0xBA:  w = 0x0113;  break; // LATIN SMALL LETTER E WITH MACRON
    case 0xBB:  w = 0x0123;  break; // LATIN SMALL LETTER G WITH CEDILLA
    case 0xBC:  w = 0x0167;  break; // LATIN SMALL LETTER T WITH STROKE
    case 0xBD:  w = 0x014A;  break; // LATIN CAPITAL LETTER ENG
    case 0xBE:  w = 0x017E;  break; // LATIN SMALL LETTER Z WITH CARON
    case 0xBF:  w = 0x014B;  break; // LATIN SMALL LETTER ENG
    case 0xC0:  w = 0x0100;  break; // LATIN CAPITAL LETTER A WITH MACRON
    case 0xC7:  w = 0x012E;  break; // LATIN CAPITAL LETTER I WITH OGONEK
    case 0xC8:  w = 0x010C;  break; // LATIN CAPITAL LETTER C WITH CARON
    case 0xCA:  w = 0x0118;  break; // LATIN CAPITAL LETTER E WITH OGONEK
    case 0xCC:  w = 0x0116;  break; // LATIN CAPITAL LETTER E WITH DOT ABOVE
    case 0xCF:  w = 0x012A;  break; // LATIN CAPITAL LETTER I WITH MACRON
    case 0xD0:  w = 0x0110;  break; // LATIN CAPITAL LETTER D WITH STROKE
    case 0xD1:  w = 0x0145;  break; // LATIN CAPITAL LETTER N WITH CEDILLA
    case 0xD2:  w = 0x014C;  break; // LATIN CAPITAL LETTER O WITH MACRON
    case 0xD3:  w = 0x0136;  break; // LATIN CAPITAL LETTER K WITH CEDILLA
    case 0xD9:  w = 0x0172;  break; // LATIN CAPITAL LETTER U WITH OGONEK
    case 0xDD:  w = 0x0168;  break; // LATIN CAPITAL LETTER U WITH TILDE
    case 0xDE:  w = 0x016A;  break; // LATIN CAPITAL LETTER U WITH MACRON
    case 0xE0:  w = 0x0101;  break; // LATIN SMALL LETTER A WITH MACRON
    case 0xE7:  w = 0x012F;  break; // LATIN SMALL LETTER I WITH OGONEK
    case 0xE8:  w = 0x010D;  break; // LATIN SMALL LETTER C WITH CARON
    case 0xEA:  w = 0x0119;  break; // LATIN SMALL LETTER E WITH OGONEK
    case 0xEC:  w = 0x0117;  break; // LATIN SMALL LETTER E WITH DOT ABOVE
    case 0xEF:  w = 0x012B;  break; // LATIN SMALL LETTER I WITH MACRON
    case 0xF0:  w = 0x0111;  break; // LATIN SMALL LETTER D WITH STROKE
    case 0xF1:  w = 0x0146;  break; // LATIN SMALL LETTER N WITH CEDILLA
    case 0xF2:  w = 0x014D;  break; // LATIN SMALL LETTER O WITH MACRON
    case 0xF3:  w = 0x0137;  break; // LATIN SMALL LETTER K WITH CEDILLA
    case 0xF9:  w = 0x0173;  break; // LATIN SMALL LETTER U WITH OGONEK
    case 0xFD:  w = 0x0169;  break; // LATIN SMALL LETTER U WITH TILDE
    case 0xFE:  w = 0x016B;  break; // LATIN SMALL LETTER U WITH MACRON
    case 0xFF:  w = 0x02D9;  break; // DOT ABOVE
    default:
      w = wchar_t(*in);
      break;
    }
    out += w;
    ++in;
  }

  return true;
}


bool DecodeISO8859_15(const char * in, std::basic_string<wchar_t> & out)
{
  out.erase();

  wchar_t w;

  while (*in != '\0') {
    switch (*in) {
    case 0xA4:  w = 0x20AC;  break; // EURO SIGN
    case 0xA6:  w = 0x0160;  break; // LATIN CAPITAL LETTER S WITH CARON
    case 0xA8:  w = 0x0161;  break; // LATIN SMALL LETTER S WITH CARON
    case 0xB4:  w = 0x017D;  break; // LATIN CAPITAL LETTER Z WITH CARON
    case 0xB8:  w = 0x017E;  break; // LATIN SMALL LETTER Z WITH CARON
    case 0xBC:  w = 0x0152;  break; // LATIN CAPITAL LIGATURE OE
    case 0xBD:  w = 0x0153;  break; // LATIN SMALL LIGATURE OE
    case 0xBE:  w = 0x0178;  break; // LATIN CAPITAL LETTER Y WITH DIAERESIS
    default:
      w = wchar_t(*in);
      break;
    }
    out += w;
    ++in;
  }

  return true;
}
#endif

// ---------------------------------------------------------------------------
// Jerry Carter writes:
//
// The UTF-8 encoding / decoding routines were modified from the Apache Xerces
// project.  The original translated from UTF-8 to UTF-16.  In this version, I
// have removed support for surrogate characters.  This removes the difference
// between platforms which treat wchar_t as UTF-16 (Windows) and those which
// use UTF-32 (Linux, MacOS, etc.).
//
// The Apache license appears below (as required).

/*
 * The Apache Software License, Version 1.1
 * 
 * Copyright (c) 1999-2000 The Apache Software Foundation.  All rights
 * reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 * 
 * 4. The names "Xerces" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written 
 *    permission, please contact apache\@apache.org.
 * 
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation, and was
 * originally based on software copyright (c) 1999, International
 * Business Machines, Inc., http://www.ibm.com .  For more information
 * on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

//  gUTFBytes
//      A list of counts of trailing bytes for each initial byte in the input.
//
//  gUTFOffsets
//      A list of values to offset each result char type, according to how
//      many source bytes when into making it.
//
//  gFirstByteMark
//      A list of values to mask onto the first byte of an encoded sequence,
//      indexed by the number of bytes used to create the sequence.

static const char gUTFBytes[256] =
{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
    ,   3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};

static const unsigned long gUTFOffsets[6] =
{  0, 0x3080, 0xE2080, 0x3C82080, 0xFA082080, 0x82082080 };

static const unsigned char gFirstByteMark[7] =
{  0x00, 0x00, 0xC0, 0xE0,
   0xF0, 0xF8, 0xFC };


int SWIutf8towcslen(const unsigned char* src)
{
  int len = 0;

  while (*src != '\0') {
    unsigned int trailingBytes;

    // Get the next leading byte out
    const unsigned char firstByte = (unsigned char) *src;

    // See how many trailing src bytes this sequence is going to require
    trailingBytes = gUTFBytes[firstByte];

    src += trailingBytes + 1;
    len++;
  }
  return len;
}


SWIcharResult SWIutf8towcs( const unsigned char *src, wchar_t *dst, int maxdstlen )
{
  // Get pointers to our start and end points of the input buffer
  const unsigned char* srcPtr = src;
  const unsigned char* srcEnd = src + strlen((const char *)src);
  //wchar_t *dstSave = dst;
  wchar_t *dstEnd = dst+maxdstlen;  /* leave room for null */

  //  We now loop until we run out of input data.
  while (srcPtr < srcEnd) {
    unsigned int trailingBytes;
    unsigned long tmpVal = 0;

    // Get the next leading byte out
    const unsigned char firstByte = (unsigned char) *srcPtr;

    // Special-case ASCII, which is a leading byte value of <= 127
    if (firstByte <= 127) {
      *dst++ = (wchar_t) firstByte;
      srcPtr++;
      continue;
    }

    // See how many trailing src bytes this sequence is going to require
    trailingBytes = gUTFBytes[firstByte];
    
    //  If there are not enough source bytes to do this one, then we
    //  are done. Note that we done >= here because we are implicitly
    //  counting the 1 byte we get no matter what.
    if (srcPtr + trailingBytes >= srcEnd)
      return SWIchar_FAIL;  // ??

    // Looks ok, so lets build up the value
    switch (trailingBytes) {
    case 5: tmpVal += *srcPtr++; tmpVal <<= 6;
    case 4: tmpVal += *srcPtr++; tmpVal <<= 6;
    case 3: tmpVal += *srcPtr++; tmpVal <<= 6;
    case 2: tmpVal += *srcPtr++; tmpVal <<= 6;
    case 1: tmpVal += *srcPtr++; tmpVal <<= 6;
    case 0: tmpVal += *srcPtr++;
      break;
    default:
      return SWIchar_ERROR;
    }
    tmpVal -= gUTFOffsets[trailingBytes];

    // If surrogate pairs would be required for 16-bit characters, fail.
    if (tmpVal & 0xFFFF0000)
      return SWIchar_FAIL;

    if ( dst >= dstEnd ) {
      return SWIchar_BUFFER_OVERFLOW;
    }

    *dst++ = (wchar_t)tmpVal;
  }

  *dst = L'\0';
  
 // return dst-dstSave;

  return SWIchar_SUCCESS;   // check this (CARO)
}

int SWIwcstoutf8len(const wchar_t* src)
{
  int len = 0;

  while (*src != 0) {
    unsigned int encodedBytes;
    wchar_t curVal = (*src++) & 0x0000FFFF;

    // Watchout for surrogates.
    if ((curVal >= 0xD800 && curVal <= 0xDBFF) || curVal == 0xFFFE ||
	curVal == 0xFFFF)
      return -2;

    // Figure out how many bytes we need
    if (curVal < 0x80)                encodedBytes = 1;
    else if (curVal < 0x800)          encodedBytes = 2;
    else if (curVal < 0x10000)        encodedBytes = 3;
    else if (curVal < 0x200000)       encodedBytes = 4;
    else if (curVal < 0x4000000)      encodedBytes = 5;
    else if (curVal <= 0x7FFFFFFF)    encodedBytes = 6;
    else {
      // THIS SHOULD NOT HAPPEN!
      return -2;
    }

    //  And spit out the bytes. We spit them out in reverse order
    //  here, so bump up the output pointer and work down as we go.
    len += encodedBytes;
  }

  return len;
}


SWIcharResult SWIwcstoutf8(const wchar_t *src, unsigned char *dst, int maxdstlen)
{
  //  Get pointers to our start and end points of the input buffer.
  const wchar_t* srcPtr = src;
  const wchar_t* srcEnd = srcPtr + wcslen(src);
  //unsigned char *dstSave = dst;
  unsigned char *dstEnd = dst+maxdstlen;

  while (srcPtr < srcEnd) {
    unsigned int encodedBytes;
    wchar_t curVal = (*srcPtr++) & 0x0000FFFF;

    // Watchout for surrogates.
    if ( ((curVal >= 0xD800) && (curVal <= 0xDFFF)) ||
         ((curVal == 0xFFFE) || curVal == 0xFFFF) )
      return SWIchar_FAIL;

    // Figure out how many bytes we need
    if (curVal < 0x80)                encodedBytes = 1;
    else if (curVal < 0x800)          encodedBytes = 2;
    else if (curVal < 0x10000)        encodedBytes = 3;
    else if (curVal < 0x200000)       encodedBytes = 4;
    else if (curVal < 0x4000000)      encodedBytes = 5;
    else if (curVal <= 0x7FFFFFFF)    encodedBytes = 6;
    else {
      // THIS SHOULD NOT HAPPEN!
      return SWIchar_ERROR;
    }

    //  And spit out the bytes. We spit them out in reverse order
    //  here, so bump up the output pointer and work down as we go.
    dst += encodedBytes;

    if ( dst > dstEnd ) {
      return SWIchar_BUFFER_OVERFLOW;
    }

    switch(encodedBytes) {
    case 6 : *--dst = (unsigned char) ((curVal | 0x80) & 0xBF);
      curVal >>= 6;
    case 5 : *--dst = (unsigned char)((curVal | 0x80) & 0xBF);
      curVal >>= 6;
    case 4 : *--dst = (unsigned char)((curVal | 0x80) & 0xBF);
      curVal >>= 6;
    case 3 : *--dst = (unsigned char)((curVal | 0x80) & 0xBF);
      curVal >>= 6;
    case 2 : *--dst = (unsigned char)((curVal | 0x80) & 0xBF);
      curVal >>= 6;
    case 1 : *--dst = (unsigned char)(curVal | gFirstByteMark[encodedBytes]);
    }

    dst += encodedBytes;
  }
  *dst = '\0';

  // return dst-dstSave

  return SWIchar_SUCCESS; // check this (CARO)
}
