/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C++ is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FLOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef MYSQLX_COLLATIONS_H
#define MYSQLX_COLLATIONS_H

#include "common.h"

/*
  List of built-in character sets and collations supported by version 5.7
  of MySQL Server. The lists are taken from:

  http://dev.mysql.com/doc/refman/5.7/en/charset-charsets.html
*/

namespace mysqlx {

/**
  This macro defines a list of known MySQL character set names. For each
  character set name CS listed here, there is `CharacterSet::CS` constant
  in the CharacterSet enumeration. For example constant `CharacterSet::latin1`
  is the id of the "latin1" character set.

  @ingroup devapi_res
*/

#define CS_LIST(X) \
  X(big5)  \
  X(dec8)  \
  X(cp850)  \
  X(hp8)  \
  X(koi8r)  \
  X(latin1)  \
  X(latin2)  \
  X(swe7)  \
  X(ascii)  \
  X(ujis)  \
  X(sjis)  \
  X(hebrew)  \
  X(tis620)  \
  X(euckr)  \
  X(koi8u)  \
  X(gb2312)  \
  X(greek)  \
  X(cp1250)  \
  X(gbk)  \
  X(latin5)  \
  X(armscii8)  \
  X(utf8)  \
  X(ucs2)  \
  X(cp866)  \
  X(keybcs2)  \
  X(macce)  \
  X(macroman)  \
  X(cp852)  \
  X(latin7)  \
  X(utf8mb4)  \
  X(cp1251)  \
  X(utf16)  \
  X(utf16le)  \
  X(cp1256)  \
  X(cp1257)  \
  X(utf32)  \
  X(binary)  \
  X(geostd8)  \
  X(cp932)  \
  X(eucjpms)  \
  X(gb18030)  \


// Fix for Solaris build
#ifdef CS
#  undef CS
#endif


/*
  Enumeration of known character sets. For each character set CS listed
  in CS_LIST() macro, there is CharacterSet::CS constant in this
  enumeration.
*/


enum class CharacterSet : unsigned short
{
  #define CS_ENUM(CS)  CS,
  CS_LIST(CS_ENUM)
};


#define CS_NAME_SWITCH(CS)  case CharacterSet::CS: return #CS;

/**
  Returns name of a character set given by its id.

  @ingroup devapi_res
*/

inline
const char* characterSetName(CharacterSet id)
{
  switch (id)
  {
    CS_LIST(CS_NAME_SWITCH)
  default:
    THROW("Unknown character set id");
  }
}


/**
  Structure that provides information about character set collation.

  For each known collation COLL over character set CS, there is static object
  `Collation<CS>::%COLL` of type `CollationInfo` which describes
  collation COLL. For example `Collation<CharacterSet::latin1>::%swedish_ci` is
  an `CollationInfo` object which describes the `swedish_ci` collation over
  `latin1` character set.

  @ingroup devapi_res
*/

struct PUBLIC_API CollationInfo
{
  /// Numeric collation id, as used by MySQL server.

  unsigned id() const { return m_id; }

  /// String name of a collation, such as "latin1_generic_ci".

  const char *getName() const { return m_name; }

  /// Id of the character set of this collation.

  CharacterSet getCharacterSet() const { return m_cs; }

  /**
    Returns true if given collation is case sensitive. Binary
    collations are assumed to be case sensitive.
  */

  bool  isCaseSensitive() const { return m_case != case_ci; }

  /// Returns true if this is binary collation.

  bool  isBinary() const { return m_case == case_bin; }


  bool  operator==(const CollationInfo &other) const
  {
    return m_id == other.m_id;
  }

  bool  operator!=(const CollationInfo &other) const
  {
    return !operator==(other);
  }

private:

  enum coll_case { case_bin, case_ci, case_cs };

  CharacterSet m_cs;
  unsigned m_id;
  coll_case m_case;
  const char *m_name;

public:

