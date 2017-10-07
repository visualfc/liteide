/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE Team. All rights reserved.
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
// Module: difflib.h
// Creator: visualfc <visualfc@gmail.com>
//
// C++ difflib is a partial port of github.com/pmezard/go-difflib
// Implement SequenceMatcher and unified_diff
//
// -----------------------------------------------------------------------------
// go-difflib
//
// Package difflib is a partial port of Python difflib module.
//
// It provides tools to compare sequences of strings and generate textual diffs.
//
// The following class and functions have been ported:
//
// - SequenceMatcher
//
// - unified_diff
//
// - context_diff
//
// Getting unified diffs was the main goal of the port. Keep in mind this code
// is mostly suitable to output text differences in a human friendly way, there
// are no guarantees generated diffs are consumable by patch(1).
// ------------------------------------------------------------------------------

#ifndef DIFFLIB_H
#define DIFFLIB_H

#include <QStringList>
#include <QMap>

double calculateRatio(int matches, int length )
{
    if (length > 0) {
        return 2.0 * double(matches) / double(length);
    }
    return 1.0;
}

struct Match
{
    Match(int a, int b, int size)
        : A(a), B(b), Size(size)
    {
    }
    int A;
    int B;
    int Size;
};

struct OpCode
{
    OpCode(quint8 tag, int i1, int i2, int j1, int j2)
        : Tag(tag), I1(i1), I2(i2), J1(j1), J2(j2)
    {
    }

    quint8 Tag;
    int I1;
    int I2;
    int J1;
    int J2;
};

