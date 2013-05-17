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

#include "highlightdefinitionhandler.h"
#include "highlightdefinition.h"
#include "specificrules.h"
#include "itemdata.h"
#include "keywordlist.h"
#include "context.h"
#include "reuse.h"
#include "manager2.h"
#include "highlighterexception.h"
#include <QDebug>

#include <QtCore/QLatin1String>

using namespace TextEditor;
using namespace Internal;

namespace {
    static const QLatin1String kName("name");
    static const QLatin1String kList("list");
    static const QLatin1String kItem("item");
    static const QLatin1String kContext("context");
    static const QLatin1String kAttribute("attribute");
    static const QLatin1String kDynamic("dynamic");
    static const QLatin1String kFallthrough("fallthrough");
    static const QLatin1String kLineEndContext("lineEndContext");
    static const QLatin1String kLineBeginContext("lineBeginContext");
    static const QLatin1String kFallthroughContext("fallthroughContext");
    static const QLatin1String kBeginRegion("beginRegion");
    static const QLatin1String kEndRegion("endRegion");
    static const QLatin1String kLookAhead("lookAhead");
    static const QLatin1String kFirstNonSpace("firstNonSpace");
    static const QLatin1String kColumn("column");
    static const QLatin1String kItemData("itemData");
    static const QLatin1String kDefStyleNum("defStyleNum");
    static const QLatin1String kColor("color");
    static const QLatin1String kSelColor("selColor");
    static const QLatin1String kItalic("italic");
    static const QLatin1String kBold("bold");
    static const QLatin1String kUnderline("underline");
    static const QLatin1String kStrikeout("strikeOut");
    static const QLatin1String kSpellChecking("spellChecking");
    static const QLatin1String kChar("char");
    static const QLatin1String kChar1("char1");
    static const QLatin1String kString("String");
    static const QLatin1String kInsensitive("insensitive");
    static const QLatin1String kMinimal("minimal");
    static const QLatin1String kKeywords("keywords");
    static const QLatin1String kCaseSensitive("casesensitive");
    static const QLatin1String kWeakDeliminator("weakDeliminator");
    static const QLatin1String kAdditionalDeliminator("additionalDeliminator");
    static const QLatin1String kWordWrapDeliminator("wordWrapDeliminator");
    static const QLatin1String kComment("comment");
    static const QLatin1String kPosition("position");
    static const QLatin1String kSingleLine("singleline");
    static const QLatin1String kMultiLine("multiline");
    static const QLatin1String kStart("start");
    static const QLatin1String kEnd("end");
    static const QLatin1String kRegion("region");
    static const QLatin1String kDetectChar("DetectChar");
    static const QLatin1String kDetect2Chars("Detect2Chars");
    static const QLatin1String kAnyChar("AnyChar");
    static const QLatin1String kStringDetect("StringDetect");
    static const QLatin1String kRegExpr("RegExpr");
    static const QLatin1String kKeyword("keyword");
    static const QLatin1String kInt("Int");
    static const QLatin1String kFloat("Float");
    static const QLatin1String kHlCOct("HlCOct");
    static const QLatin1String kHlCHex("HlCHex");
    static const QLatin1String kHlCStringChar("HlCStringChar");
    static const QLatin1String kHlCChar("HlCChar");
    static const QLatin1String kRangeDetect("RangeDetect");
    static const QLatin1String kLineContinue("LineContinue");
    static const QLatin1String kIncludeRules("IncludeRules");
    static const QLatin1String kDetectSpaces("DetectSpaces");
    static const QLatin1String kDetectIdentifier("DetectIdentifier");
    static const QLatin1String kLanguage("language");
    static const QLatin1String kExtensions("extensions");
    static const QLatin1String kIncludeAttrib("includeAttrib");
    static const QLatin1String kFolding("folding");
    static const QLatin1String kIndentationSensitive("indentationsensitive");
    static const QLatin1String kHash("#");
    static const QLatin1String kDoubleHash("##");
}

HighlightDefinitionHandler::
HighlightDefinitionHandler(const QSharedPointer<HighlightDefinition> &definition) :
    m_definition(definition),
    m_processingKeyword(false),
    m_initialContext(true)
{}

HighlightDefinitionHandler::~HighlightDefinitionHandler()
{}

bool HighlightDefinitionHandler::startDocument()
{
    return true;
}

bool HighlightDefinitionHandler::endDocument()
{
    processIncludeRules();
    return true;
}

