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

#include "highlighter.h"
#include "highlightdefinition.h"
#include "context.h"
#include "rule.h"
#include "itemdata.h"
#include "highlighterexception.h"
#include "progressdata.h"
#include "reuse.h"
#include "tabsettings.h"

#include <QtCore/QLatin1String>
#include <QtCore/QLatin1Char>
#include <QDebug>

using namespace TextEditor;
using namespace Internal;

namespace {
    static const QLatin1String kStay("#stay");
    static const QLatin1String kPop("#pop");
    static const QLatin1Char kBackSlash('\\');
    static const QLatin1Char kHash('#');
}

const Highlighter::KateFormatMap Highlighter::m_kateFormats;

Highlighter::Highlighter(QTextDocument *parent) :
    TextEditor::SyntaxHighlighter(parent),
    m_regionDepth(0),
    m_lastRegionDepth(0),
    m_indentationBasedFolding(false),
    m_tabSettings(0),
    m_persistentObservableStatesCounter(PersistentsStart),
    m_dynamicContextsCounter(0),
    m_isBroken(false),
    m_stringOrComment(false)
{}

Highlighter::~Highlighter()
{}

Highlighter::BlockData::BlockData() : m_foldingIndentDelta(0), m_originalObservableState(-1)
{}

Highlighter::BlockData::~BlockData()
{}

Highlighter::KateFormatMap::KateFormatMap()
{
    m_ids.insert(QLatin1String("dsNormal"), Highlighter::Normal);
    m_ids.insert(QLatin1String("dsKeyword"), Highlighter::Keyword);
    m_ids.insert(QLatin1String("dsDataType"), Highlighter::DataType);
    m_ids.insert(QLatin1String("dsDecVal"), Highlighter::Decimal);
    m_ids.insert(QLatin1String("dsBaseN"), Highlighter::BaseN);
    m_ids.insert(QLatin1String("dsFloat"), Highlighter::Float);
    m_ids.insert(QLatin1String("dsChar"), Highlighter::Char);
    m_ids.insert(QLatin1String("dsString"), Highlighter::String);
    m_ids.insert(QLatin1String("dsComment"), Highlighter::Comment);
    m_ids.insert(QLatin1String("dsOthers"), Highlighter::Others);
    m_ids.insert(QLatin1String("dsAlert"), Highlighter::Alert);
    m_ids.insert(QLatin1String("dsFunction"), Highlighter::Function);
    m_ids.insert(QLatin1String("dsRegionMarker"), Highlighter::RegionMarker);
    m_ids.insert(QLatin1String("dsError"), Highlighter::Error);
    m_ids.insert(QLatin1String("esBuiltinFunc"), Highlighter::BuiltinFunc);
    m_ids.insert(QLatin1String("esPredeclared"), Highlighter::Predeclared);
    m_ids.insert(QLatin1String("esFuncDecl"), Highlighter::FuncDecl);
}

void Highlighter::configureFormat(TextFormatId id, const QTextCharFormat &format)
{
    m_creatorFormats[id] = format;
}

void  Highlighter::setDefaultContext(const QSharedPointer<Context> &defaultContext)
{
    m_defaultContext = defaultContext;
    m_persistentObservableStates.insert(m_defaultContext->name(), Default);
    m_indentationBasedFolding = defaultContext->definition()->isIndentationBasedFolding();
}

void Highlighter::setTabSettings(const TabSettings &ts)
{
    m_tabSettings = &ts;
}

void Highlighter::highlightBlock(const QString &text)
{
    if (!m_defaultContext.isNull() && !m_isBroken) {
        try {
            if (!currentBlockUserData())
                initializeBlockData();
            setupDataForBlock(text);

            handleContextChange(m_currentContext->lineBeginContext(),
                                m_currentContext->definition());

            ProgressData progress;
            const int length = text.length();
            while (progress.offset() < length)
                iterateThroughRules(text, length, &progress, false, m_currentContext->rules());

            handleContextChange(m_currentContext->lineEndContext(),
                                m_currentContext->definition(),
                                false);
            m_contexts.clear();

            if (m_indentationBasedFolding) {
                applyIndentationBasedFolding(text);
            } else {
                applyRegionBasedFolding();

                // In the case region depth has changed since the last time the state was set.
                setCurrentBlockState(computeState(extractObservableState(currentBlockState())));
            }
        } catch (const HighlighterException &) {
            m_isBroken = true;
        }
    }

    applyFormatToSpaces(text, m_creatorFormats.value(VisualWhitespace));
}