// SequenceMatcher compares sequence of strings. The basic
// algorithm predates, and is a little fancier than, an algorithm
// published in the late 1980's by Ratcliff and Obershelp under the
// hyperbolic name "gestalt pattern matching".  The basic idea is to find
// the longest contiguous matching subsequence that contains no "junk"
// elements (R-O doesn't address junk).  The same idea is then applied
// recursively to the pieces of the sequences to the left and to the right
// of the matching subsequence.  This does not yield minimal edit
// sequences, but does tend to yield matches that "look right" to people.
//
// SequenceMatcher tries to compute a "human-friendly diff" between two
// sequences.  Unlike e.g. UNIX(tm) diff, the fundamental notion is the
// longest *contiguous* & junk-free matching subsequence.  That's what
// catches peoples' eyes.  The Windows(tm) windiff has another interesting
// notion, pairing up elements that appear uniquely in each sequence.
// That, and the method here, appear to yield more intuitive difference
// reports than does diff.  This method appears to be the least vulnerable
// to synching up on blocks of "junk lines", though (like blank lines in
// ordinary text files, or maybe "<P>" lines in HTML files).  That may be
// because this is the only method of the 3 that has a *concept* of
// "junk" <wink>.
//
// Timing:  Basic R-O is cubic time worst case and quadratic time expected
// case.  SequenceMatcher is quadratic time for the worst case and has
// expected-case behavior dependent in a complicated way on how many
// elements the sequences have in common; best case time is linear.
class SequenceMatcher
{
public:
    SequenceMatcher(const QStringList &a, const QStringList &b)
        : IsJunk(0), autoJunk(true)
    {
        SetSeqs(a,b);
    }
    SequenceMatcher(const QStringList &a, const QStringList &b, bool autoJunk, bool (*IsJunk)(QString))
    {
        this->autoJunk = autoJunk;
        this->IsJunk = IsJunk;
        SetSeqs(a,b);
    }
public:
    QStringList a;
    QStringList b;
    QMap<QString,QList<int> > b2j;
    bool (*IsJunk)(QString);
    bool autoJunk;
    QMap<QString,bool> bJunk;
    QList<Match> matchingBlocks;
    QMap<QString,int> fullBCount;
    QMap<QString,bool> bPopular;
    QList<OpCode> opCodes;
public:
    // Set two sequences to be compared.
    void SetSeqs(const QStringList &a, const QStringList &b) {
        SetSeq1(a);
        SetSeq2(b);
    }
    // Set the first sequence to be compared. The second sequence to be compared is
    // not changed.
    //
    // SequenceMatcher computes and caches detailed information about the second
    // sequence, so if you want to compare one sequence S against many sequences,
    // use .SetSeq2(s) once and call .SetSeq1(x) repeatedly for each of the other
    // sequences.
    //
    // See also SetSeqs() and SetSeq2().
    void SetSeq1(const QStringList &a) {
        if (this->a == a) {
            return;
        }
        this->a = a;
        this->matchingBlocks.clear();
        this->opCodes.clear();
    }
    // Set the second sequence to be compared. The first sequence to be compared is
    // not changed.
    void SetSeq2(const QStringList &b) {
        if (this->b == b) {
            return;
        }
        this->b = b;
        this->matchingBlocks.clear();
        this->opCodes.clear();
        this->fullBCount.clear();
        this->chainB();
    }
    void chainB()
    {
        // Populate line -> index mapping
        QMap<QString,QList<int> > b2j;
        for (int i = 0; i < this->b.size(); i++) {
            QString s = b[i];
            QList<int> indices = b2j[s];
            indices.append(i);
            b2j[s] = indices;
        }

        // Purge junk elements
        this->bJunk.clear();
        if (this->IsJunk != 0) {
            QMap<QString,bool> junk = this->bJunk;
            foreach (QString s, b2j.keys()) {
                if (this->IsJunk(s)) {
                    junk[s] = true;
                }
            }
            foreach (QString s, junk.keys()) {
                b2j.remove(s);
            }
        }

        // Purge remaining popular elements
        QMap<QString,bool> popular;
        int n = this->b.size();
        if (this->autoJunk && n >= 200) {
            int ntest = n/100 + 1;
            QMapIterator<QString,QList<int> > i(b2j);
            while (i.hasNext()) {
                i.next();
                if (i.value().size() > ntest) {
                    popular[i.key()] = true;
                }
            }
            foreach (QString s, popular.keys()) {
                b2j.remove(s);
            }
        }
        this->bPopular = popular;
        this->b2j = b2j;
    }
    bool isBJunk(const QString & s)
    {
        return this->bJunk[s];
    }
    // Find longest matching block in a[alo:ahi] and b[blo:bhi].
    //
    // If IsJunk is not defined:
    //
    // Return (i,j,k) such that a[i:i+k] is equal to b[j:j+k], where
    //     alo <= i <= i+k <= ahi
    //     blo <= j <= j+k <= bhi
    // and for all (i',j',k') meeting those conditions,
    //     k >= k'
    //     i <= i'
    //     and if i == i', j <= j'
    //
    // In other words, of all maximal matching blocks, return one that
    // starts earliest in a, and of all those maximal matching blocks that
    // start earliest in a, return the one that starts earliest in b.
    //
    // If IsJunk is defined, first the longest matching block is
    // determined as above, but with the additional restriction that no
    // junk element appears in the block.  Then that block is extended as
    // far as possible by matching (only) junk elements on both sides.  So
    // the resulting block never matches on junk except as identical junk
    // happens to be adjacent to an "interesting" match.
    //
    // If no blocks match, return (alo, blo, 0).
    Match findLongestMatch(int alo, int ahi, int blo, int bhi)
    {
        // CAUTION:  stripping common prefix or suffix would be incorrect.
        // E.g.,
        //    ab
        //    acab
        // Longest matching block is "ab", but if common prefix is
        // stripped, it's "a" (tied with "b").  UNIX(tm) diff does so
        // strip, so ends up claiming that ab is changed to acab by
        // inserting "ca" in the middle.  That's minimal but unintuitive:
        // "it's obvious" that someone inserted "ac" at the front.
        // Windiff ends up at the same place as diff, but by pairing up
        // the unique 'b's and then matching the first two 'a's.
        int besti = alo, bestj = blo, bestsize = 0;

        // find longest junk-free match
        // during an iteration of the loop, j2len[j] = length of longest
        // junk-free match ending with a[i-1] and b[j]
        QMap<int,int> j2len;
        for (int i = alo; i != ahi; i++) {
            // look at all instances of a[i] in b; note that because
            // b2j has no junk keys, the loop is skipped if a[i] is junk
            QMap<int,int> newj2len;
            foreach (int j, this->b2j[this->a[i]]) {
                // a[i] matches b[j]
                if (j < blo) {
                    continue;
                }
                if (j >= bhi) {
                    break;
                }
                int k = j2len[j-1] + 1;
                newj2len[j] = k;
                if (k > bestsize) {
                    besti = i-k+1;
                    bestj = j-k+1;
                    bestsize = k;
                }
            }
            j2len = newj2len;
        }

        // Extend the best by non-junk elements on each end.  In particular,
        // "popular" non-junk elements aren't in b2j, which greatly speeds
        // the inner loop above, but also means "the best" match so far
        // doesn't contain any junk *or* popular non-junk elements.
        while (besti > alo && bestj > blo && !this->isBJunk(this->b[bestj-1]) &&
            this->a[besti-1] == this->b[bestj-1]) {
            besti = besti-1;
            bestj = bestj-1;
            bestsize = bestsize+1;
        }
        while (besti+bestsize < ahi && bestj+bestsize < bhi &&
            !this->isBJunk(this->b[bestj+bestsize]) &&
            this->a[besti+bestsize] == this->b[bestj+bestsize]) {
            bestsize += 1;
        }

        // Now that we have a wholly interesting match (albeit possibly
        // empty!), we may as well suck up the matching junk on each
        // side of it too.  Can't think of a good reason not to, and it
        // saves post-processing the (possibly considerable) expense of
        // figuring out what to do with it.  In the case of an empty
        // interesting match, this is clearly the right thing to do,
        // because no other kind of match is possible in the regions.
        while (besti > alo && bestj > blo && this->isBJunk(this->b[bestj-1]) &&
            this->a[besti-1] == this->b[bestj-1]) {
            besti = besti-1;
            bestj = bestj-1;
            bestsize = bestsize+1;
        }
        while (besti+bestsize < ahi && bestj+bestsize < bhi &&
            this->isBJunk(this->b[bestj+bestsize]) &&
            this->a[besti+bestsize] == this->b[bestj+bestsize]) {
            bestsize += 1;
        }

        return Match(besti,bestj, bestsize);
    }