bool HighlightDefinitionHandler::startElement(const QString &,
                                              const QString &,
                                              const QString &qName,
                                              const QXmlAttributes &atts)
{
    if (qName == kList) {
        listElementStarted(atts);
    } else if (qName == kItem) {
        itemElementStarted();
    } else if (qName == kContext) {
        contextElementStarted(atts);
    } else if (qName == kItemData) {
        itemDataElementStarted(atts);
    } else if (qName == kComment) {
        commentElementStarted(atts);
    } else if (qName == kKeywords) {
        keywordsElementStarted(atts);
    } else if (qName == kFolding) {
        foldingElementStarted(atts);
    } else if (qName == kDetectChar) {
        detectCharStarted(atts);
    } else if (qName == kDetect2Chars) {
        detect2CharsStarted(atts);
    } else if (qName == kAnyChar) {
        anyCharStarted(atts);
    } else if (qName == kStringDetect) {
        stringDetectedStarted(atts);
    } else if (qName == kRegExpr) {
        regExprStarted(atts);
    } else if (qName == kKeyword) {
        keywordStarted(atts);
    } else if (qName == kInt) {
        intStarted(atts);
    } else if (qName == kFloat) {
        floatStarted(atts);
    } else if (qName == kHlCOct) {
        hlCOctStarted(atts);
    } else if (qName == kHlCHex) {
        hlCHexStarted(atts);
    } else if (qName == kHlCStringChar) {
        hlCStringCharStarted(atts);
    } else if (qName == kHlCChar) {
        hlCCharStarted(atts);
    } else if (qName == kRangeDetect) {
        rangeDetectStarted(atts);
    } else if (qName == kLineContinue) {
        lineContinue(atts);
    } else if (qName == kIncludeRules) {
        includeRulesStarted(atts);
    } else if (qName == kDetectSpaces) {
        detectSpacesStarted(atts);
    } else if (qName == kDetectIdentifier) {
        detectIdentifier(atts);
    }

    return true;
}

bool HighlightDefinitionHandler::endElement(const QString &, const QString &, const QString &qName)
{
    if (qName == kItem) {
        m_currentList->addKeyword(m_currentKeyword.trimmed());
        m_processingKeyword = false;
    } else if (qName == kDetectChar || qName == kDetect2Chars || qName == kAnyChar ||
               qName == kStringDetect || qName == kRegExpr || qName == kKeyword || qName == kInt ||
               qName == kFloat || qName == kHlCOct || qName == kHlCHex || qName == kHlCStringChar ||
               qName == kHlCChar || qName == kRangeDetect || qName == kLineContinue ||
               qName == kDetectSpaces || qName == kDetectIdentifier) {
        m_currentRule.pop();
    }

    return true;
}

bool HighlightDefinitionHandler::characters(const QString& ch)
{
    // Character data of an element may be reported in more than one chunk.
    if (m_processingKeyword)
        m_currentKeyword.append(ch);

    return true;
}

void HighlightDefinitionHandler::listElementStarted(const QXmlAttributes &atts)
{
    m_currentList = m_definition->createKeywordList(atts.value(kName));
}

void HighlightDefinitionHandler::itemElementStarted()
{
    m_currentKeyword.clear();
    m_processingKeyword = true;
}

void HighlightDefinitionHandler::contextElementStarted(const QXmlAttributes &atts)
{
    m_currentContext = m_definition->createContext(atts.value(kName), m_initialContext);
    m_currentContext->setDefinition(m_definition);
    m_currentContext->setItemData(atts.value(kAttribute));
    m_currentContext->setDynamic(atts.value(kDynamic));
    m_currentContext->setFallthrough(atts.value(kFallthrough));
    m_currentContext->setFallthroughContext(atts.value(kFallthroughContext));
    m_currentContext->setLineBeginContext(atts.value(kLineBeginContext));
    m_currentContext->setLineEndContext(atts.value(kLineEndContext));

    m_initialContext = false;
}

void HighlightDefinitionHandler::ruleElementStarted(const QXmlAttributes &atts,
                                                    const QSharedPointer<Rule> &rule)
{
    // The definition of a rule is not necessarily the same of its enclosing context because of
    // externally included rules.
    rule->setDefinition(m_definition);
    rule->setItemData(atts.value(kAttribute));
    rule->setContext(atts.value(kContext));
    rule->setBeginRegion(atts.value(kBeginRegion));
    rule->setEndRegion(atts.value(kEndRegion));
    rule->setLookAhead(atts.value(kLookAhead));
    rule->setFirstNonSpace(atts.value(kFirstNonSpace));
    rule->setColumn(atts.value(kColumn));

    if (m_currentRule.isEmpty())
        m_currentContext->addRule(rule);
    else
        m_currentRule.top()->addChild(rule);

    m_currentRule.push(rule);
}

