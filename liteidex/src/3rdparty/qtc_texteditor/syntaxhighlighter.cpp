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

#include "syntaxhighlighter.h"

#include <qtextdocument.h>
#include <qtextlayout.h>
#include <qpointer.h>
#include <qtextobject.h>
#include <qtextcursor.h>
#include <qdebug.h>
#include <qtextedit.h>
#include <qtimer.h>

using namespace TextEditor;

class TextEditor::SyntaxHighlighterPrivate
{
    SyntaxHighlighter *q_ptr;
    Q_DECLARE_PUBLIC(SyntaxHighlighter)
public:
    inline SyntaxHighlighterPrivate()
        : q_ptr(0), rehighlightPending(false), inReformatBlocks(false)
    {}

    QPointer<QTextDocument> doc;

    void _q_reformatBlocks(int from, int charsRemoved, int charsAdded);
    void reformatBlocks(int from, int charsRemoved, int charsAdded);
    void reformatBlock(const QTextBlock &block, int from, int charsRemoved, int charsAdded);

    inline void rehighlight(QTextCursor &cursor, QTextCursor::MoveOperation operation) {
        inReformatBlocks = true;
        cursor.beginEditBlock();
        int from = cursor.position();
        cursor.movePosition(operation);
        reformatBlocks(from, 0, cursor.position() - from);
        cursor.endEditBlock();
        inReformatBlocks = false;
    }

    inline void _q_delayedRehighlight() {
        if (!rehighlightPending)
            return;
        rehighlightPending = false;
        q_func()->rehighlight();
    }

    void applyFormatChanges(int from, int charsRemoved, int charsAdded);
    QVector<QTextCharFormat> formatChanges;
    QTextBlock currentBlock;
    bool rehighlightPending;
    bool inReformatBlocks;
};

static bool adjustRange(QTextLayout::FormatRange &range, int from, int charsRemoved, int charsAdded) {

    if (range.start >= from) {
        range.start += charsAdded - charsRemoved;
        return true;
    } else if (range.start + range.length > from) {
        range.length += charsAdded - charsRemoved;
        return true;
    }
    return false;
}

void SyntaxHighlighterPrivate::applyFormatChanges(int from, int charsRemoved, int charsAdded)
{
    bool formatsChanged = false;

    QTextLayout *layout = currentBlock.layout();

    QList<QTextLayout::FormatRange> ranges = layout->additionalFormats();

    bool doAdjustRange = currentBlock.contains(from);

    QList<QTextLayout::FormatRange> old_ranges;

    if (!ranges.isEmpty()) {
        QList<QTextLayout::FormatRange>::Iterator it = ranges.begin();
        while (it != ranges.end()) {
            if (it->format.property(QTextFormat::UserProperty).toBool()) {
                if (doAdjustRange)
                    formatsChanged = adjustRange(*it, from - currentBlock.position(), charsRemoved, charsAdded)
                            || formatsChanged;
                ++it;
            } else {
                old_ranges.append(*it);
                it = ranges.erase(it);
            }
        }
    }

    QTextCharFormat emptyFormat;

    QTextLayout::FormatRange r;
    r.start = -1;

    QList<QTextLayout::FormatRange> new_ranges;
    int i = 0;
    while (i < formatChanges.count()) {

        while (i < formatChanges.count() && formatChanges.at(i) == emptyFormat)
            ++i;

        if (i >= formatChanges.count())
            break;

        r.start = i;
        r.format = formatChanges.at(i);

        while (i < formatChanges.count() && formatChanges.at(i) == r.format)
            ++i;

        if (i >= formatChanges.count())
            break;

        r.length = i - r.start;

        new_ranges << r;
        r.start = -1;
    }

    if (r.start != -1) {
        r.length = formatChanges.count() - r.start;

        new_ranges << r;
    }

    formatsChanged = formatsChanged || (new_ranges.size() != old_ranges.size());

    for (int i = 0; !formatsChanged && i < new_ranges.size(); ++i) {
        const QTextLayout::FormatRange &o = old_ranges.at(i);
        const QTextLayout::FormatRange &n = new_ranges.at(i);
        formatsChanged = (o.start != n.start || o.length != n.length || o.format != n.format);
    }

    if (formatsChanged) {
        ranges.append(new_ranges);
        layout->setAdditionalFormats(ranges);
        doc->markContentsDirty(currentBlock.position(), currentBlock.length());
    }
}