     void matchBlocks(int alo, int ahi, int blo, int bhi, QList<Match> &matched)
     {
        Match match = this->findLongestMatch(alo, ahi, blo, bhi);
        int i = match.A;
        int j = match.B;
        int k = match.Size;
        if (match.Size > 0 ){
            if (alo < i && blo < j) {
                matchBlocks(alo, i, blo, j, matched);
            }
            matched.append(match);
            if (i+k < ahi && j+k < bhi) {
                matchBlocks(i+k, ahi, j+k, bhi, matched);
            }
        }
    }

    // Return list of triples describing matching subsequences.
    //
    // Each triple is of the form (i, j, n), and means that
    // a[i:i+n] == b[j:j+n].  The triples are monotonically increasing in
    // i and in j. It's also guaranteed that if (i, j, n) and (i', j', n') are
    // adjacent triples in the list, and the second is not the last triple in the
    // list, then i+n != i' or j+n != j'. IOW, adjacent triples never describe
    // adjacent equal blocks.
    //
    // The last triple is a dummy, (len(a), len(b), 0), and is the only
    // triple with n==0.
    QList<Match> GetMatchingBlocks()
    {
        if (!this->matchingBlocks.isEmpty()) {
            return this->matchingBlocks;
        }

        QList<Match> matched;
        matchBlocks(0, this->a.size(), 0, this->b.size(), matched);

        // It's possible that we have adjacent equal blocks in the
        // matching_blocks list now.
        QList<Match> nonAdjacent;
        int i1 = 0, j1 = 0, k1 = 0;
        foreach (Match b, matched) {
            // Is this block adjacent to i1, j1, k1?
            int i2 = b.A;
            int j2 = b.B;
            int k2 = b.Size;
            if (i1+k1 == i2 && j1+k1 == j2) {
                // Yes, so collapse them -- this just increases the length of
                // the first block by the length of the second, and the first
                // block so lengthened remains the block to compare against.
                k1 += k2;
            } else {
                // Not adjacent.  Remember the first block (k1==0 means it's
                // the dummy we started with), and make the second block the
                // new block to compare against.
                if (k1 > 0) {
                    nonAdjacent.append(Match(i1, j1, k1));
                }
                i1 = i2;
                j1 = j2;
                k1 = k2;
            }
        }
        if (k1 > 0 ){
            nonAdjacent.append(Match(i1, j1, k1));
        }

        nonAdjacent.append(Match(this->a.size(), this->b.size(), 0));
        this->matchingBlocks = nonAdjacent;
        return this->matchingBlocks;
    }