void HighlightDefinitionHandler::itemDataElementStarted(const QXmlAttributes &atts) const
{
    QSharedPointer<ItemData> itemData = m_definition->createItemData(atts.value(kName));
    itemData->setStyle(atts.value(kDefStyleNum));
    itemData->setColor(atts.value(kColor));
    itemData->setSelectionColor(atts.value(kSelColor));
    itemData->setItalic(atts.value(kItalic));
    itemData->setBold(atts.value(kBold));
    itemData->setUnderlined(atts.value(kUnderline));
    itemData->setStrikeOut(atts.value(kStrikeout));
    itemData->setSpellChecking(atts.value(kSpellChecking));
}

void HighlightDefinitionHandler::commentElementStarted(const QXmlAttributes &atts) const
{
    const QString &commentType = atts.value(kName);
    if (commentType.compare(kSingleLine, Qt::CaseInsensitive) == 0) {
        m_definition->setSingleLineComment(atts.value(kStart));
        m_definition->setCommentAfterWhitespaces(atts.value(kPosition));
    } else if (commentType.compare(kMultiLine, Qt::CaseInsensitive) == 0) {
        m_definition->setMultiLineCommentStart(atts.value(kStart));
        m_definition->setMultiLineCommentEnd(atts.value(kEnd));
        m_definition->setMultiLineCommentRegion(atts.value(kRegion));
    }
}

void HighlightDefinitionHandler::keywordsElementStarted(const QXmlAttributes &atts) const
{
    // Global case sensitivity appears last in the document (required by dtd) and is set here.
    m_definition->setKeywordsSensitive(atts.value(kCaseSensitive));
    m_definition->removeDelimiters(atts.value(kWeakDeliminator));
    m_definition->addDelimiters(atts.value(kAdditionalDeliminator));
    //@todo: wordWrapDelimiters?
}

void HighlightDefinitionHandler::foldingElementStarted(const QXmlAttributes &atts) const
{
    m_definition->setIndentationBasedFolding(atts.value(kIndentationSensitive));
}

void HighlightDefinitionHandler::detectCharStarted(const QXmlAttributes &atts)
{
    DetectCharRule *rule = new DetectCharRule;
    rule->setChar(atts.value(kChar));
    rule->setActive(atts.value(kDynamic));
    ruleElementStarted(atts, QSharedPointer<Rule>(rule));
}

void HighlightDefinitionHandler::detect2CharsStarted(const QXmlAttributes &atts)
{
    Detect2CharsRule *rule = new Detect2CharsRule;
    rule->setChar(atts.value(kChar));
    rule->setChar1(atts.value(kChar1));
    rule->setActive(atts.value(kDynamic));
    ruleElementStarted(atts, QSharedPointer<Rule>(rule));
}

void HighlightDefinitionHandler::anyCharStarted(const QXmlAttributes &atts)
{
    AnyCharRule *rule = new AnyCharRule;
    rule->setCharacterSet(atts.value(kString));
    ruleElementStarted(atts, QSharedPointer<Rule>(rule));
}

void HighlightDefinitionHandler::stringDetectedStarted(const QXmlAttributes &atts)
{
    StringDetectRule *rule = new StringDetectRule;
    rule->setString(atts.value(kString));
    rule->setInsensitive(atts.value(kInsensitive));
    rule->setActive(atts.value(kDynamic));
    ruleElementStarted(atts, QSharedPointer<Rule>(rule));
}

void HighlightDefinitionHandler::regExprStarted(const QXmlAttributes &atts)
{
    RegExprRule *rule = new RegExprRule;
    rule->setPattern(atts.value(kString));
    rule->setMinimal(atts.value(kMinimal));
    rule->setInsensitive(atts.value(kInsensitive));
    rule->setActive(atts.value(kDynamic));
    ruleElementStarted(atts, QSharedPointer<Rule>(rule));
}

void HighlightDefinitionHandler::keywordStarted(const QXmlAttributes &atts)
{
    KeywordRule *rule = new KeywordRule(m_definition);
    rule->setList(atts.value(kString));
    rule->setInsensitive(atts.value(kInsensitive));
    ruleElementStarted(atts, QSharedPointer<Rule>(rule));
}

void HighlightDefinitionHandler::intStarted(const QXmlAttributes &atts)
{    
    ruleElementStarted(atts, QSharedPointer<Rule>(new IntRule));
}