void Highlighter::setupDataForBlock(const QString &text)
{
    if (extractObservableState(currentBlockState()) == WillContinue)
        analyseConsistencyOfWillContinueBlock(text);

    if (previousBlockState() == -1) {
        m_regionDepth = 0;
        m_lastRegionDepth = 0;
        setupDefault();
    } else {
        m_regionDepth = extractRegionDepth(previousBlockState());
        m_lastRegionDepth = m_regionDepth;
        const int observablePreviousState = extractObservableState(previousBlockState());
        if (observablePreviousState == Default)
            setupDefault();
        else if (observablePreviousState == WillContinue)
            setupFromWillContinue();
        else if (observablePreviousState == Continued)
            setupFromContinued();
        else
            setupFromPersistent();

        blockData(currentBlockUserData())->m_foldingRegions =
            blockData(currentBlock().previous().userData())->m_foldingRegions;
        blockData(currentBlockUserData())->clearParentheses();
    }

    assignCurrentContext();
}

void Highlighter::setupDefault()
{
    m_contexts.push_back(m_defaultContext);

    setCurrentBlockState(computeState(Default));
}

void Highlighter::setupFromWillContinue()
{
    BlockData *previousData = blockData(currentBlock().previous().userData());
    m_contexts.push_back(previousData->m_contextToContinue);

    BlockData *data = blockData(currentBlock().userData());
    data->m_originalObservableState = previousData->m_originalObservableState;

    if (currentBlockState() == -1 || extractObservableState(currentBlockState()) == Default)
        setCurrentBlockState(computeState(Continued));
}

void Highlighter::setupFromContinued()
{
    BlockData *previousData = blockData(currentBlock().previous().userData());

    Q_ASSERT(previousData->m_originalObservableState != WillContinue &&
             previousData->m_originalObservableState != Continued);

    if (previousData->m_originalObservableState == Default ||
        previousData->m_originalObservableState == -1) {
        m_contexts.push_back(m_defaultContext);
    } else {
        pushContextSequence(previousData->m_originalObservableState);
    }

    setCurrentBlockState(computeState(previousData->m_originalObservableState));
}

void Highlighter::setupFromPersistent()
{
    pushContextSequence(extractObservableState(previousBlockState()));

    setCurrentBlockState(previousBlockState());
}