void SyntaxHighlighterPrivate::_q_reformatBlocks(int from, int charsRemoved, int charsAdded)
{
    if (!inReformatBlocks)
        reformatBlocks(from, charsRemoved, charsAdded);
}

void SyntaxHighlighterPrivate::reformatBlocks(int from, int charsRemoved, int charsAdded)
{
    rehighlightPending = false;

    QTextBlock block = doc->findBlock(from);
    if (!block.isValid())
        return;

    int endPosition;
    QTextBlock lastBlock = doc->findBlock(from + charsAdded + (charsRemoved > 0 ? 1 : 0));
    if (lastBlock.isValid())
        endPosition = lastBlock.position() + lastBlock.length();
    else
        endPosition =  doc->lastBlock().position() + doc->lastBlock().length(); //doc->docHandle()->length();

    bool forceHighlightOfNextBlock = false;

    while (block.isValid() && (block.position() < endPosition || forceHighlightOfNextBlock)) {
        const int stateBeforeHighlight = block.userState();

        reformatBlock(block, from, charsRemoved, charsAdded);

        forceHighlightOfNextBlock = (block.userState() != stateBeforeHighlight);

        block = block.next();
    }

    formatChanges.clear();
}

void SyntaxHighlighterPrivate::reformatBlock(const QTextBlock &block, int from, int charsRemoved, int charsAdded)
{
    Q_Q(SyntaxHighlighter);

    Q_ASSERT_X(!currentBlock.isValid(), "SyntaxHighlighter::reformatBlock()", "reFormatBlock() called recursively");

    currentBlock = block;

    formatChanges.fill(QTextCharFormat(), block.length() - 1);
    q->highlightBlock(block.text());
    applyFormatChanges(from, charsRemoved, charsAdded);

    currentBlock = QTextBlock();
}

const SyntaxHighlighter::KateFormatMap SyntaxHighlighter::m_kateFormats;

SyntaxHighlighter::KateFormatMap::KateFormatMap()
{
    m_ids.insert(QLatin1String("dsNormal"), SyntaxHighlighter::Normal);
    m_ids.insert(QLatin1String("dsKeyword"), SyntaxHighlighter::Keyword);
    m_ids.insert(QLatin1String("dsDataType"), SyntaxHighlighter::DataType);
    m_ids.insert(QLatin1String("dsDecVal"), SyntaxHighlighter::Decimal);
    m_ids.insert(QLatin1String("dsBaseN"), SyntaxHighlighter::BaseN);
    m_ids.insert(QLatin1String("dsFloat"), SyntaxHighlighter::Float);
    m_ids.insert(QLatin1String("dsChar"), SyntaxHighlighter::Char);
    m_ids.insert(QLatin1String("dsString"), SyntaxHighlighter::String);
    m_ids.insert(QLatin1String("dsComment"), SyntaxHighlighter::Comment);
    m_ids.insert(QLatin1String("dsOthers"), SyntaxHighlighter::Others);
    m_ids.insert(QLatin1String("dsAlert"), SyntaxHighlighter::Alert);
    m_ids.insert(QLatin1String("dsFunction"), SyntaxHighlighter::Function);
    m_ids.insert(QLatin1String("dsRegionMarker"), SyntaxHighlighter::RegionMarker);
    m_ids.insert(QLatin1String("dsError"), SyntaxHighlighter::Error);
    m_ids.insert(QLatin1String("dsSymbol"),SyntaxHighlighter::Symbol);
    m_ids.insert(QLatin1String("dsBuiltinFunc"), SyntaxHighlighter::BuiltinFunc);
    m_ids.insert(QLatin1String("dsPredeclared"), SyntaxHighlighter::Predeclared);
    m_ids.insert(QLatin1String("dsFuncDecl"), SyntaxHighlighter::FuncDecl);
    m_ids.insert(QLatin1String("dsPlaceholder"), SyntaxHighlighter::Placeholder);
    m_ids.insert(QLatin1String("dsToDo"), SyntaxHighlighter::ToDo);
    m_ids.insert(QLatin1String("dsPreprocessorFormat"),SyntaxHighlighter::PreprocessorFormat);
}

