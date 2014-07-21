/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
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
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TEXTEDITOR_SYNTAXHIGHLIGHTER_H
#define TEXTEDITOR_SYNTAXHIGHLIGHTER_H

#include "texteditor_global.h"

#include <QtCore/QObject>
#include <QtGui/QTextLayout>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QSyntaxHighlighterPrivate;
class QTextCharFormat;
class QFont;
class QColor;
class QTextBlockUserData;
class QTextEdit;
QT_END_NAMESPACE

namespace TextEditor {

class SyntaxHighlighterPrivate;

class TEXTEDITOR_EXPORT SyntaxHighlighter : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SyntaxHighlighter)
public:
    enum TextFormatId {
        Normal = 1,
        VisualWhitespace,
        Keyword,
        DataType,
        Decimal,
        BaseN,
        Float,
        Char,
        String,
        Comment,
        Alert,
        Error,
        Function,
        RegionMarker,
        Others,
        Symbol,
        BuiltinFunc,
        Predeclared,
        FuncDecl,
        Placeholder,
        ToDo,
        PreprocessorFormat,
        TextFormatId_MAX
    };
public:
    SyntaxHighlighter(QObject *parent);
    SyntaxHighlighter(QTextDocument *parent);
    SyntaxHighlighter(QTextEdit *parent);
    virtual ~SyntaxHighlighter();

    void setDocument(QTextDocument *doc);
    QTextDocument *document() const;

    void setContextData(const QString &id, const QString &value) {
        m_contextMap[id] = value;
    }
    QString contextData(const QString &id) {
        return m_contextMap[id];
    }

    void setExtraAdditionalFormats(const QTextBlock& block, const QList<QTextLayout::FormatRange> &formats);
    void configureFormat(TextFormatId id, const QTextCharFormat &format);
    virtual void setTabSize(int tabSize);
signals:
    void foldIndentChanged(QTextBlock block);
public Q_SLOTS:
    void rehighlight();
    void rehighlightBlock(const QTextBlock &block);

protected:
    virtual void highlightBlock(const QString &text) = 0;

    void setFormat(int start, int count, const QTextCharFormat &format);
    void setFormat(int start, int count, const QColor &color);
    void setFormat(int start, int count, const QFont &font);
    QTextCharFormat format(int pos) const;

    void applyFormatToSpaces(const QString &text, const QTextCharFormat &format);

    int previousBlockState() const;
    int currentBlockState() const;
    void setCurrentBlockState(int newState);

    void setCurrentBlockUserData(QTextBlockUserData *data);
    QTextBlockUserData *currentBlockUserData() const;

    QTextBlock currentBlock() const;
    QMap<QString,QString> m_contextMap;
protected:
    struct KateFormatMap
    {
        KateFormatMap();
        QHash<QString, TextFormatId> m_ids;
    };
    static const KateFormatMap m_kateFormats;
    QTextCharFormat m_creatorFormats[TextFormatId_MAX];
private:
    Q_DISABLE_COPY(SyntaxHighlighter)
    Q_PRIVATE_SLOT(d_ptr, void _q_reformatBlocks(int from, int charsRemoved, int charsAdded))
    Q_PRIVATE_SLOT(d_ptr, void _q_delayedRehighlight())

    QScopedPointer<SyntaxHighlighterPrivate> d_ptr;
};

} // namespace TextEditor

#endif // TEXTEDITOR_SYNTAXHIGHLIGHTER_H