void Highlighter::iterateThroughRules(const QString &text,
                                      const int length,
                                      ProgressData *progress,
                                      const bool childRule,
                                      const QList<QSharedPointer<Rule> > &rules)
{
    typedef QList<QSharedPointer<Rule> >::const_iterator RuleIterator;

    bool contextChanged = false;
    bool atLeastOneMatch = false;

    RuleIterator it = rules.begin();
    RuleIterator endIt = rules.end();
    bool detlaDeptn = false;
    while (it != endIt && progress->offset() < length) {
        int startOffset = progress->offset();
        const QSharedPointer<Rule> &rule = *it;

        if (rule->itemData() == "String" ||
                rule->itemData() == "Comment") {
            m_stringOrComment = true;
        } else {
            m_stringOrComment = false;
        }

        if (rule->matchSucceed(text, length, progress)) {
            atLeastOneMatch = true;
            if (!m_stringOrComment) {
                if (!rule->beginRegion().isEmpty()) {
                    QChar ch = text.at(startOffset);
                    if (ch == '{' || ch == '(' || ch == '[' ) {
                        blockData(currentBlockUserData())->appendParenthese(Parenthesis(Parenthesis::Opened,ch,startOffset));
                    }
                }
                if (!rule->endRegion().isEmpty()) {
                    QChar ch = text.at(startOffset);
                    if (ch == '}' || ch == ')' || ch == ']' ) {
                        blockData(currentBlockUserData())->appendParenthese(Parenthesis(Parenthesis::Closed,ch,startOffset));
                    }
                }
            }

            if (!m_indentationBasedFolding) {
                if (!rule->endRegion().isEmpty()) {
                    QStack<QString> *currentRegions =
                        &blockData(currentBlockUserData())->m_foldingRegions;
                    if (!currentRegions->isEmpty() && rule->endRegion() == currentRegions->top()) {
                        currentRegions->pop();
                        --m_regionDepth;
                        if (m_lastRegionDepth > m_regionDepth) {
                            detlaDeptn = true;
                        }
                        if (!m_stringOrComment && progress->isClosingBraceMatchAtNonEnd()) {
                            --blockData(currentBlockUserData())->m_foldingIndentDelta;
                        }
                    }
                }
                if (!rule->beginRegion().isEmpty()) {
                    blockData(currentBlockUserData())->m_foldingRegions.push(rule->beginRegion());
                    ++m_regionDepth;                    
                    if (!m_stringOrComment && progress->isOpeningBraceMatchAtFirstNonSpace()) {
                        ++blockData(currentBlockUserData())->m_foldingIndentDelta;
                    }
                }
                progress->clearBracesMatches();
            }

            if (progress->isWillContinueLine()) {
                createWillContinueBlock();
                progress->setWillContinueLine(false);
            } else {
                if (rule->hasChildren())
                    iterateThroughRules(text, length, progress, true, rule->children());

                if (!rule->context().isEmpty() && contextChangeRequired(rule->context())) {
                    m_currentCaptures = progress->captures();
                    changeContext(rule->context(), rule->definition());
                    contextChanged = true;
                }
            }

            // Format is not applied to child rules directly (but relative to the offset of their
            // parent) nor to look ahead rules.
            if (!childRule && !rule->isLookAhead()) {
                if (rule->itemData().isEmpty())
                    applyFormat(startOffset, progress->offset() - startOffset,
                                m_currentContext->itemData(), m_currentContext->definition());
                else
                    applyFormat(startOffset, progress->offset() - startOffset, rule->itemData(),
                                rule->definition());
            }

            // When there is a match of one child rule the others should be skipped. Otherwise
            // the highlighting would be incorret in a case like 9ULLLULLLUULLULLUL, for example.
            if (contextChanged || childRule) {
                break;
            } else {
                it = rules.begin();
                continue;
            }
        }
        ++it;
    }

    if (!childRule && !atLeastOneMatch) {
        if (m_currentContext->isFallthrough()) {
            handleContextChange(m_currentContext->fallthroughContext(),
                                m_currentContext->definition());
            iterateThroughRules(text, length, progress, false, m_currentContext->rules());
        } else {
            applyFormat(progress->offset(), 1, m_currentContext->itemData(),
                        m_currentContext->definition());
            if (progress->isOnlySpacesSoFar() && !text.at(progress->offset()).isSpace())
                progress->setOnlySpacesSoFar(false);
            progress->incrementOffset();
        }
    }
    if (detlaDeptn && m_lastRegionDepth == m_regionDepth) {
        blockData(currentBlockUserData())->m_foldingIndentDelta--;
    }
}

bool Highlighter::contextChangeRequired(const QString &contextName) const
{
    if (contextName == kStay)
        return false;
    return true;
}

void Highlighter::changeContext(const QString &contextName,
                                const QSharedPointer<HighlightDefinition> &definition,
                                const bool assignCurrent)
{
    if (contextName.startsWith(kPop)) {
        QStringList list = contextName.split(kHash, QString::SkipEmptyParts);
        for (int i = 0; i < list.size(); ++i)
            m_contexts.pop_back();

        if (extractObservableState(currentBlockState()) >= PersistentsStart) {
            // One or more contexts were popped during during a persistent state.
            const QString &currentSequence = currentContextSequence();
            if (m_persistentObservableStates.contains(currentSequence))
                setCurrentBlockState(
                    computeState(m_persistentObservableStates.value(currentSequence)));
            else
                setCurrentBlockState(
                    computeState(m_leadingObservableStates.value(currentSequence)));
        }
    } else {
        const QSharedPointer<Context> &context = definition->context(contextName);

        if (context->isDynamic())
            pushDynamicContext(context);
        else
            m_contexts.push_back(context);

        if (m_contexts.back()->lineEndContext() == kStay ||
            extractObservableState(currentBlockState()) >= PersistentsStart) {
            const QString &currentSequence = currentContextSequence();
            mapLeadingSequence(currentSequence);
            if (m_contexts.back()->lineEndContext() == kStay) {
                // A persistent context was pushed.
                mapPersistentSequence(currentSequence);
                setCurrentBlockState(
                    computeState(m_persistentObservableStates.value(currentSequence)));
            }
        }
    }

    if (assignCurrent)
        assignCurrentContext();
}