void SyntaxHighlighter::configureFormat(TextFormatId id, const QTextCharFormat &format)
{
    m_creatorFormats[id] = format;
}

void SyntaxHighlighter::setTabSize(int tabSize)
{
}


/*!
    \class SyntaxHighlighter
    \reentrant

    \brief The SyntaxHighlighter class allows you to define syntax
    highlighting rules, and in addition you can use the class to query
    a document's current formatting or user data.

    \since 4.1

    \ingroup richtext-processing

    The SyntaxHighlighter class is a base class for implementing
    QTextEdit syntax highlighters.  A syntax highligher automatically
    highlights parts of the text in a QTextEdit, or more generally in
    a QTextDocument. Syntax highlighters are often used when the user
    is entering text in a specific format (for example source code)
    and help the user to read the text and identify syntax errors.

    To provide your own syntax highlighting, you must subclass
    SyntaxHighlighter and reimplement highlightBlock().

    When you create an instance of your SyntaxHighlighter subclass,
    pass it the QTextEdit or QTextDocument that you want the syntax
    highlighting to be applied to. For example:

    \snippet doc/src/snippets/code/src_gui_text_SyntaxHighlighter.cpp 0

    After this your highlightBlock() function will be called
    automatically whenever necessary. Use your highlightBlock()
    function to apply formatting (e.g. setting the font and color) to
    the text that is passed to it. SyntaxHighlighter provides the
    setFormat() function which applies a given QTextCharFormat on
    the current text block. For example:

    \snippet doc/src/snippets/code/src_gui_text_SyntaxHighlighter.cpp 1

    Some syntaxes can have constructs that span several text
    blocks. For example, a C++ syntax highlighter should be able to
    cope with \c{/}\c{*...*}\c{/} multiline comments. To deal with
    these cases it is necessary to know the end state of the previous
    text block (e.g. "in comment").

    Inside your highlightBlock() implementation you can query the end
    state of the previous text block using the previousBlockState()
    function. After parsing the block you can save the last state
    using setCurrentBlockState().

    The currentBlockState() and previousBlockState() functions return
    an int value. If no state is set, the returned value is -1. You
    can designate any other value to identify any given state using
    the setCurrentBlockState() function. Once the state is set the
    QTextBlock keeps that value until it is set set again or until the
    corresponding paragraph of text is deleted.

    For example, if you're writing a simple C++ syntax highlighter,
    you might designate 1 to signify "in comment":

    \snippet doc/src/snippets/code/src_gui_text_SyntaxHighlighter.cpp 2

    In the example above, we first set the current block state to
    0. Then, if the previous block ended within a comment, we higlight
    from the beginning of the current block (\c {startIndex =
    0}). Otherwise, we search for the given start expression. If the
    specified end expression cannot be found in the text block, we
    change the current block state by calling setCurrentBlockState(),
    and make sure that the rest of the block is higlighted.

    In addition you can query the current formatting and user data
    using the format() and currentBlockUserData() functions
    respectively. You can also attach user data to the current text
    block using the setCurrentBlockUserData() function.
    QTextBlockUserData can be used to store custom settings. In the
    case of syntax highlighting, it is in particular interesting as
    cache storage for information that you may figure out while
    parsing the paragraph's text. For an example, see the
    setCurrentBlockUserData() documentation.

    \sa QTextEdit, {Syntax Highlighter Example}
*/

/*!
    Constructs a SyntaxHighlighter with the given \a parent.
*/
SyntaxHighlighter::SyntaxHighlighter(QObject *parent)
    : QObject(parent), d_ptr(new SyntaxHighlighterPrivate)
{
    d_ptr->q_ptr = this;
}

/*!
    Constructs a SyntaxHighlighter and installs it on \a parent.
    The specified QTextDocument also becomes the owner of the
    SyntaxHighlighter.
*/
SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QObject(parent), d_ptr(new SyntaxHighlighterPrivate)
{
    d_ptr->q_ptr = this;
    setDocument(parent);
}

