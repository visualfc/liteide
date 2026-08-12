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

#ifndef SPECIFICRULES_H
#define SPECIFICRULES_H

#include "rule.h"
#include "dynamicrule.h"

#include <QtCore/QChar>
#include <QtCore/QStringList>
#include <QRegExp>
#include <QtCore/QSharedPointer>

namespace TextEditor {
namespace Internal {

class KeywordList;
class HighlightDefinition;

class DetectCharRule : public DynamicRule
{
public:
    virtual ~DetectCharRule() {}

    void setChar(const QString &character);

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual DetectCharRule *doClone() const { return new DetectCharRule(*this); }
    virtual void doReplaceExpressions(const QStringList &captures);

    QChar m_char;
};

class Detect2CharsRule : public DynamicRule
{
public:
    virtual ~Detect2CharsRule() {}

    void setChar(const QString &character);
    void setChar1(const QString &character);

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual Detect2CharsRule *doClone() const { return new Detect2CharsRule(*this); }
    virtual void doReplaceExpressions(const QStringList &captures);

    QChar m_char;
    QChar m_char1;
};

class AnyCharRule : public Rule
{
public:
    virtual ~AnyCharRule() {}

    void setCharacterSet(const QString &s);

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual AnyCharRule *doClone() const { return new AnyCharRule(*this); }

    QString m_characterSet;
};

class StringDetectRule : public DynamicRule
{
public:
    virtual ~StringDetectRule() {}

    void setString(const QString &s);
    void setInsensitive(const QString &insensitive);

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual StringDetectRule *doClone() const { return new StringDetectRule(*this); }
    virtual void doReplaceExpressions(const QStringList &captures);

    QString m_string;
    int m_length;
    Qt::CaseSensitivity m_caseSensitivity;
};

class RegExprRule : public DynamicRule
{
public:
    RegExprRule() : m_onlyBegin(false), m_isCached(false) {}
    virtual ~RegExprRule() {}

    void setPattern(const QString &pattern);
    void setInsensitive(const QString &insensitive);
    void setMinimal(const QString &minimal);    

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual RegExprRule *doClone() const { return new RegExprRule(*this); }
    virtual void doReplaceExpressions(const QStringList &captures);
    virtual void doProgressFinished();

    bool isExactMatch(ProgressData *progress);

    bool m_onlyBegin;
    bool m_isCached;
    int m_offset;
    int m_length;
    QStringList m_captures;
    QRegExp m_expression;
};

class KeywordRule : public Rule
{
public:
    KeywordRule(const QSharedPointer<HighlightDefinition> &definition);
    virtual ~KeywordRule();

    void setInsensitive(const QString &insensitive);
    void setList(const QString &listName);

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual KeywordRule *doClone() const { return new KeywordRule(*this); }

    bool m_overrideGlobal;
    Qt::CaseSensitivity m_localCaseSensitivity;
    QSharedPointer<KeywordList> m_list;
};

class IntRule : public Rule
{
public:
    virtual ~IntRule() {}

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual IntRule *doClone() const { return new IntRule(*this); }
};

class FloatRule : public Rule
{
public:
    virtual ~FloatRule() {}

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual FloatRule *doClone() const { return new FloatRule(*this); }
};

class HlCOctRule : public Rule
{
public:
    virtual ~HlCOctRule() {}

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual HlCOctRule *doClone() const { return new HlCOctRule(*this); }
};

class HlCHexRule : public Rule
{
public:
    virtual ~HlCHexRule() {}

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual HlCHexRule *doClone() const { return new HlCHexRule(*this); }
};

class HlCStringCharRule : public Rule
{
public:
    virtual ~HlCStringCharRule() {}

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual HlCStringCharRule *doClone() const { return new HlCStringCharRule(*this); }
};

class HlCCharRule : public Rule
{
public:
    virtual ~HlCCharRule() {}

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual HlCCharRule *doClone() const { return new HlCCharRule(*this); }
};

class RangeDetectRule : public Rule
{
public:
    virtual ~RangeDetectRule() {}

    void setChar(const QString &character);
    void setChar1(const QString &character);

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual RangeDetectRule *doClone() const { return new RangeDetectRule(*this); }

    QChar m_char;
    QChar m_char1;
};

class LineContinueRule : public Rule
{
public:
    virtual ~LineContinueRule() {}

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual LineContinueRule *doClone() const { return new LineContinueRule(*this); }
};

class DetectSpacesRule : public Rule
{
public:
    DetectSpacesRule();
    virtual ~DetectSpacesRule() {}

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual DetectSpacesRule *doClone() const { return new DetectSpacesRule(*this); }
};

class DetectIdentifierRule : public Rule
{
public:
    virtual ~DetectIdentifierRule() {}

private:
    virtual bool doMatchSucceed(const QString &text,
                                const int length,
                                ProgressData *progress);
    virtual DetectIdentifierRule *doClone() const { return new DetectIdentifierRule(*this); }
};

} // namespace Internal
} // namespace TextEditor

#endif // SPECIFICRULES_H