void Highlighter::handleContextChange(const QString &contextName,
                                      const QSharedPointer<HighlightDefinition> &definition,
                                      const bool setCurrent)
{
    if (!contextName.isEmpty() && contextChangeRequired(contextName))
        changeContext(contextName, definition, setCurrent);
}

void Highlighter::applyFormat(int offset,
                              int count,
                              const QString &itemDataName,
                              const QSharedPointer<HighlightDefinition> &definition)
{
    if (count == 0)
        return;

    QSharedPointer<ItemData> itemData;
    try {
        itemData = definition->itemData(itemDataName);
    } catch (const HighlighterException &) {
        // There are some broken files. For instance, the Printf context in java.xml points to an
        // inexistent Printf item data. These cases are considered to have normal text style.
        return;
    }

    TextFormatId formatId = m_kateFormats.m_ids.value(itemData->style());
    if (formatId != Normal) {
        QHash<TextFormatId, QTextCharFormat>::const_iterator cit =
            m_creatorFormats.constFind(formatId);
        if (cit != m_creatorFormats.constEnd()) {
            QTextCharFormat format = cit.value();
            if (itemData->isCustomized()) {
                // Please notice that the following are applied every time for item datas which have
                // customizations. The configureFormats method could be used to provide a "one time"
                // configuration, but it would probably require to traverse all item datas from all
                // definitions available/loaded (either to set the values or for some "notifying"
                // strategy). This is because the highlighter does not really know on which
                // definition(s) it is working. Since not many item datas specify customizations I
                // think this approach would fit better. If there are other ideas...
                if (itemData->color().isValid())
                    format.setForeground(itemData->color());
                if (itemData->isItalicSpecified())
                    format.setFontItalic(itemData->isItalic());
                if (itemData->isBoldSpecified())
                    format.setFontWeight(toFontWeight(itemData->isBold()));
                if (itemData->isUnderlinedSpecified())
                    format.setFontUnderline(itemData->isUnderlined());
                if (itemData->isStrikeOutSpecified())
                    format.setFontStrikeOut(itemData->isStrikeOut());
            }

            setFormat(offset, count, format);
        }
    }
}

void Highlighter::createWillContinueBlock()
{
    BlockData *data = blockData(currentBlockUserData());
    const int currentObservableState = extractObservableState(currentBlockState());
    if (currentObservableState == Continued) {
        BlockData *previousData = blockData(currentBlock().previous().userData());
        data->m_originalObservableState = previousData->m_originalObservableState;
    } else if (currentObservableState != WillContinue) {
        data->m_originalObservableState = currentObservableState;
    }
    data->m_contextToContinue = m_currentContext;

    setCurrentBlockState(computeState(WillContinue));
}

void Highlighter::analyseConsistencyOfWillContinueBlock(const QString &text)
{
    if (currentBlock().next().isValid() && (
        text.length() == 0 || text.at(text.length() - 1) != kBackSlash) &&
        extractObservableState(currentBlock().next().userState()) != Continued) {
        currentBlock().next().setUserState(computeState(Continued));
    }

    if (text.length() == 0 || text.at(text.length() - 1) != kBackSlash) {
        BlockData *data = blockData(currentBlockUserData());
        data->m_contextToContinue.clear();
        setCurrentBlockState(computeState(data->m_originalObservableState));
    }
}

void Highlighter::mapPersistentSequence(const QString &contextSequence)
{
    if (!m_persistentObservableStates.contains(contextSequence)) {
        int newState = m_persistentObservableStatesCounter;
        m_persistentObservableStates.insert(contextSequence, newState);
        m_persistentContexts.insert(newState, m_contexts);
        ++m_persistentObservableStatesCounter;
    }
}

void Highlighter::mapLeadingSequence(const QString &contextSequence)
{
    if (!m_leadingObservableStates.contains(contextSequence))
        m_leadingObservableStates.insert(contextSequence,
                                         extractObservableState(currentBlockState()));
}