/*!
    Constructs a SyntaxHighlighter and installs it on \a parent 's
    QTextDocument. The specified QTextEdit also becomes the owner of
    the SyntaxHighlighter.
*/
SyntaxHighlighter::SyntaxHighlighter(QTextEdit *parent)
    : QObject(parent), d_ptr(new SyntaxHighlighterPrivate)
{
    d_ptr->q_ptr = this;
    setDocument(parent->document());
}

/*!
    Destructor. Uninstalls this syntax highlighter from the text document.
*/
SyntaxHighlighter::~SyntaxHighlighter()
{
    setDocument(0);
}

/*!
    Installs the syntax highlighter on the given QTextDocument \a doc.
    A SyntaxHighlighter can only be used with one document at a time.
*/
void SyntaxHighlighter::setDocument(QTextDocument *doc)
{
    Q_D(SyntaxHighlighter);
    if (d->doc) {
        disconnect(d->doc, SIGNAL(contentsChange(int,int,int)),
                   this, SLOT(_q_reformatBlocks(int,int,int)));

        QTextCursor cursor(d->doc);
        cursor.beginEditBlock();
        for (QTextBlock blk = d->doc->begin(); blk.isValid(); blk = blk.next())
            blk.layout()->clearAdditionalFormats();
        cursor.endEditBlock();
    }
    d->doc = doc;
    if (d->doc) {
        connect(d->doc, SIGNAL(contentsChange(int,int,int)),
                this, SLOT(_q_reformatBlocks(int,int,int)));
        d->rehighlightPending = true;
        QTimer::singleShot(0, this, SLOT(_q_delayedRehighlight()));
    }
}

/*!
    Returns the QTextDocument on which this syntax highlighter is
    installed.
*/
QTextDocument *SyntaxHighlighter::document() const
{
    Q_D(const SyntaxHighlighter);
    return d->doc;
}

/*!
    \since 4.2

    Reapplies the highlighting to the whole document.

    \sa rehighlightBlock()
*/
void SyntaxHighlighter::rehighlight()
{
    Q_D(SyntaxHighlighter);
    if (!d->doc)
        return;

    QTextCursor cursor(d->doc);
    d->rehighlight(cursor, QTextCursor::End);
}

/*!
    \since 4.6

    Reapplies the highlighting to the given QTextBlock \a block.

    \sa rehighlight()
*/
void SyntaxHighlighter::rehighlightBlock(const QTextBlock &block)
{
    Q_D(SyntaxHighlighter);
    if (!d->doc || !block.isValid() || block.document() != d->doc)
        return;

    const bool rehighlightPending = d->rehighlightPending;

    QTextCursor cursor(block);
    d->rehighlight(cursor, QTextCursor::EndOfBlock);

    if (rehighlightPending)
        d->rehighlightPending = rehighlightPending;
}

/*!
    \fn void SyntaxHighlighter::highlightBlock(const QString &text)

    Highlights the given text block. This function is called when
    necessary by the rich text engine, i.e. on text blocks which have
    changed.

    To provide your own syntax highlighting, you must subclass
    SyntaxHighlighter and reimplement highlightBlock(). In your
    reimplementation you should parse the block's \a text and call
    setFormat() as often as necessary to apply any font and color
    changes that you require. For example:

    \snippet doc/src/snippets/code/src_gui_text_SyntaxHighlighter.cpp 3

    Some syntaxes can have constructs that span several text
    blocks. For example, a C++ syntax highlighter should be able to
    cope with \c{/}\c{*...*}\c{/} multiline comments. To deal with
    these cases it is necessary to know the end state of the previous
    text block (e.g. "in comment").

    Inside your highlightBlock() implementation you can query the end
    state of the previous text block using the previousBlockState()
    function. After parsing the block you can save the last state
    using setCurrentBlockState().

    The currentBlockState() and previousBlockState() functions return
    an int value. If no state is set, the returned value is -1. You
    can designate any other value to identify any given state using
    the setCurrentBlockState() function. Once the state is set the
    QTextBlock keeps that value until it is set set again or until the
    corresponding paragraph of text gets deleted.

    For example, if you're writing a simple C++ syntax highlighter,
    you might designate 1 to signify "in comment". For a text block
    that ended in the middle of a comment you'd set 1 using
    setCurrentBlockState, and for other paragraphs you'd set 0.
    In your parsing code if the return value of previousBlockState()
    is 1, you would highlight the text as a C++ comment until you
    reached the closing \c{*}\c{/}.

    \sa previousBlockState(), setFormat(), setCurrentBlockState()
*/

