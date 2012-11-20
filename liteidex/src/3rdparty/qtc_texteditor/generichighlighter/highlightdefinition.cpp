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

#include "highlightdefinition.h"
#include "highlighterexception.h"
#include "context.h"
#include "keywordlist.h"
#include "itemdata.h"
#include "reuse.h"

#include <QtCore/QString>

using namespace TextEditor;
using namespace Internal;

HighlightDefinition::HighlightDefinition() :
    m_singleLineCommentAfterWhiteSpaces(false),
    m_keywordCaseSensitivity(Qt::CaseSensitive),
    m_indentationBasedFolding(false)
{
    QString s(QLatin1String(".():!+,-<=>%&/;?[]^{|}~\\*, \t"));
    foreach (const QChar &c, s)
        m_delimiters.insert(c);
}

HighlightDefinition::~HighlightDefinition()
{}

template <class Element, class Container>
QSharedPointer<Element> HighlightDefinition::
GenericHelper::create(const QString &name, Container &container)
{
    if (name.isEmpty())
        throw HighlighterException();

    if (container.contains(name))
        throw HighlighterException();

    return container.insert(name, QSharedPointer<Element>(new Element)).value();
}

template <class Element, class Container>
QSharedPointer<Element> HighlightDefinition::
GenericHelper::find(const QString &name, const Container &container) const
{
    typename Container::const_iterator it = container.find(name);
    if (it == container.end())
        throw HighlighterException();

    return it.value();
}

QSharedPointer<KeywordList> HighlightDefinition::createKeywordList(const QString &list)
{ return m_helper.create<KeywordList>(list, m_lists); }

QSharedPointer<KeywordList> HighlightDefinition::keywordList(const QString &list)
{ return m_helper.find<KeywordList>(list, m_lists); }

QSharedPointer<Context> HighlightDefinition::createContext(const QString &context, bool initial)
{
    if (initial)
        m_initialContext = context;

    QSharedPointer<Context> newContext = m_helper.create<Context>(context, m_contexts);
    newContext->setName(context);
    return newContext;
}

QSharedPointer<Context> HighlightDefinition::initialContext() const
{ return m_helper.find<Context>(m_initialContext, m_contexts); }

QSharedPointer<Context> HighlightDefinition::context(const QString &context) const
{ return m_helper.find<Context>(context, m_contexts); }

const QHash<QString, QSharedPointer<Context> > &HighlightDefinition::contexts() const
{ return m_contexts; }

QSharedPointer<ItemData> HighlightDefinition::createItemData(const QString &itemData)
{ return m_helper.create<ItemData>(itemData, m_itemsData); }

QSharedPointer<ItemData> HighlightDefinition::itemData(const QString &itemData) const
{ return m_helper.find<ItemData>(itemData, m_itemsData); }

void HighlightDefinition::setSingleLineComment(const QString &start)
{ m_singleLineComment = start; }

const QString &HighlightDefinition::singleLineComment() const
{ return m_singleLineComment; }

void HighlightDefinition::setCommentAfterWhitespaces(const QString &after)
{
    if (after == QLatin1String("afterwhitespace"))
        m_singleLineCommentAfterWhiteSpaces = true;
}

bool HighlightDefinition::isCommentAfterWhiteSpaces() const
{ return m_singleLineCommentAfterWhiteSpaces; }

void HighlightDefinition::setMultiLineCommentStart(const QString &start)
{ m_multiLineCommentStart = start; }

const QString &HighlightDefinition::multiLineCommentStart() const
{ return m_multiLineCommentStart; }

void HighlightDefinition::setMultiLineCommentEnd(const QString &end)
{ m_multiLineCommentEnd = end; }

const QString &HighlightDefinition::multiLineCommentEnd() const
{ return m_multiLineCommentEnd; }

void HighlightDefinition::setMultiLineCommentRegion(const QString &region)
{ m_multiLineCommentRegion = region; }

const QString &HighlightDefinition::multiLineCommentRegion() const
{ return m_multiLineCommentRegion; }

void HighlightDefinition::removeDelimiters(const QString &characters)
{
    for (int i = 0; i < characters.length(); ++i)
        m_delimiters.remove(characters.at(i));
}

void HighlightDefinition::addDelimiters(const QString &characters)
{
    for (int i = 0; i < characters.length(); ++i) {
        if (!m_delimiters.contains(characters.at(i)))
            m_delimiters.insert(characters.at(i));
    }
}

bool HighlightDefinition::isDelimiter(const QChar &character) const
{
    if (m_delimiters.contains(character))
        return true;
    return false;
}

void HighlightDefinition::setKeywordsSensitive(const QString &sensitivity)
{
    if (!sensitivity.isEmpty())
        m_keywordCaseSensitivity = toCaseSensitivity(toBool(sensitivity));
}

Qt::CaseSensitivity HighlightDefinition::keywordsSensitive() const
{ return m_keywordCaseSensitivity; }

void HighlightDefinition::setIndentationBasedFolding(const QString &indentationBasedFolding)
{ m_indentationBasedFolding = toBool(indentationBasedFolding); }

bool HighlightDefinition::isIndentationBasedFolding() const
{ return m_indentationBasedFolding; }