    // Return list of 5-tuples describing how to turn a into b.
    //
    // Each tuple is of the form (tag, i1, i2, j1, j2).  The first tuple
    // has i1 == j1 == 0, and remaining tuples have i1 == the i2 from the
    // tuple preceding it, and likewise for j1 == the previous j2.
    //
    // The tags are characters, with these meanings:
    //
    // 'r' (replace):  a[i1:i2] should be replaced by b[j1:j2]
    //
    // 'd' (delete):   a[i1:i2] should be deleted, j1==j2 in this case.
    //
    // 'i' (insert):   b[j1:j2] should be inserted at a[i1:i1], i1==i2 in this case.
    //
    // 'e' (equal):    a[i1:i2] == b[j1:j2]
    QList<OpCode> GetOpCodes()
    {
        if (!this->opCodes.isEmpty()) {
            return this->opCodes;
        }
        int i = 0, j = 0;
        QList<Match> matching = this->GetMatchingBlocks();
        QList<OpCode> opCodes;
        //opCodes.reserve(matching.size());

        foreach (Match m, matching) {
            //  invariant:  we've pumped out correct diffs to change
            //  a[:i] into b[:j], and the next matching block is
            //  a[ai:ai+size] == b[bj:bj+size]. So we need to pump
            //  out a diff to change a[i:ai] into b[j:bj], pump out
            //  the matching block, and move (i,j) beyond the match
            int ai = m.A, bj = m.B, size = m.Size;
            quint8 tag = 0;
            if (i < ai && j < bj) {
                tag = 'r';
            } else if (i < ai) {
                tag = 'd';
            } else if (j < bj) {
                tag = 'i';
            }
            if (tag > 0 ){
                opCodes.append(OpCode(tag, i, ai, j, bj));
            }
            i = ai+size, j = bj+size;
            // the list of matching blocks is terminated by a
            // sentinel with size 0
            if (size > 0) {
                opCodes.append(OpCode('e', ai, i, bj, j));
            }
        }
        this->opCodes = opCodes;
        return this->opCodes;
    }
    // Isolate change clusters by eliminating ranges with no changes.
    //
    // Return a generator of groups with up to n lines of context.
    // Each group is in the same format as returned by GetOpCodes().
    QList<QList<OpCode> > GetGroupedOpCodes(int n)
    {
        if (n < 0) {
            n = 3;
        }
        QList<OpCode> codes = this->GetOpCodes();
        if (codes.size() == 0) {
            codes .append(OpCode('e', 0, 1, 0, 1));
        }
        // Fixup leading and trailing groups if they show no changes.
        if (codes[0].Tag == 'e') {
            OpCode c = codes[0];
            int i1 = c.I1, i2 = c.I2, j1 = c.J1, j2 = c.J2;
            codes[0] = OpCode(c.Tag, qMax(i1, i2-n), i2, qMax(j1, j2-n), j2);
        }
        if (codes[codes.size()-1].Tag == 'e') {
            OpCode c = codes[codes.size()-1];
            int i1 = c.I1, i2 = c.I2, j1 = c.J1, j2 = c.J2;
            codes[codes.size()-1] = OpCode(c.Tag, i1, qMin(i2, i1+n), j1, qMin(j2, j1+n));
        }
        int nn = n + n;
        QList<QList<OpCode> > groups;
        QList<OpCode> group;
        foreach(OpCode c, codes) {
            int i1 = c.I1, i2 = c.I2, j1 = c.J1, j2 = c.J2;
            // End the current group and start a new one whenever
            // there is a large range with no changes.
            if (c.Tag == 'e' && i2-i1 > nn) {
                group.append(OpCode(c.Tag, i1, qMin(i2, i1+n),
                    j1, qMin(j2, j1+n)));
                groups.append(group);
                group.clear();
                i1 = qMax(i1, i2-n), j1 = qMax(j1, j2-n);
            }
            group.append(OpCode(c.Tag, i1, i2, j1, j2));
        }
        if (group.size() > 0 && !(group.size() == 1 && group[0].Tag == 'e')) {
            groups.append(group);
        }
        return groups;
    }
    // Return a measure of the sequences' similarity (float in [0,1]).
    //
    // Where T is the total number of elements in both sequences, and
    // M is the number of matches, this is 2.0*M / T.
    // Note that this is 1 if the sequences are identical, and 0 if
    // they have nothing in common.
    //
    // .Ratio() is expensive to compute if you haven't already computed
    // .GetMatchingBlocks() or .GetOpCodes(), in which case you may
    // want to try .QuickRatio() or .RealQuickRation() first to get an
    // upper bound.
    double Ratio()
    {
        int matches = 0;
        foreach(Match m, this->GetMatchingBlocks()) {
            matches += m.Size;
        }
        return calculateRatio(matches, this->a.size()+this->b.size());
    }
    // Return an upper bound on ratio() relatively quickly.
    //
    // This isn't defined beyond that it is an upper bound on .Ratio(), and
    // is faster to compute.
    double QuickRatio()
    {
        // viewing a and b as multisets, set matches to the cardinality
        // of their intersection; this counts the number of matches
        // without regard to order, so is clearly an upper bound
        if (this->fullBCount.isEmpty()) {
            foreach (QString s, this->b) {
                this->fullBCount[s] = this->fullBCount[s] + 1;
            }
        }

        // avail[x] is the number of times x appears in 'b' less the
        // number of times we've seen it in 'a' so far ... kinda
        QMap<QString,int> avail;
        int matches = 0;
        foreach (QString s, this->a) {
            int n = avail.value(s,this->fullBCount[s]);
            avail[s] = n - 1;
            if (n > 0) {
                matches += 1;
            }
        }
        return calculateRatio(matches, this->a.size()+this->b.size());
    }
    // Return an upper bound on ratio() very quickly.
    //
    // This isn't defined beyond that it is an upper bound on .Ratio(), and
    // is faster to compute than either .Ratio() or .QuickRatio().
    double RealQuickRatio()
    {
        int la = this->a.size(), lb = this->b.size();
        return calculateRatio(qMin(la, lb), la+lb);
    }
};