/*!
    This function is applied to the syntax highlighter's current text
    block (i.e. the text that is passed to the highlightBlock()
    function).

    The specified \a format is applied to the text from the \a start
    position for a length of \a count characters (if \a count is 0,
    nothing is done). The formatting properties set in \a format are
    merged at display time with the formatting information stored
    directly in the document, for example as previously set with
    QTextCursor's functions. Note that the document itself remains
    unmodified by the format set through this function.

    \sa format(), highlightBlock()
*/
void SyntaxHighlighter::setFormat(int start, int count, const QTextCharFormat &format)
{
    Q_D(SyntaxHighlighter);
    if (start < 0 || start >= d->formatChanges.count())
        return;

    const int end = qMin(start + count, d->formatChanges.count());
    for (int i = start; i < end; ++i)
        d->formatChanges[i] = format;
}

/*!
    \overload

    The specified \a color is applied to the current text block from
    the \a start position for a length of \a count characters.

    The other attributes of the current text block, e.g. the font and
    background color, are reset to default values.

    \sa format(), highlightBlock()
*/
void SyntaxHighlighter::setFormat(int start, int count, const QColor &color)
{
    QTextCharFormat format;
    format.setForeground(color);
    setFormat(start, count, format);
}

/*!
    \overload

    The specified \a font is applied to the current text block from
    the \a start position for a length of \a count characters.

    The other attributes of the current text block, e.g. the font and
    background color, are reset to default values.

    \sa format(), highlightBlock()
*/
void SyntaxHighlighter::setFormat(int start, int count, const QFont &font)
{
    QTextCharFormat format;
    format.setFont(font);
    setFormat(start, count, format);
}

void SyntaxHighlighter::applyFormatToSpaces(const QString &text, const QTextCharFormat &format)
{
    int offset = 0;
    const int length = text.length();
    while (offset < length) {
        if (text.at(offset).isSpace()) {
            int start = offset++;
            while (offset < length && text.at(offset).isSpace())
                ++offset;
            setFormat(start, offset - start, format);
        } else {
            ++offset;
        }
    }
}

/*!
    \fn QTextCharFormat SyntaxHighlighter::format(int position) const

    Returns the format at \a position inside the syntax highlighter's
    current text block.
*/
QTextCharFormat SyntaxHighlighter::format(int pos) const
{
    Q_D(const SyntaxHighlighter);
    if (pos < 0 || pos >= d->formatChanges.count())
        return QTextCharFormat();
    return d->formatChanges.at(pos);
}

/*!
    Returns the end state of the text block previous to the
    syntax highlighter's current block. If no value was
    previously set, the returned value is -1.

    \sa highlightBlock(), setCurrentBlockState()
*/
int SyntaxHighlighter::previousBlockState() const
{
    Q_D(const SyntaxHighlighter);
    if (!d->currentBlock.isValid())
        return -1;

    const QTextBlock previous = d->currentBlock.previous();
    if (!previous.isValid())
        return -1;

    return previous.userState();
}

/*!
    Returns the state of the current text block. If no value is set,
    the returned value is -1.
*/
int SyntaxHighlighter::currentBlockState() const
{
    Q_D(const SyntaxHighlighter);
    if (!d->currentBlock.isValid())
        return -1;

    return d->currentBlock.userState();
}

/*!
    Sets the state of the current text block to \a newState.

    \sa highlightBlock()
*/
void SyntaxHighlighter::setCurrentBlockState(int newState)
{
    Q_D(SyntaxHighlighter);
    if (!d->currentBlock.isValid())
        return;

    d->currentBlock.setUserState(newState);
}

