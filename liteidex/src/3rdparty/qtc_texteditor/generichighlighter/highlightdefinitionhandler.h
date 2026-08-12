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

#ifndef HIGHLIGHTDEFINITIONHANDLER_H
#define HIGHLIGHTDEFINITIONHANDLER_H

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QSharedPointer>
#include <QtCore/QStack>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QtXml/QXmlDefaultHandler>
#else
#include <QtCore5Compat/QXmlDefaultHandler>
#endif

namespace TextEditor {
namespace Internal {

class KeywordList;
class Context;
class Rule;
class HighlightDefinition;

class HighlightDefinitionHandler : public QXmlDefaultHandler
{
public:
    HighlightDefinitionHandler(const QSharedPointer<HighlightDefinition> &definition);
    ~HighlightDefinitionHandler();

    bool startDocument();
    bool endDocument();
    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &atts);
    bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    bool characters(const QString &ch);

private:
    void listElementStarted(const QXmlAttributes &atts);
    void itemElementStarted();
    void contextElementStarted(const QXmlAttributes &atts);
    void itemDataElementStarted(const QXmlAttributes &atts) const;
    void commentElementStarted(const QXmlAttributes &atts) const;
    void keywordsElementStarted(const QXmlAttributes &atts) const;
    void foldingElementStarted(const QXmlAttributes &atts) const;
    void ruleElementStarted(const QXmlAttributes &atts, const QSharedPointer<Rule> &rule);

    // Specific rules.
    void detectCharStarted(const QXmlAttributes &atts);
    void detect2CharsStarted(const QXmlAttributes &atts);
    void anyCharStarted(const QXmlAttributes &atts);
    void stringDetectedStarted(const QXmlAttributes &atts);
    void regExprStarted(const QXmlAttributes &atts);
    void keywordStarted(const QXmlAttributes &atts);
    void intStarted(const QXmlAttributes &atts);
    void floatStarted(const QXmlAttributes &atts);
    void hlCOctStarted(const QXmlAttributes &atts);
    void hlCHexStarted(const QXmlAttributes &atts);
    void hlCStringCharStarted(const QXmlAttributes &atts);
    void hlCCharStarted(const QXmlAttributes &atts);
    void rangeDetectStarted(const QXmlAttributes &atts);
    void lineContinue(const QXmlAttributes &atts);
    void includeRulesStarted(const QXmlAttributes &atts);
    void detectSpacesStarted(const QXmlAttributes &atts);
    void detectIdentifier(const QXmlAttributes &atts);

    void processIncludeRules() const;
    void processIncludeRules(const QSharedPointer<Context> &context) const;

    QSharedPointer<HighlightDefinition> m_definition;

    bool m_processingKeyword;
    QString m_currentKeyword;
    QSharedPointer<KeywordList> m_currentList;
    QSharedPointer<Context> m_currentContext;
    QStack<QSharedPointer<Rule> > m_currentRule;

    bool m_initialContext;
};

} // namespace Internal
} // namespace TextEditor

#endif // HIGHLIGHTDEFINITIONHANDLER_H