// Convert range to the "ed" format
QString formatRangeUnified(int start, int stop)
{
    // Per the diff spec at http://www.unix.org/single_unix_specification/
    int beginning = start + 1; // lines start numbering with one
    int length = stop - start;
    if (length == 1) {
        return QString("%1").arg(beginning);
    }
    if (length == 0 ) {
        beginning -= 1; // empty ranges begin at line just before the range
    }
    return QString("%1,%2").arg(beginning).arg(length);
}

// Unified diff parameters
struct UnifiedDiff
{
    UnifiedDiff() : Context(0)
    {
    }
    QStringList A; // First sequence lines
    QString FromFile;   // First file name
    QString FromDate;   // First file time
    QStringList B; // Second sequence lines
    QString ToFile;   // Second file name
    QString ToDate;   // Second file time
    QString Eol;   // Headers end of line, defaults to LF
    int Context;      // Number of context lines
};

// Compare two sequences of lines; generate the delta as a unified diff.
//
// Unified diffs are a compact way of showing line changes and a few
// lines of context.  The number of context lines is set by 'n' which
// defaults to three.
//
// By default, the diff control lines (those with ---, +++, or @@) are
// created with a trailing newline.  This is helpful so that inputs
// created from file.readlines() result in diffs that are suitable for
// file.writelines() since both the inputs and outputs have trailing
// newlines.
//
// For inputs that do not have trailing newlines, set the lineterm
// argument to "" so that the output will be uniformly newline free.
//
// The unidiff format normally has a header for filenames and modification
// times.  Any or all of these may be specified using strings for
// 'fromfile', 'tofile', 'fromfiledate', and 'tofiledate'.
// The modification times are normally expressed in the ISO 8601 format.
QStringList GetUnifiedDiffStringList(const UnifiedDiff &diff)
{
    QStringList bufList;

    bool started = false;
    SequenceMatcher *m =  new SequenceMatcher(diff.A, diff.B);
    foreach(QList<OpCode> g, m->GetGroupedOpCodes(diff.Context)) {
        if (!started) {
            started = true;
            QString fromDate;
            if (!diff.FromDate.isEmpty()) {
                fromDate = "\t" + diff.FromDate;
            }
            QString toDate;
            if (!diff.ToDate.isEmpty()) {
                toDate = "\t" + diff.ToDate;
            }
            if (!diff.FromFile.isEmpty() || !diff.ToFile.isEmpty()) {
                bufList << QString("--- %1%2").arg(diff.FromFile).arg(fromDate);
                bufList << QString("+++ %1%2").arg(diff.ToFile).arg(toDate);
            }
        }
        OpCode first = g.first(), last = g.last();
        QString range1 = formatRangeUnified(first.I1, last.I2);
        QString range2 = formatRangeUnified(first.J1, last.J2);
        bufList << QString("@@ -%1 +%2 @@").arg(range1).arg(range2);
        foreach (OpCode c, g) {
            int i1 = c.I1, i2 = c.I2, j1 = c.J1, j2 = c.J2;
            if (c.Tag == 'e') {
                foreach (QString line, diff.A.mid(i1,i2-i1)) {
                    bufList << " "+line;
                }
                continue;
            }
            if (c.Tag == 'r' || c.Tag == 'd') {
                foreach (QString line, diff.A.mid(i1,i2-i1)) {
                    bufList << "-"+line;
                }
            }
            if (c.Tag == 'r' || c.Tag == 'i') {
                foreach (QString line, diff.B.mid(j1,j2-j1)) {
                    bufList << "+" + line;
                }
            }
        }
    }
    return bufList;
}

// Like WriteUnifiedDiff but returns the diff a string.
QString GetUnifiedDiffString(const UnifiedDiff &diff)
{
    QString sep = diff.Eol;
    if (sep.isEmpty()) {
        sep = "\n";
    }
    return GetUnifiedDiffStringList(diff).join(sep);
}

QStringList UnifiedDiffLines(const QStringList &a, const QStringList &b)
{
    UnifiedDiff diff;
    diff.A = a;
    diff.B = b;
    diff.FromFile = "Original";
    diff.ToFile = "Current";
    diff.Context = 3;
    diff.Eol = "\n";
    return GetUnifiedDiffStringList(diff);
}

QString UnifiedDiffLines(const QString &a, const QString &b)
{
    UnifiedDiff diff;
    diff.A = a.split("\n");
    diff.B = b.split("\n");
    diff.FromFile = "Original";
    diff.ToFile = "Current";
    diff.Context = 3;
    diff.Eol = "\n";
    return GetUnifiedDiffString(diff);
}

#endif // DIFFLIB_H