void Highlighter::pushContextSequence(int state)
{
    const QVector<QSharedPointer<Context> > &contexts = m_persistentContexts.value(state);
    for (int i = 0; i < contexts.size(); ++i)
        m_contexts.push_back(contexts.at(i));
}

QString Highlighter::currentContextSequence() const
{
    QString sequence;
    for (int i = 0; i < m_contexts.size(); ++i)
        sequence.append(m_contexts.at(i)->id());

    return sequence;
}

Highlighter::BlockData *Highlighter::initializeBlockData()
{
    BlockData *data = new BlockData;
    setCurrentBlockUserData(data);
    return data;
}

Highlighter::BlockData *Highlighter::blockData(QTextBlockUserData *userData)
{
    return static_cast<BlockData *>(userData);
}

void Highlighter::pushDynamicContext(const QSharedPointer<Context> &baseContext)
{
    // A dynamic context is created from another context which serves as its basis. Then,
    // its rules are updated according to the captures from the calling regular expression which
    // triggered the push of the dynamic context.
    QSharedPointer<Context> context(new Context(*baseContext));
    context->configureId(m_dynamicContextsCounter);
    context->updateDynamicRules(m_currentCaptures);
    m_contexts.push_back(context);
    ++m_dynamicContextsCounter;
}

void Highlighter::assignCurrentContext()
{
    if (m_contexts.isEmpty()) {
        // This is not supposed to happen. However, there are broken files (for example, php.xml)
        // which will cause this behaviour. In such cases pushing the default context is enough to
        // keep highlighter working.
        m_contexts.push_back(m_defaultContext);
    }
    m_currentContext = m_contexts.back();
}

int Highlighter::extractRegionDepth(const int state)
{
    return state >> 12;
}

int Highlighter::extractObservableState(const int state)
{
    return state & 0xFFF;
}

int Highlighter::computeState(const int observableState) const
{
    return m_regionDepth << 12 | observableState;
}

void Highlighter::setFoldIndent(BlockData *data, int indent, const QTextBlock &block)
{
    if (data->foldingIndent() != indent) {
        emit foldIndentChanged(block);
    }
    data->setFoldingIndent(indent);
}

void Highlighter::applyRegionBasedFolding()
{
    int folding = 0;
    BlockData *data = blockData(currentBlockUserData());
    BlockData *previousData = blockData(currentBlock().previous().userData());
    if (previousData) {
        folding = extractRegionDepth(previousBlockState());
        if (data->m_foldingIndentDelta != 0) {
            folding += data->m_foldingIndentDelta;
            if (data->m_foldingIndentDelta > 0)
                data->setFoldingStartIncluded(true);
            else
                previousData->setFoldingEndIncluded(false);
            data->m_foldingIndentDelta = 0;
        }
    }
    data->setFoldingEndIncluded(true);
    //data->setFoldingIndent(folding);
    setFoldIndent(data,folding,currentBlock());
}

void Highlighter::applyIndentationBasedFolding(const QString &text)
{
    BlockData *data = blockData(currentBlockUserData());
    data->setFoldingEndIncluded(true);

    // If this line is empty, check its neighbours. They all might be part of the same block.
    if (text.trimmed().isEmpty()) {
        //data->setFoldingIndent(0);
        setFoldIndent(data,0,currentBlock());
        const int previousIndent = neighbouringNonEmptyBlockIndent(currentBlock().previous(), true);
        if (previousIndent > 0) {
            const int nextIndent = neighbouringNonEmptyBlockIndent(currentBlock().next(), false);
            if (previousIndent == nextIndent) {
                //data->setFoldingIndent(previousIndent);
                setFoldIndent(data,0,currentBlock());
            }
        }
    } else {
        //data->setFoldingIndent(m_tabSettings->indentationColumn(text));
        setFoldIndent(data,m_tabSettings->indentationColumn(text),currentBlock());
    }
}

int Highlighter::neighbouringNonEmptyBlockIndent(QTextBlock block, const bool previous) const
{
    while (true) {
        if (!block.isValid())
            return 0;
        if (block.text().trimmed().isEmpty()) {
            if (previous)
                block = block.previous();
            else
                block = block.next();
        } else {
            return m_tabSettings->indentationColumn(block.text());
        }
    }
}
