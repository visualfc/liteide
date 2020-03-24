/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: checkdata.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef CHECKDATA_H
#define CHECKDATA_H


/*
zlib-1.2.7
doc/txtvsbin.txt

A Fast Method for Identifying Plain Text Files
==============================================


Introduction
------------

Given a file coming from an unknown source, it is sometimes desirable
to find out whether the format of that file is plain text.  Although
this may appear like a simple task, a fully accurate detection of the
file type requires heavy-duty semantic analysis on the file contents.
It is, however, possible to obtain satisfactory results by employing
various heuristics.

Previous versions of PKZip and other zip-compatible compression tools
were using a crude detection scheme: if more than 80% (4/5) of the bytes
found in a certain buffer are within the range [7..127], the file is
labeled as plain text, otherwise it is labeled as binary.  A prominent
limitation of this scheme is the restriction to Latin-based alphabets.
Other alphabets, like Greek, Cyrillic or Asian, make extensive use of
the bytes within the range [128..255], and texts using these alphabets
are most often misidentified by this scheme; in other words, the rate
of false negatives is sometimes too high, which means that the recall
is low.  Another weakness of this scheme is a reduced precision, due to
the false positives that may occur when binary files containing large
amounts of textual characters are misidentified as plain text.

In this article we propose a new, simple detection scheme that features
a much increased precision and a near-100% recall.  This scheme is
designed to work on ASCII, Unicode and other ASCII-derived alphabets,
and it handles single-byte encodings (ISO-8859, MacRoman, KOI8, etc.)
and variable-sized encodings (ISO-2022, UTF-8, etc.).  Wider encodings
(UCS-2/UTF-16 and UCS-4/UTF-32) are not handled, however.


The Algorithm
-------------

The algorithm works by dividing the set of bytecodes [0..255] into three
categories:
- The white list of textual bytecodes:
  9 (TAB), 10 (LF), 13 (CR), 32 (SPACE) to 255.
- The gray list of tolerated bytecodes:
  7 (BEL), 8 (BS), 11 (VT), 12 (FF), 26 (SUB), 27 (ESC).
- The black list of undesired, non-textual bytecodes:
  0 (NUL) to 6, 14 to 31.

If a file contains at least one byte that belongs to the white list and
no byte that belongs to the black list, then the file is categorized as
plain text; otherwise, it is categorized as binary.  (The boundary case,
when the file is empty, automatically falls into the latter category.)


Rationale
---------

The idea behind this algorithm relies on two observations.

The first observation is that, although the full range of 7-bit codes
[0..127] is properly specified by the ASCII standard, most control
characters in the range [0..31] are not used in practice.  The only
widely-used, almost universally-portable control codes are 9 (TAB),
10 (LF) and 13 (CR).  There are a few more control codes that are
recognized on a reduced range of platforms and text viewers/editors:
7 (BEL), 8 (BS), 11 (VT), 12 (FF), 26 (SUB) and 27 (ESC); but these
codes are rarely (if ever) used alone, without being accompanied by
some printable text.  Even the newer, portable text formats such as
XML avoid using control characters outside the list mentioned here.

The second observation is that most of the binary files tend to contain
control characters, especially 0 (NUL).  Even though the older text
detection schemes observe the presence of non-ASCII codes from the range
[128..255], the precision rarely has to suffer if this upper range is
labeled as textual, because the files that are genuinely binary tend to
contain both control characters and codes from the upper range.  On the
other hand, the upper range needs to be labeled as textual, because it
is used by virtually all ASCII extensions.  In particular, this range is
used for encoding non-Latin scripts.

Since there is no counting involved, other than simply observing the
presence or the absence of some byte values, the algorithm produces
consistent results, regardless what alphabet encoding is being used.
(If counting were involved, it could be possible to obtain different
results on a text encoded, say, using ISO-8859-16 versus UTF-8.)

There is an extra category of plain text files that are "polluted" with
one or more black-listed codes, either by mistake or by peculiar design
considerations.  In such cases, a scheme that tolerates a small fraction
of black-listed codes would provide an increased recall (i.e. more true
positives).  This, however, incurs a reduced precision overall, since
false positives are more likely to appear in binary files that contain
large chunks of textual data.  Furthermore, "polluted" plain text should
be regarded as binary by general-purpose text detection schemes, because
general-purpose text processing algorithms might not be applicable.
Under this premise, it is safe to say that our detection method provides
a near-100% recall.

Experiments have been run on many files coming from various platforms
and applications.  We tried plain text files, system logs, source code,
formatted office documents, compiled object code, etc.  The results
confirm the optimistic assumptions about the capabilities of this
algorithm.


--
Cosmin Truta
Last updated: 2006-May-28
*/

#include <QByteArray>
#include <QChar>

/* ===========================================================================
 * Check if the data type is TEXT or BINARY, using the following algorithm:
 * - TEXT if the two conditions below are satisfied:
 *    a) There are no non-portable control characters belonging to the
 *       "black list" (0..6, 14..25, 28..31).
 *    b) There is at least one printable character belonging to the
 *       "white list" (9 {TAB}, 10 {LF}, 13 {CR}, 32..255).
 * - BINARY otherwise.
 * - The following partially-portable control characters form a
 *   "gray list" that is ignored in this detection algorithm:
 *   (7 {BEL}, 8 {BS}, 11 {VT}, 12 {FF}, 26 {SUB}, 27 {ESC}).
 * IN assertion: the fields Freq of dyn_ltree are set.
 */

inline bool IsBinaryCode(unsigned short c)
{
    if (c < 7 ||( c > 13 && c < 26) ||( c > 27 && c < 32) )
        return true;
    return false;
}

inline bool IsBinaryChar(unsigned char c)
{
    if (c < 7 ||( c > 13 && c < 26) ||( c > 27 && c < 32) )
        return true;
    return false;
}


inline bool HasBinaryData(const QByteArray &data, int checkSize /*= 32*/)
{
    // UTF16 byte order marks
    static const char bigEndianBOM[] = "\xFE\xFF";
    static const char littleEndianBOM[] = "\xFF\xFE";
    if (data.startsWith(bigEndianBOM) || data.startsWith(littleEndianBOM))
        return true;

    // Check the first 32 bytes (see shared-mime spec)
    const char *p = data.constData();
    const char *e = p + qMin(checkSize, data.size());
    for ( ; p < e; ++p) {
        if (IsBinaryChar((unsigned char)*p)) {
            return true;
        }
    }

    return false;
}

//inline bool isTextData(const QByteArray &data)
//{
//    // UTF16 byte order marks
//    static const char bigEndianBOM[] = "\xFE\xFF";
//    static const char littleEndianBOM[] = "\xFF\xFE";
//    if (data.startsWith(bigEndianBOM) || data.startsWith(littleEndianBOM))
//        return true;

//    // Check the first 32 bytes (see shared-mime spec)
//    const char *p = data.constData();
//    const char *e = p + qMin(32, data.size());
//    for ( ; p < e; ++p) {
//        if ((unsigned char)(*p) < 32 && *p != 9 && *p !=10 && *p != 13)
//            return false;
//    }

//    return true;
//}

#endif // CHECKDATA_H
