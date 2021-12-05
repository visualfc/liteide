/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** No Commercial Usage
**
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#include "rule.h"
#include "highlighterexception.h"
#include "progressdata.h"
#include "highlightdefinition.h"
#include "reuse.h"

#include <QtCore/QStringList>

#include <functional>

using namespace TextEditor;
using namespace Internal;

const QLatin1Char Rule::kBackSlash('\\');
const QLatin1Char Rule::kUnderscore('_');
const QLatin1Char Rule::kDot('.');
const QLatin1Char Rule::kPlus('+');
const QLatin1Char Rule::kMinus('-');
const QLatin1Char Rule::kZero('0');
const QLatin1Char Rule::kQuote('\"');
const QLatin1Char Rule::kSingleQuote('\'');
const QLatin1Char Rule::kQuestion('?');
const QLatin1Char Rule::kX('x');
const QLatin1Char Rule::kA('a');
const QLatin1Char Rule::kB('b');
const QLatin1Char Rule::kE('e');
const QLatin1Char Rule::kF('f');
const QLatin1Char Rule::kN('n');
const QLatin1Char Rule::kR('r');
const QLatin1Char Rule::kT('t');
const QLatin1Char Rule::kV('v');
const QLatin1Char Rule::kOpeningBrace('{');
const QLatin1Char Rule::kClosingBrace('}');

Rule::Rule(bool consumesNonSpace) :
    m_lookAhead(false), m_firstNonSpace(false), m_column(-1), m_consumesNonSpace(consumesNonSpace)
{}

Rule::~Rule()
{}

void Rule::setContext(const QString &context)
{ m_context = context; }

const QString &Rule::context() const
{ return m_context; }

void Rule::setItemData(const QString &itemData)
{ m_itemData = itemData; }

const QString &Rule::itemData() const
{ return m_itemData; }

void Rule::setBeginRegion(const QString &begin)
{ m_beginRegion = begin; }

const QString &Rule::beginRegion() const
{ return m_beginRegion; }

void Rule::setEndRegion(const QString &end)
{ m_endRegion = end; }

const QString &Rule::endRegion() const
{ return m_endRegion; }

void Rule::setLookAhead(const QString &lookAhead)
{ m_lookAhead = toBool(lookAhead); }

bool Rule::isLookAhead() const
{ return m_lookAhead; }

void Rule::setFirstNonSpace(const QString &firstNonSpace)
{ m_firstNonSpace = toBool(firstNonSpace); }

bool Rule::isFirstNonSpace() const
{ return m_firstNonSpace; }

void Rule::setColumn(const QString &column)
{
    bool ok;
    m_column = column.toInt(&ok);
    if (!ok)
        m_column = -1;
}

int Rule::column() const
{ return m_column; }

void Rule::addChild(const QSharedPointer<Rule> &rule)
{ m_children.append(rule); }

bool Rule::hasChildren() const
{ return !m_children.isEmpty(); }

const QList<QSharedPointer<Rule> > &Rule::children() const
{ return m_children; }

void Rule::setDefinition(const QSharedPointer<HighlightDefinition> &definition)
{ m_definition = definition; }

const QSharedPointer<HighlightDefinition> &Rule::definition() const
{ return m_definition; }

template <class predicate_t>
bool Rule::predicateMatchSucceed(const QString &text,
                                 const int length,
                                 ProgressData *progress,
                                 const predicate_t &p) const
{
    int original = progress->offset();
    while (progress->offset() < length && p(text.at(progress->offset())))
        progress->incrementOffset();

    if (original != progress->offset())
        return true;

    return false;
}

bool Rule::charPredicateMatchSucceed(const QString &text,
                                     const int length,
                                     ProgressData *progress,
                                     bool (QChar::* predicate)() const) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    return predicateMatchSucceed(text, length, progress, std::mem_fn(predicate));
#else
    return predicateMatchSucceed(text, length, progress, std::mem_fun_ref(predicate));