  struct Access;
  friend Access;
};


template <CharacterSet CS> struct Collation;


/*
  The Collation<CS>::COLL constants are generated from information provided by
  COLLATION_XXX() macros.Each line X(CS, ID, COLL, CASE) in the expansion of
  a COLLATION_XXX() macro declares collation with name COLL for character set
  CS. ID is the MySQL id number for the collation. CASE is one of ci, cs or bin
  and indicates whether it is case insensitive, sensitive or binary collation,
  respectively.
*/

/**
  @macro
  Macro defining known collations for a given character set.
  @{
*/

#define COLLATIONS_big5(X)  \
  X(big5,1,chinese,ci) \
  X(big5,84,bin,bin) \

#define COLLATIONS_dec8(X) \
  X(dec8,3,swedish,ci)  \
  X(dec8,69,bin,bin)  \

#define COLLATIONS_cp850(X) \
  X(cp850,4,general,ci)  \
  X(cp850,80,bin,bin)  \

#define COLLATIONS_hp8(X) \
  X(hp8,6,english,ci)  \
  X(hp8,72,bin,bin)  \

#define COLLATIONS_koi8r(X) \
  X(koi8r,7,general,ci)  \
  X(koi8r,74,bin,bin)  \

#define COLLATIONS_latin1(X) \
  X(latin1,5,german1,ci)  \
  X(latin1,8,swedish,ci)  \
  X(latin1,15,danish,ci)  \
  X(latin1,31,german2,ci)  \
  X(latin1,47,bin,bin)  \
  X(latin1,48,general,ci)  \
  X(latin1,49,general,cs)  \
  X(latin1,94,spanish,ci)  \

#define COLLATIONS_latin2(X) \
  X(latin2,2,czech,cs)  \
  X(latin2,9,general,ci)  \
  X(latin2,21,hungarian,ci)  \
  X(latin2,27,croatian,ci)  \
  X(latin2,77,bin,bin)  \

#define COLLATIONS_swe7(X) \
  X(swe7,10,swedish,ci)  \
  X(swe7,82,bin,bin)  \

#define COLLATIONS_ascii(X) \
  X(ascii,11,general,ci)  \
  X(ascii,65,bin,bin)  \

#define COLLATIONS_ujis(X) \
  X(ujis,12,japanese,ci)  \
  X(ujis,91,bin,bin)  \

#define COLLATIONS_sjis(X) \
  X(sjis,13,japanese,ci)  \
  X(sjis,88,bin,bin)  \

#define COLLATIONS_hebrew(X) \
  X(hebrew,16,general,ci)  \
  X(hebrew,71,bin,bin)  \

#define COLLATIONS_tis620(X) \
  X(tis620,18,thai,ci)  \
  X(tis620,89,bin,bin)  \

#define COLLATIONS_euckr(X) \
  X(euckr,19,korean,ci)  \
  X(euckr,85,bin,bin)  \

#define COLLATIONS_koi8u(X) \
  X(koi8u,22,general,ci)  \
  X(koi8u,75,bin,bin)  \

#define COLLATIONS_gb2312(X) \
  X(gb2312,24,chinese,ci)  \
  X(gb2312,86,bin,bin)  \

#define COLLATIONS_greek(X) \
  X(greek,25,general,ci)  \
  X(greek,70,bin,bin)  \

#define COLLATIONS_cp1250(X) \
  X(cp1250,26,general,ci)  \
  X(cp1250,34,czech,cs)  \
  X(cp1250,44,croatian,ci)  \
  X(cp1250,66,bin,bin)  \
  X(cp1250,99,polish,ci)  \

#define COLLATIONS_gbk(X) \
  X(gbk,28,chinese,ci)  \
  X(gbk,87,bin,bin)  \

#define COLLATIONS_latin5(X) \
  X(latin5,30,turkish,ci)  \
  X(latin5,78,bin,bin)  \

#define COLLATIONS_armscii8(X) \
  X(armscii8,32,general,ci)  \
  X(armscii8,64,bin,bin)  \

#define COLLATIONS_utf8(X) \
  X(utf8,33,general,ci)  \
  X(utf8,83,bin,bin)  \
  X(utf8,192,unicode,ci)  \
  X(utf8,193,icelandic,ci)  \
  X(utf8,194,latvian,ci)  \
  X(utf8,195,romanian,ci)  \
  X(utf8,196,slovenian,ci)  \
  X(utf8,197,polish,ci)  \
  X(utf8,198,estonian,ci)  \
  X(utf8,199,spanish,ci)  \
  X(utf8,200,swedish,ci)  \
  X(utf8,201,turkish,ci)  \
  X(utf8,202,czech,ci)  \
  X(utf8,203,danish,ci)  \
  X(utf8,204,lithuanian,ci)  \
  X(utf8,205,slovak,ci)  \
  X(utf8,206,spanish2,ci)  \
  X(utf8,207,roman,ci)  \
  X(utf8,208,persian,ci)  \
  X(utf8,209,esperanto,ci)  \
  X(utf8,210,hungarian,ci)  \
  X(utf8,211,sinhala,ci)  \
  X(utf8,212,german2,ci)  \
  X(utf8,213,croatian,ci)  \
  X(utf8,214,unicode_520,ci)  \
  X(utf8,215,vietnamese,ci)  \
  X(utf8,223,general_mysql500,ci)  \

#define COLLATIONS_ucs2(X) \
  X(ucs2,35,general,ci)  \
  X(ucs2,90,bin,bin)  \
  X(ucs2,128,unicode,ci)  \
  X(ucs2,129,icelandic,ci)  \
  X(ucs2,130,latvian,ci)  \
  X(ucs2,131,romanian,ci)  \
  X(ucs2,132,slovenian,ci)  \
  X(ucs2,133,polish,ci)  \
  X(ucs2,134,estonian,ci)  \
  X(ucs2,135,spanish,ci)  \
  X(ucs2,136,swedish,ci)  \
  X(ucs2,137,turkish,ci)  \
  X(ucs2,138,czech,ci)  \
  X(ucs2,139,danish,ci)  \
  X(ucs2,140,lithuanian,ci)  \
  X(ucs2,141,slovak,ci)  \
  X(ucs2,142,spanish2,ci)  \
  X(ucs2,143,roman,ci)  \
  X(ucs2,144,persian,ci)  \
  X(ucs2,145,esperanto,ci)  \
  X(ucs2,146,hungarian,ci)  \
  X(ucs2,147,sinhala,ci)  \
  X(ucs2,148,german2,ci)  \
  X(ucs2,149,croatian,ci)  \
  X(ucs2,150,unicode_520,ci)  \
  X(ucs2,151,vietnamese,ci)  \
  X(ucs2,159,general_mysql500,ci)  \

#define COLLATIONS_cp866(X) \
  X(cp866,36,general,ci)  \
  X(cp866,68,bin,bin)  \

#define COLLATIONS_keybcs2(X) \
  X(keybcs2,37,general,ci)  \
  X(keybcs2,73,bin,bin)  \

#define COLLATIONS_macce(X) \
  X(macce,38,general,ci)  \
  X(macce,43,bin,bin)  \

#define COLLATIONS_macroman(X) \
  X(macroman,39,general,ci)  \
  X(macroman,53,bin,bin)  \

#define COLLATIONS_cp852(X) \
  X(cp852,40,general,ci)  \
  X(cp852,81,bin,bin)  \

#define COLLATIONS_latin7(X) \
  X(latin7,20,estonian,cs)  \
  X(latin7,41,general,ci)  \
  X(latin7,42,general,cs)  \
  X(latin7,79,bin,bin)  \

#define COLLATIONS_utf8mb4(X) \
  X(utf8mb4,45,general,ci)  \
  X(utf8mb4,46,bin,bin)  \
  X(utf8mb4,224,unicode,ci)  \
  X(utf8mb4,225,icelandic,ci)  \
  X(utf8mb4,226,latvian,ci)  \
  X(utf8mb4,227,romanian,ci)  \
  X(utf8mb4,228,slovenian,ci)  \
  X(utf8mb4,229,polish,ci)  \
  X(utf8mb4,230,estonian,ci)  \
  X(utf8mb4,231,spanish,ci)  \
  X(utf8mb4,232,swedish,ci)  \
  X(utf8mb4,233,turkish,ci)  \
  X(utf8mb4,234,czech,ci)  \
  X(utf8mb4,235,danish,ci)  \
  X(utf8mb4,236,lithuanian,ci)  \
  X(utf8mb4,237,slovak,ci)  \
  X(utf8mb4,238,spanish2,ci)  \
  X(utf8mb4,239,roman,ci)  \
  X(utf8mb4,240,persian,ci)  \
  X(utf8mb4,241,esperanto,ci)  \
  X(utf8mb4,242,hungarian,ci)  \
  X(utf8mb4,243,sinhala,ci)  \
  X(utf8mb4,244,german2,ci)  \
  X(utf8mb4,245,croatian,ci)  \
  X(utf8mb4,246,unicode_520,ci)  \
  X(utf8mb4,247,vietnamese,ci)  \

#define COLLATIONS_cp1251(X) \
  X(cp1251,14,bulgarian,ci)  \
  X(cp1251,23,ukrainian,ci)  \
  X(cp1251,50,bin,bin)  \
  X(cp1251,51,general,ci)  \
  X(cp1251,52,general,cs)  \

#define COLLATIONS_utf16(X) \
  X(utf16,54,general,ci)  \
  X(utf16,55,bin,bin)  \
  X(utf16,101,unicode,ci)  \
  X(utf16,102,icelandic,ci)  \
  X(utf16,103,latvian,ci)  \
  X(utf16,104,romanian,ci)  \
  X(utf16,105,slovenian,ci)  \
  X(utf16,106,polish,ci)  \
  X(utf16,107,estonian,ci)  \
  X(utf16,108,spanish,ci)  \
  X(utf16,109,swedish,ci)  \
  X(utf16,110,turkish,ci)  \
  X(utf16,111,czech,ci)  \
  X(utf16,112,danish,ci)  \
  X(utf16,113,lithuanian,ci)  \
  X(utf16,114,slovak,ci)  \
  X(utf16,115,spanish2,ci)  \
  X(utf16,116,roman,ci)  \
  X(utf16,117,persian,ci)  \
  X(utf16,118,esperanto,ci)  \
  X(utf16,119,hungarian,ci)  \
  X(utf16,120,sinhala,ci)  \
  X(utf16,121,german2,ci)  \
  X(utf16,122,croatian,ci)  \
  X(utf16,123,unicode_520,ci)  \
  X(utf16,124,vietnamese,ci)  \

#define COLLATIONS_utf16le(X) \
  X(utf16le,56,general,ci)  \
  X(utf16le,62,bin,bin)  \

#define COLLATIONS_cp1256(X) \
  X(cp1256,57,general,ci)  \
  X(cp1256,67,bin,bin)  \

#define COLLATIONS_cp1257(X) \
  X(cp1257,29,lithuanian,ci)  \
  X(cp1257,58,bin,bin)  \
  X(cp1257,59,general,ci)  \

#define COLLATIONS_utf32(X) \
  X(utf32,60,general,ci)  \
  X(utf32,61,bin,bin)  \
  X(utf32,160,unicode,ci)  \
  X(utf32,161,icelandic,ci)  \
  X(utf32,162,latvian,ci)  \
  X(utf32,163,romanian,ci)  \
  X(utf32,164,slovenian,ci)  \
  X(utf32,165,polish,ci)  \
  X(utf32,166,estonian,ci)  \
  X(utf32,167,spanish,ci)  \
  X(utf32,168,swedish,ci)  \
  X(utf32,169,turkish,ci)  \
  X(utf32,170,czech,ci)  \
  X(utf32,171,danish,ci)  \
  X(utf32,172,lithuanian,ci)  \
  X(utf32,173,slovak,ci)  \
  X(utf32,174,spanish2,ci)  \
  X(utf32,175,roman,ci)  \
  X(utf32,176,persian,ci)  \
  X(utf32,177,esperanto,ci)  \
  X(utf32,178,hungarian,ci)  \
  X(utf32,179,sinhala,ci)  \
  X(utf32,180,german2,ci)  \
  X(utf32,181,croatian,ci)  \
  X(utf32,182,unicode_520,ci)  \
  X(utf32,183,vietnamese,ci)  \

#define COLLATIONS_binary(X) \
  X(binary,63,bin,bin)  \

#define COLLATIONS_geostd8(X) \
  X(geostd8,92,general,ci)  \
  X(geostd8,93,bin,bin)  \

#define COLLATIONS_cp932(X) \
  X(cp932,95,japanese,ci)  \
  X(cp932,96,bin,bin)  \

#define COLLATIONS_eucjpms(X) \
  X(eucjpms,97,japanese,ci)  \
  X(eucjpms,98,bin,bin)  \

#define COLLATIONS_gb18030(X) \
  X(gb18030,248,chinese,ci)  \
  X(gb18030,249,bin,bin)  \
  X(gb18030,250,unicode_520,ci)  \

/**@}*/


/*
  Generate declarations of Collation<CS>::COLL constants using
  COLLATINS_XXX() macros. The list CS_LIST() is processed using
  COLL_DECL() macro, which for each character set CS declares
  specialization Collation<CS> of the Collation<> template and
  declares collation constants within this specialization.

  The collation constant declarations are generated by processing
  COLLATIONS_CS() list with COLL_CONST() macro. The name of each
  constant is generated by COLL_CONST_NAME() macro. The convention
  used is that case insensitive and case sensitive collations get
  a _ci or _cs suffix, respectively, while binary collation constant
  have no suffix to the collation name.

  Collation constants declared here are defined in file result.cc.
*/

#define COLL_DECL(CS) \
template<> struct Collation<CharacterSet::CS> \
{ COLLATIONS_##CS(COLL_CONST) }; \

#define COLL_CONST(CS,ID,COLL,CASE) \
static PUBLIC_API const CollationInfo COLL_CONST_NAME(COLL,CASE);

#define COLL_CONST_NAME(COLL,CASE) COLL_CONST_NAME_##CASE(COLL)

#define COLL_CONST_NAME_bin(COLL) COLL
#define COLL_CONST_NAME_ci(COLL)  COLL##_ci
#define COLL_CONST_NAME_cs(COLL)  COLL##_cs

CS_LIST(COLL_DECL)

}  // mysqlx

#endif
