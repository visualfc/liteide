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

#ifndef HIGHLIGHTDEFINITION_H
#define HIGHLIGHTDEFINITION_H

#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QSharedPointer>

namespace TextEditor {
namespace Internal {

class KeywordList;
class Context;
class ItemData;

class HighlightDefinition
{
public:
    HighlightDefinition();
    ~HighlightDefinition();

    QSharedPointer<KeywordList> createKeywordList(const QString &list);
    QSharedPointer<KeywordList> keywordList(const QString &list);

    QSharedPointer<Context> createContext(const QString &context, bool initial);
    QSharedPointer<Context> initialContext() const;
    QSharedPointer<Context> context(const QString &context) const;
    const QHash<QString, QSharedPointer<Context> > &contexts() const;

    QSharedPointer<ItemData> createItemData(const QString &itemData);
    QSharedPointer<ItemData> itemData(const QString &itemData) const;

    void setKeywordsSensitive(const QString &sensitivity);
    Qt::CaseSensitivity keywordsSensitive() const;

    void addDelimiters(const QString &characters);
    void removeDelimiters(const QString &characters);
    bool isDelimiter(const QChar &character) const;

    void setSingleLineComment(const QString &start);
    const QString &singleLineComment() const;

    void setCommentAfterWhitespaces(const QString &after);
    bool isCommentAfterWhiteSpaces() const;

    void setMultiLineCommentStart(const QString &start);
    const QString &multiLineCommentStart() const;

    void setMultiLineCommentEnd(const QString &end);
    const QString &multiLineCommentEnd() const;

    void setMultiLineCommentRegion(const QString &region);
    const QString &multiLineCommentRegion() const;

    void setIndentationBasedFolding(const QString &indentationBasedFolding);
    bool isIndentationBasedFolding() const;

private:
    Q_DISABLE_COPY(HighlightDefinition)

    struct GenericHelper
    {
        template <class Element, class Container>
        QSharedPointer<Element> create(const QString &name, Container &container);

        template <class Element, class Container>
        QSharedPointer<Element> find(const QString &name, const Container &container) const;
    };
    GenericHelper m_helper;

    QHash<QString, QSharedPointer<KeywordList> > m_lists;
    QHash<QString, QSharedPointer<Context> > m_contexts;
    QHash<QString, QSharedPointer<ItemData> > m_itemsData;

    QString m_initialContext;

    QString m_singleLineComment;
    bool m_singleLineCommentAfterWhiteSpaces;

    QString m_multiLineCommentStart;
    QString m_multiLineCommentEnd;
    QString m_multiLineCommentRegion;

    Qt::CaseSensitivity m_keywordCaseSensitivity;

    bool m_indentationBasedFolding;

    QSet<QChar> m_delimiters;
};

} // namespace Internal
} // namespace TextEditor

#endif // HIGHLIGHTDEFINITION_H