void HighlightDefinitionHandler::floatStarted(const QXmlAttributes &atts)
{
    ruleElementStarted(atts, QSharedPointer<Rule>(new FloatRule));
}

void HighlightDefinitionHandler::hlCOctStarted(const QXmlAttributes &atts)
{    
    ruleElementStarted(atts, QSharedPointer<Rule>(new HlCOctRule));
}

void HighlightDefinitionHandler::hlCHexStarted(const QXmlAttributes &atts)
{    
    ruleElementStarted(atts, QSharedPointer<Rule>(new HlCHexRule));
}

void HighlightDefinitionHandler::hlCStringCharStarted(const QXmlAttributes &atts)
{    
    ruleElementStarted(atts, QSharedPointer<Rule>(new HlCStringCharRule));
}

void HighlightDefinitionHandler::hlCCharStarted(const QXmlAttributes &atts)
{
    ruleElementStarted(atts, QSharedPointer<Rule>(new HlCCharRule));
}

void HighlightDefinitionHandler::rangeDetectStarted(const QXmlAttributes &atts)
{
    RangeDetectRule *rule = new RangeDetectRule;
    rule->setChar(atts.value(kChar));
    rule->setChar1(atts.value(kChar1));
    ruleElementStarted(atts, QSharedPointer<Rule>(rule));
}

void HighlightDefinitionHandler::lineContinue(const QXmlAttributes &atts)
{    
    ruleElementStarted(atts, QSharedPointer<Rule>(new LineContinueRule));
}

void HighlightDefinitionHandler::includeRulesStarted(const QXmlAttributes &atts)
{
    // Include rules are treated as instructions for latter processing.
    IncludeRulesInstruction instruction(atts.value(kContext), m_currentContext->rules().size(),
                                        atts.value(kIncludeAttrib));

    // Include rules (as many others) are not allowed as a child.
    m_currentContext->addIncludeRulesInstruction(instruction);
}

void HighlightDefinitionHandler::detectSpacesStarted(const QXmlAttributes &atts)
{
    ruleElementStarted(atts, QSharedPointer<Rule>(new DetectSpacesRule));
}

void HighlightDefinitionHandler::detectIdentifier(const QXmlAttributes &atts)
{
    ruleElementStarted(atts, QSharedPointer<Rule>(new DetectIdentifierRule));
}

void HighlightDefinitionHandler::processIncludeRules() const
{
    const QHash<QString, QSharedPointer<Context> > &allContexts = m_definition->contexts();
    foreach (const QSharedPointer<Context> &context, allContexts)
        processIncludeRules(context);
}

void HighlightDefinitionHandler::processIncludeRules(const QSharedPointer<Context> &context) const
{
    if (context->includeRulesInstructions().isEmpty())
        return;

    int rulesIncluded = 0;
    const QList<IncludeRulesInstruction> &instructions = context->includeRulesInstructions();
    foreach (const IncludeRulesInstruction &instruction, instructions) {

        QSharedPointer<Context> sourceContext;
        const QString &sourceName = instruction.sourceContext();
        if (sourceName.startsWith(kDoubleHash)) {
            // This refers to an external definition. The rules included are the ones from its
            // initial context. Others contexts and rules from the external definition will work
            // transparently to the highlighter. This is because contexts and rules know the
            // definition they are from.
            QString externalName = QString::fromRawData(sourceName.unicode() + 2,
                                                        sourceName.length() - 2);
            const QString &id = Manager2::instance()->definitionIdByName(externalName);

            // If there is an incorrect circular dependency among definitions this is skipped.
            if (Manager2::instance()->isBuildingDefinition(id))
                continue;

            const QSharedPointer<HighlightDefinition> &externalDefinition =
                Manager2::instance()->definition(id);
            if (externalDefinition.isNull())
                continue;

            sourceContext = externalDefinition->initialContext();
        } else if (!sourceName.startsWith(kHash)) {
            sourceContext = m_definition->context(sourceName);

            // Recursion is done only for context direct rules. Child rules are not processed
            // because they cannot be include rules.
            processIncludeRules(sourceContext);
        } else {
            continue;
        }

        if (instruction.replaceItemData()) {
            context->setItemData(sourceContext->itemData());
            context->setDefinition(sourceContext->definition());
        }

        foreach (QSharedPointer<Rule> rule, sourceContext->rules()) {
            context->addRule(rule, instruction.indexHint() + rulesIncluded);
            ++rulesIncluded;
        }
    }
    context->clearIncludeRulesInstructions();
}