/*!
    Attaches the given \a data to the current text block.  The
    ownership is passed to the underlying text document, i.e. the
    provided QTextBlockUserData object will be deleted if the
    corresponding text block gets deleted.

    QTextBlockUserData can be used to store custom settings. In the
    case of syntax highlighting, it is in particular interesting as
    cache storage for information that you may figure out while
    parsing the paragraph's text.

    For example while parsing the text, you can keep track of
    parenthesis characters that you encounter ('{[(' and the like),
    and store their relative position and the actual QChar in a simple
    class derived from QTextBlockUserData:

    \snippet doc/src/snippets/code/src_gui_text_SyntaxHighlighter.cpp 4

    During cursor navigation in the associated editor, you can ask the
    current QTextBlock (retrieved using the QTextCursor::block()
    function) if it has a user data object set and cast it to your \c
    BlockData object. Then you can check if the current cursor
    position matches with a previously recorded parenthesis position,
    and, depending on the type of parenthesis (opening or closing),
    find the next opening or closing parenthesis on the same level.

    In this way you can do a visual parenthesis matching and highlight
    from the current cursor position to the matching parenthesis. That
    makes it easier to spot a missing parenthesis in your code and to
    find where a corresponding opening/closing parenthesis is when
    editing parenthesis intensive code.

    \sa QTextBlock::setUserData()
*/
void SyntaxHighlighter::setCurrentBlockUserData(QTextBlockUserData *data)
{
    Q_D(SyntaxHighlighter);
    if (!d->currentBlock.isValid())
        return;

    d->currentBlock.setUserData(data);
}

/*!
    Returns the QTextBlockUserData object previously attached to the
    current text block.

    \sa QTextBlock::userData(), setCurrentBlockUserData()
*/
QTextBlockUserData *SyntaxHighlighter::currentBlockUserData() const
{
    Q_D(const SyntaxHighlighter);
    if (!d->currentBlock.isValid())
        return 0;

    return d->currentBlock.userData();
}

/*!
    \since 4.4

    Returns the current text block.
*/
QTextBlock SyntaxHighlighter::currentBlock() const
{
    Q_D(const SyntaxHighlighter);
    return d->currentBlock;
}

static bool byStartOfRange(const QTextLayout::FormatRange &range, const QTextLayout::FormatRange &other)
{
    return range.start < other.start;
}

void SyntaxHighlighter::setExtraAdditionalFormats(const QTextBlock& block,
                                                  const QList<QTextLayout::FormatRange> &fmts)
{

//    qDebug() << "setAdditionalFormats() on block" << block.blockNumber();
//    for (int i = 0; i < overrides.count(); ++i)
//        qDebug() << "   from " << overrides.at(i).start << "length"
//                 << overrides.at(i).length
//                 << "color:" << overrides.at(i).format.foreground().color();
    Q_D(SyntaxHighlighter);

    if (block.layout() == 0)
        return;

    QList<QTextLayout::FormatRange> formats;
    formats.reserve(fmts.size());
    foreach (QTextLayout::FormatRange r, fmts) {
        r.format.setProperty(QTextFormat::UserProperty, true);
        formats.append(r);
    }
    qSort(formats.begin(), formats.end(), byStartOfRange);

    QList<QTextLayout::FormatRange> previousSemanticFormats;
    QList<QTextLayout::FormatRange> formatsToApply;

    const QList<QTextLayout::FormatRange> all = block.layout()->additionalFormats();
    foreach (const QTextLayout::FormatRange &r, all) {
        if (r.format.hasProperty(QTextFormat::UserProperty))
            previousSemanticFormats.append(r);
        else
            formatsToApply.append(r);
    }

    if (formats.size() == previousSemanticFormats.size()) {
        qSort(previousSemanticFormats.begin(), previousSemanticFormats.end(), byStartOfRange);

        int index = 0;
        for (; index != formats.size(); ++index) {
            const QTextLayout::FormatRange &range = formats.at(index);
            const QTextLayout::FormatRange &previousRange = previousSemanticFormats.at(index);

            if (range.start != previousRange.start ||
                    range.length != previousRange.length ||
                    range.format != previousRange.format)
                break;
        }

        if (index == formats.size())
            return;
    }

    formatsToApply += formats;

    bool wasInReformatBlocks = d->inReformatBlocks;
    d->inReformatBlocks = true;
    block.layout()->setAdditionalFormats(formatsToApply);
    document()->markContentsDirty(block.position(), block.length()-1);
    d->inReformatBlocks = wasInReformatBlocks;
}

#include "moc_syntaxhighlighter.cpp"