#endif
}

bool Rule::charPredicateMatchSucceed(const QString &text,
                                     const int length,
                                     ProgressData *progress,
                                     bool (*predicate)(const QChar &)) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    return predicateMatchSucceed(text, length, progress, predicate);
#else
    return predicateMatchSucceed(text, length, progress, std::ptr_fun(predicate));
#endif
}

bool Rule::matchSucceed(const QString &text, const int length, ProgressData *progress)
{ 
    if (m_firstNonSpace && !progress->isOnlySpacesSoFar())
        return false;

    if (m_column != -1 && m_column != progress->offset())
        return false;

    int original = progress->offset();
    if (doMatchSucceed(text, length, progress)) {
        if (progress->isOnlySpacesSoFar() && !m_lookAhead && m_consumesNonSpace)
            progress->setOnlySpacesSoFar(false);

        if (m_lookAhead)
            progress->setOffset(original);

        return true;
    }

    return false;
}

Rule *Rule::clone() const
{ return doClone(); }

void Rule::progressFinished()
{ doProgressFinished(); }

bool Rule::matchCharacter(const QString &text,
                          const int length,
                          ProgressData *progress,
                          const QChar &c,
                          bool saveRestoreOffset) const
{
    Q_UNUSED(length)
    Q_ASSERT(progress->offset() < length);

    if (text.at(progress->offset()) == c) {
        if (saveRestoreOffset)
            progress->saveOffset();
        progress->incrementOffset();
        return true;
    }

    return false;
}

bool Rule::matchEscapeSequence(const QString &text,
                               const int length,
                               ProgressData *progress,
                               bool saveRestoreOffset) const
{
    if (matchCharacter(text, length, progress, kBackSlash, saveRestoreOffset)) {

        if (progress->offset() < length) {
            const QChar &c = text.at(progress->offset());
            if (c == kA || c == kB || c == kE || c == kF || c == kN || c == kR || c == kT ||
                c == kV || c == kQuestion || c == kSingleQuote || c == kQuote || c == kBackSlash) {
                progress->incrementOffset();
                return true;
            } else if (saveRestoreOffset) {
                    progress->restoreOffset();
            }
        } else if (saveRestoreOffset) {
            progress->restoreOffset();
        }
    }

    return false;
}

bool Rule::matchOctalSequence(const QString &text,
                              const int length,
                              ProgressData *progress,
                              bool saveRestoreOffset) const
{
    // An octal sequence is identified as in the C++ Standard.
    // octal-escape-sequence:
    //   \ octal-digit
    //   \ octal-digit octal-digit
    //   \ octal-digit octal-digit octal-digit

    if (matchCharacter(text, length, progress, kBackSlash, saveRestoreOffset)) {

        int count = 0;
        while (progress->offset() < length &&
               count < 3 &&
               isOctalDigit(text.at(progress->offset()))) {
            ++count;
            progress->incrementOffset();
        }

        if (count > 0)
            return true;
        else if (saveRestoreOffset)
            progress->restoreOffset();
    }

    return false;
}

bool Rule::matchHexSequence(const QString &text,
                            const int length,
                            ProgressData *progress,
                            bool saveRestoreOffset) const
{
    // An hex sequence is identified as in the C++ Standard.
    // hexadecimal-escape-sequence:
    //   \x hexadecimal-digit
    //   hexadecimal-escape-sequence hexadecimal-digit

    if (matchCharacter(text, length, progress, kBackSlash, saveRestoreOffset)) {

        if (progress->offset() < length && matchCharacter(text, length, progress, kX, false)) {
            bool found = false;
            while (progress->offset() < length && isHexDigit(text.at(progress->offset()))) {
                if (!found)
                    found = true;
                progress->incrementOffset();
            }

            if (found)
                return true;
            else if (saveRestoreOffset)
                progress->restoreOffset();
        } else if (saveRestoreOffset) {
            progress->restoreOffset();
        }
    }

    return false;
}
