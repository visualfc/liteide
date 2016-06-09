/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: liteeditorwidgetbase.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteeditorwidgetbase.h"
#include "qtc_texteditor/basetextdocumentlayout.h"
#include <QCoreApplication>
#include <QApplication>
#include <QTextBlock>
#include <QPainter>
#include <QStyle>
#include <QDebug>
#include <QMessageBox>
#include <QToolTip>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QScrollBar>
#include <QInputMethodEvent>
#include <QTimer>
#include <cmath>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

const int PRIORITYLIST_LENGTH = 5;
const LiteApi::EditorNaviagteType MARKTYPE_PRIORITYLIST[PRIORITYLIST_LENGTH] = {
        LiteApi::EditorNavigateError, LiteApi::EditorNavigateBad, LiteApi::EditorNavigateWarning, LiteApi::EditorNavigateReload, LiteApi::EditorNavigateNormal
    };

inline QColor markTypeColor(LiteApi::EditorNaviagteType type) {
    switch(type) {
    case LiteApi::EditorNavigateBad:
    case LiteApi::EditorNavigateError:
        return Qt::darkRed;
    case LiteApi::EditorNavigateWarning:
        //Returns orange color
        return QColor::fromRgb(255, 125, 0);
    case LiteApi::EditorNavigateNormal:
        return Qt::darkGreen;
    case LiteApi::EditorNavigateReload:
        return Qt::darkBlue;
    }
    return Qt::darkGreen;
}

struct NavigateMark
{
    NavigateMark() {}
    ~NavigateMark()
    {
        foreach (Node *node, m_nodeList) {
            delete node;
        }
        m_nodeList.clear();
    }    

    struct Node {
        LiteApi::EditorNaviagteType type;
        QString msg;
        QString tag;

        QColor markColor() {
            return markTypeColor(type);
        }
    };
    void addNode(LiteApi::EditorNaviagteType type, const QString & msg, const char* tag = "")
    {
        Node *node = new Node;
        node->type = type;
        node->msg = msg;
        node->tag = QString(tag);
        m_nodeList.append(node);
    }
    Node* findNode(LiteApi::EditorNaviagteType type)
    {
        foreach (Node *node, m_nodeList) {
            if (node->type == type) {
                return node;
            }
        }
        return 0;
    }
    void removeNode(LiteApi::EditorNaviagteType types, const char* tag = "")
    {
        QMutableListIterator<Node*> i(m_nodeList);
        while (i.hasNext()) {
            Node *node = i.next();
            if ((node->type&types) && (QString(tag).isEmpty() || QString(tag) == node->tag)) {
                i.remove();
                delete node;
            }
        }
    }
    bool isEmpty() const {
        return m_nodeList.isEmpty();
    }

    QList<Node*> m_nodeList;
};

typedef QMap<int,NavigateMark*> NavigateMarkMap;

class NavigateManager : public QObject
{
public:
    NavigateManager(QObject *parent) : QObject(parent)
    {
        m_type = LiteApi::EditorNavigateNormal;
    }
    ~NavigateManager()
    {
        clearAll();
    }
    void insertMark(int line, const QString &msg, LiteApi::EditorNaviagteType type, const char* tag = "")
    {
         NavigateMarkMap::iterator it = markMap.find(line);
         if (it == markMap.end()) {
             NavigateMark *mark = new NavigateMark;
             mark->addNode(type, msg, tag);
             markMap.insert(line,mark);
         } else {
             NavigateMark *mark = it.value();
             NavigateMark::Node *node = mark->findNode(type);
             if (node) {
                 node->msg = msg;
             } else {
                 mark->addNode(type,msg);
             }
         }
    }
    void clearAllNavigateMark(LiteApi::EditorNaviagteType types, const char* tag = "")
    {
        QMutableMapIterator<int,NavigateMark*> i(markMap);
        while (i.hasNext()) {
            i.next();
            NavigateMark *mark = i.value();
            mark->removeNode(types, tag);
            if (mark->isEmpty()) {
                delete mark;
                i.remove();
            }
        }
    }
    void clearAll()
    {
        foreach(NavigateMark *mark,markMap.values()) {
            delete mark;
        }
        markMap.clear();
    }
    void setType(LiteApi::EditorNaviagteType type, const QString &msg)
    {
        m_type = type;
        m_msg = msg;
    }
    QColor headMarkColor()
    {
        return markTypeColor(m_type);
    }
public:
    NavigateMarkMap markMap;
    LiteApi::EditorNaviagteType m_type;
    QString m_msg;
};

class TextEditExtraArea : public QWidget {
public:
    TextEditExtraArea(LiteEditorWidgetBase *edit):QWidget(edit) {
        textEdit = edit;
        setAutoFillBackground(true);
    }
public:

    QSize sizeHint() const {
        return QSize(textEdit->extraAreaWidth(), 0);
    }
protected:
    void paintEvent(QPaintEvent *event){
        textEdit->extraAreaPaintEvent(event);
    }
    void mousePressEvent(QMouseEvent *event){
        textEdit->extraAreaMouseEvent(event);
    }
    void mouseMoveEvent(QMouseEvent *event){
        textEdit->extraAreaMouseEvent(event);
    }
    void mouseReleaseEvent(QMouseEvent *event){
        textEdit->extraAreaMouseEvent(event);
    }
    void leaveEvent(QEvent *event){
        textEdit->extraAreaLeaveEvent(event);
    }

    void wheelEvent(QWheelEvent *event) {
        QCoreApplication::sendEvent(textEdit->viewport(), event);
    }
protected:
    LiteEditorWidgetBase *textEdit;
};

class TextEditNavigateArea : public QWidget {
public:
    TextEditNavigateArea(LiteEditorWidgetBase *edit):QWidget(edit) {
        textEdit = edit;
        setAutoFillBackground(true);
    }
public:

    QSize sizeHint() const {
        return QSize(textEdit->navigateAreaWidth(), 0);
    }
protected:
    void paintEvent(QPaintEvent *event){
        textEdit->navigateAreaPaintEvent(event);
    }
    void mousePressEvent(QMouseEvent *event){
        textEdit->navigateAreaMouseEvent(event);
    }
    void mouseMoveEvent(QMouseEvent *event){
        textEdit->navigateAreaMouseEvent(event);
    }
    void mouseReleaseEvent(QMouseEvent *event){
        textEdit->navigateAreaMouseEvent(event);
    }
    void enterEvent(QEvent * event) {
        this->setMouseTracking(true);
        textEdit->navigateAreaEnterEvent(event);
    }
    void leaveEvent(QEvent *event){
        this->setMouseTracking(false);
        textEdit->navigateAreaLeaveEvent(event);
    }
protected:
    LiteEditorWidgetBase *textEdit;
};

class BaseTextLexer : public LiteApi::ITextLexer
{
public:
    BaseTextLexer(QObject *parent = 0) : LiteApi::ITextLexer(parent),
        m_bAC(true), m_bCC(true)
    {
    }
    virtual bool isLangSupport() const {
        return false;
    }
    virtual bool isInComment(const QTextCursor &/*cursor*/) const {
        return false;
    }
    virtual bool isInString(const QTextCursor &cursor) const {
        int pos = cursor.positionInBlock();
        if (pos == 0) {
            return false;
        }
        QString text = cursor.block().text();
        if (text.mid(pos).indexOf("\"") < 0) {
            return false;
        }
        if (text.left(pos).indexOf("\"") < 0) {
            return false;
        }
        return true;
    }
    virtual bool isInEmptyString(const QTextCursor &cursor) const {
        int pos = cursor.positionInBlock();
        if (pos > 0) {
            QString text = cursor.block().text();
            return text.at(pos-1) == '\"' && text.at(pos) == '\"';
        }
        return false;
    }
    virtual bool isEndOfString(const QTextCursor &cursor) const {
        int pos = cursor.positionInBlock();
        return cursor.block().text().at(pos) == '\"';
    }
    virtual bool isInStringOrComment(const QTextCursor &cursor) const {
        if (isInString(cursor)) {
            return true;
        }
        return false;
    }
    virtual bool isInImport(const QTextCursor &/*cursor*/) const {
        return false;
    }
    virtual bool isCanAutoCompleter(const QTextCursor &/*cursor*/) const {
        return m_bAC;
    }
    virtual void setCanCodeCompleter(bool b) {
        m_bCC = b;
    }
    virtual void setCanAutoCompleter(bool b) {
        m_bAC = b;
    }
    virtual int startOfFunctionCall(const QTextCursor &/*cursor*/) const {
        return -1;
    }
    virtual QString fetchFunctionTip(const QString &/*func*/, const QString &/*kind*/, const QString &/*info*/)
    {
        return QString();
    }
    virtual bool fetchFunctionArgs(const QString &/*str*/, int &/*argnr*/, int &/*parcount*/)
    {
        return false;
    }
protected:
    bool m_bAC;
    bool m_bCC;
};

LiteEditorWidgetBase::LiteEditorWidgetBase(LiteApi::IApplication *app, QWidget *parent)
    : QPlainTextEdit(parent),
      m_liteApp(app),
      m_editorMark(0),
      m_textLexer(new BaseTextLexer()),
      m_contentsChanged(false),
      m_lastCursorChangeWasInteresting(false)
{
    m_inputCursorOffset = 0;
    m_upToolTipTime = 200;
    m_linkPressed = false;
    m_moveLineUndoHack = false;
    setLineWrapMode(QPlainTextEdit::NoWrap);
    m_extraArea = new TextEditExtraArea(this);
    m_navigateArea = new TextEditNavigateArea(this);
    m_navigateManager = new NavigateManager(this);

    m_indentLineForeground = QColor(Qt::darkCyan);
    m_visualizeWhitespaceForeground = QColor(Qt::darkGray);
    m_extraForeground = QColor(Qt::darkCyan);
    m_extraBackground = m_extraArea->palette().color(QPalette::Background);
    m_currentLineBackground = QColor(180,200,200,128);

    setLayoutDirection(Qt::LeftToRight);
    viewport()->setMouseTracking(true);
    m_defaultWordWrap = false;
    m_wordWrapOverridden = false;
    m_wordWrap = false;
    m_lineNumbersVisible = true;
    m_navigateWidgetVisible = true;
    m_marksVisible = true;
    m_codeFoldingVisible = true;
    m_rightLineVisible = true;
    m_eofVisible = false;
    m_indentLineVisible = true;
    m_rightLineWidth = 80;
    m_lastSaveRevision = 0;
    m_extraAreaSelectionNumber = -1;
    m_autoIndent = true;
    m_bLastBraces = false;
    m_bTabUseSpace = false;
    m_nTabSize = 4;
    m_mouseOnFoldedMarker = false;
    m_uplinkSkip = false;
    m_mouseNavigation = true;
    m_showLinkNavigation = false;
    m_showLinkInfomation = false;
    m_visualizeWhitespace = false;
    m_lastLine = -1;
    m_inBlockSelectionMode = false;
    m_maxTipInfoLines = 10;

    m_upToolTipDeployTimer = new QTimer(this);
    m_upToolTipDeployTimer->setSingleShot(true);
    connect(m_upToolTipDeployTimer,SIGNAL(timeout()),this,SLOT(uplinkDeployTimeout()));
    m_upToolTipTimer = new QTimer(this);
    m_upToolTipTimer->setSingleShot(true);
    connect(m_upToolTipTimer,SIGNAL(timeout()),this,SLOT(uplinkInfoTimeout()));

    m_selectionExpression.setCaseSensitivity(Qt::CaseSensitive);
    m_selectionExpression.setPatternSyntax(QRegExp::FixedString);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(slotUpdateExtraAreaWidth()));
    connect(this, SIGNAL(modificationChanged(bool)), this, SLOT(slotModificationChanged(bool)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()));
    //connect(this, SIGNAL(selectionChanged()),this,SLOT(updateSelection()));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(slotUpdateRequest(QRect, int)));
    connect(this->document(),SIGNAL(contentsChange(int,int,int)),this,SLOT(editContentsChanged(int,int,int)));
    connect(this,SIGNAL(selectionChanged()),this,SLOT(slotSelectionChanged()));

    QTextDocument *doc = this->document();
    if (doc) {
        TextEditor::BaseTextDocumentLayout *layout = new TextEditor::BaseTextDocumentLayout(doc);
        layout->m_tabSettings.m_autoIndent = true;
        layout->m_tabSettings.m_autoSpacesForTabs = m_bTabUseSpace;
        layout->m_tabSettings.m_tabSize = m_nTabSize;
        doc->setDocumentLayout(layout);
        connect(layout,SIGNAL(updateBlock(QTextBlock)),this,SLOT(updateBlock(QTextBlock)));
        connect(layout,SIGNAL(documentSizeChanged(QSizeF)),this,SLOT(documentSizeChanged(QSizeF)));
    }
    updateTabWidth();
}

LiteEditorWidgetBase::~LiteEditorWidgetBase()
{
}

static void indentBlock(QTextDocument *doc,
                                 const QTextBlock &block,
                                 const QChar &typedChar,
                                 const TextEditor::TabSettings &tabSettings)
{
    Q_UNUSED(typedChar)

    // At beginning: Leave as is.
    if (block == doc->begin())
        return;

    if (block.text().isEmpty()) {
        return;
    }

    QString previousText;
    QTextBlock previous = block.previous();
    while (previous.isValid()) {
        previousText = previous.text();
        if (!previousText.isEmpty() && !previousText.trimmed().isEmpty()) {
            break;
        }
        previous = previous.previous();
    }
    // Empty line indicates a start of a new paragraph. Leave as is.
    if (previousText.isEmpty() || previousText.trimmed().isEmpty())
        return;

    int offset = 0;
    QString text = previousText.trimmed();
    if (text.endsWith("{") || text.endsWith("(")) {
        offset += tabSettings.m_tabSize;
    }
    text = block.text().trimmed();
    if (text.startsWith("}") || text.startsWith(")")) {
        offset -= tabSettings.m_tabSize;
    }
    int i = 0;
    while (i < previousText.size()) {
        if (!previousText.at(i).isSpace()) {
            tabSettings.indentLine(block, tabSettings.columnAt(previousText, i+offset));
            break;
        }
        ++i;
    }
}

static void autoIndent(QTextDocument *doc, const QTextCursor &cursor, const TextEditor::TabSettings &tabSettings)
{
    if (cursor.hasSelection()) {
        QTextBlock block = doc->findBlock(cursor.selectionStart());
        const QTextBlock end = doc->findBlock(cursor.selectionEnd()).next();

        // skip empty blocks
        while (block.isValid() && block != end) {
//            QString bt = block.text();
//            if (tabSettings.firstNonSpace(bt) < bt.size())
//                break;
            indentBlock(doc, block, QChar::Null, tabSettings);
            block = block.next();
        }
//        int previousIndentation = tabSettings.indentationColumn(block.text());
//        indentBlock(doc, block, QChar::Null, tabSettings);
//        int currentIndentation = tabSettings.indentationColumn(block.text());
//        int delta = currentIndentation - previousIndentation;

//        block = block.next();
//        while (block.isValid() && block != end) {
//            tabSettings.reindentLine(block, delta);
//            block = block.next();
//        }
    } else {
        indentBlock(doc, cursor.block(), QChar::Null, tabSettings);
    }
}

void LiteEditorWidgetBase::setEditorMark(LiteApi::IEditorMark *mark)
{
    m_editorMark = mark;
    if (m_editorMark) {
        connect(m_editorMark,SIGNAL(markChanged()),m_extraArea,SLOT(update()));
    }
}

void LiteEditorWidgetBase::setTextLexer(LiteApi::ITextLexer *lexer)
{
    m_textLexer.reset(lexer);
}

void LiteEditorWidgetBase::setTabSize(int n)
{
    m_nTabSize = n;
    updateTabWidth();
    TextEditor::BaseTextDocumentLayout *layout = (TextEditor::BaseTextDocumentLayout*)document()->documentLayout();
    if (layout) {
        layout->m_tabSettings.m_tabSize = m_nTabSize;
    }
}

int LiteEditorWidgetBase::tabSize() const
{
    return m_nTabSize;
}

void LiteEditorWidgetBase::updateTabWidth()
{
    setTabStopWidth(QFontMetrics(font()).averageCharWidth() * m_nTabSize);
}

void LiteEditorWidgetBase::setTabToSpaces(bool b)
{
    m_bTabUseSpace = b;
    TextEditor::BaseTextDocumentLayout *layout = (TextEditor::BaseTextDocumentLayout*)document()->documentLayout();
    if (layout) {
        layout->m_tabSettings.m_autoSpacesForTabs = m_bTabUseSpace;
    }
}

void LiteEditorWidgetBase::initLoadDocument()
{
    m_lastSaveRevision = document()->revision();
    document()->setModified(false);
    this->moveCursor(QTextCursor::Start);
}


void LiteEditorWidgetBase::editContentsChanged(int, int, int)
{
    m_contentsChanged = true;
}

struct MatchBracePos {
    int startPos;
    int matchPos;
    bool startIsLeft;
    bool startIsOpen;
    TextEditor::TextBlockUserData::MatchType matchType;
};

static bool findMatchBrace(QTextCursor &cur, MatchBracePos &mb)
{
    QTextBlock block = cur.block();
    int pos = cur.positionInBlock();
    mb.startPos = -1;
    mb.matchPos = -1;
    if (block.isValid()) {
        TextEditor::TextBlockUserData *data = static_cast<TextEditor::TextBlockUserData*>(block.userData());
        if (data) {
            TextEditor::Parentheses ses = data->parentheses();
            QChar chr;
            int i = ses.size();
            while(i--) {
                TextEditor::Parenthesis s = ses.at(i);
                if (s.pos == pos || s.pos+1 == pos) {
                    mb.startPos = cur.block().position()+s.pos;
                    mb.startIsLeft = (s.pos == pos);
                    mb.startIsOpen = (s.type == TextEditor::Parenthesis::Opened);
                    chr = s.chr;
                    break;
                }
            }
            if (mb.startPos != -1) {
                if (mb.startIsOpen) {
                    cur.setPosition(mb.startPos);
                    mb.matchType = TextEditor::TextBlockUserData::checkOpenParenthesis(&cur,chr);
                    mb.matchPos = cur.position()-1;
                } else {
                    cur.setPosition(mb.startPos+1);
                    mb.matchType = TextEditor::TextBlockUserData::checkClosedParenthesis(&cur,chr);
                    mb.matchPos = cur.position();
                }
                return true;
            }
        }
    }
    return false;
}

void LiteEditorWidgetBase::gotoMatchBrace()
{
    QTextCursor cur = this->textCursor();
    MatchBracePos mb;
    if (findMatchBrace(cur,mb) && mb.matchType == TextEditor::TextBlockUserData::Match) {
        if (mb.startIsLeft) {
            cur.setPosition(mb.matchPos+1);
        } else {
            cur.setPosition(mb.matchPos);
        }
        this->setTextCursor(cur);
        if (!cur.block().isVisible()) {
            unfold();
        }
        ensureCursorVisible();
    }
}

void LiteEditorWidgetBase::highlightCurrentLine()
{    
    QTextCursor cur = textCursor();
    if (!cur.block().isVisible()) {
        unfold();
    }

//    if (!isReadOnly()) {
//        QTextEdit::ExtraSelection full;
//        full.format.setBackground(m_currentLineBackground);
//        full.format.setProperty(QTextFormat::FullWidthSelection, true);
//        full.cursor = this->textCursor();
//        //full.cursor.select(QTextCursor::LineUnderCursor);
//        setExtraSelections(LiteApi::CurrentLineSelection,QList<QTextEdit::ExtraSelection>() << full );
//    }

    QList<QTextEdit::ExtraSelection> extraSelections;
    MatchBracePos mb;
    if (findMatchBrace(cur,mb)) {
        if (mb.matchType == TextEditor::TextBlockUserData::Match) {
            QTextEdit::ExtraSelection selection;
            cur.setPosition(mb.startPos);
            cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,1);
            selection.cursor = cur;
            selection.format.setFontUnderline(true);
            selection.format.setProperty(LiteEditorWidgetBase::MatchBrace,true);
            extraSelections.append(selection);

            cur.setPosition(mb.matchPos);
            cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,1);
            selection.cursor = cur;
            selection.format.setFontUnderline(true);
            selection.format.setProperty(LiteEditorWidgetBase::MatchBrace,true);
            extraSelections.append(selection);
        } else if (mb.matchType == TextEditor::TextBlockUserData::Mismatch) {
            QTextEdit::ExtraSelection selection;
            cur.setPosition(mb.startPos);
            cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,1);
            selection.cursor = cur;
            selection.format.setFontUnderline(true);
            selection.format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
            selection.format.setProperty(LiteEditorWidgetBase::MatchBrace,true);
            selection.format.setForeground(Qt::red);
            extraSelections.append(selection);
        }
    }
    setExtraSelections(LiteApi::ParenthesesMatchingSelection,extraSelections);
    this->update();
}

static int foldBoxWidth(const QFontMetrics &fm)
{
    const int lineSpacing = fm.lineSpacing();
    return lineSpacing/2+lineSpacing%2+1;
}

QWidget* LiteEditorWidgetBase::extraArea()
{
    return m_extraArea;
}

QWidget *LiteEditorWidgetBase::navigateArea()
{
    return m_navigateArea;
}

void LiteEditorWidgetBase::setCurrentLineColor(const QColor &background)
{
    if (background.isValid()) {
        m_currentLineBackground = background;
    } else {
        m_currentLineBackground = QColor(180,200,200,128);
    }
    m_currentLineBackground.setAlpha(128);
}

void LiteEditorWidgetBase::setIndentLineColor(const QColor &foreground)
{
    if (foreground.isValid()) {
        m_indentLineForeground = foreground;
    } else {
        m_indentLineForeground = QColor(Qt::darkCyan);
    }
    m_indentLineForeground.setAlpha(128);
}

void LiteEditorWidgetBase::setVisualizeWhitespaceColor(const QColor &foreground)
{
    if (foreground.isValid()) {
        m_visualizeWhitespaceForeground = foreground;
    } else {
        m_visualizeWhitespaceForeground = QColor(Qt::darkGray);
    }
    m_visualizeWhitespaceForeground.setAlpha(200);
}

void LiteEditorWidgetBase::setExtraColor(const QColor &foreground,const QColor &background)
{
    if (foreground.isValid()) {
        m_extraForeground = foreground;
    } else {
        m_extraForeground = QColor(Qt::darkCyan);
    }
    if (background.isValid()) {
        m_extraBackground = background;
    } else {
        m_extraBackground = m_extraArea->palette().color(QPalette::Background);
    }
}

int LiteEditorWidgetBase::extraAreaWidth()
{
    int space = 0;
    const QFontMetrics fm(m_extraArea->fontMetrics());
    if (m_lineNumbersVisible) {
        QFont fnt = m_extraArea->font();
        fnt.setBold(true);
        const QFontMetrics linefm(fnt);
        int digits = 2;
        int max = qMax(1, blockCount());
        while (max >= 100) {
            max /= 10;
            ++digits;
        }
        space += linefm.width(QLatin1Char('9')) * digits;
    }
    if (m_marksVisible) {
        int markWidth = fm.lineSpacing();
        space += markWidth;
    } else {
        space += 3;
    }
    if (m_codeFoldingVisible) {
        space += foldBoxWidth(fm);
    }
    space += 4;

    return space;
}

void LiteEditorWidgetBase::drawFoldingMarker(QPainter *painter, const QPalette&,
                                       const QRect &rect,
                                       bool expanded) const
{
    painter->save();
    painter->setPen(Qt::NoPen);
    int size = rect.size().width();
    int sqsize = 2*(size/2);

    QColor textColor = m_extraForeground;
    QColor brushColor = m_extraBackground;

    textColor.setAlpha(128);
    brushColor.setAlpha(128);
    QPolygon a;
    if (expanded) {
        // down arrow
        //a.setPoints(3, 0, sqsize/3,  sqsize/2, sqsize  - sqsize/3,  sqsize, sqsize/3);
        a.setPoints(3, 1, sqsize/2+sqsize/3,  sqsize/2+sqsize/3, sqsize/2+sqsize/3,sqsize/2+sqsize/3,1);
    } else {
        // right arrow
        a.setPoints(3, sqsize - sqsize/3, sqsize/2,  sqsize/2 - sqsize/3, 0,  sqsize/2 - sqsize/3, sqsize);
    }
    painter->translate(0.5, 0.5);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->translate(rect.topLeft());
    painter->setPen(textColor);
    if (expanded) {
        painter->setBrush(textColor);
    } else {
        painter->setBrush(brushColor);
    }
    painter->drawPolygon(a);
    painter->restore();

}

void LiteEditorWidgetBase::extraAreaPaintEvent(QPaintEvent *e)
{
    QTextDocument *doc = document();

    int selStart = textCursor().selectionStart();
    int selEnd = textCursor().selectionEnd();

    QPalette pal = m_extraArea->palette();
    pal.setCurrentColorGroup(QPalette::Active);
    QPainter painter(m_extraArea);
    const QFontMetrics fm(m_extraArea->font());

    int fmLineSpacing = fm.lineSpacing();
    int markWidth = 0;
    if (m_marksVisible)
        markWidth += fm.lineSpacing();

    const int collapseColumnWidth = m_codeFoldingVisible ? foldBoxWidth(fm): 0;
    const int extraAreaWidth = m_extraArea->width() - collapseColumnWidth;

//    painter.fillRect(e->rect(), pal.color(QPalette::Base));
//    painter.fillRect(e->rect().intersected(QRect(0, 0, m_extraArea->width(), INT_MAX)),
//                     m_extraBackground);
    painter.fillRect(e->rect(),m_extraBackground);
    //painter.setPen(QPen(m_extraForeground,1,Qt::DotLine));
   // painter.drawLine(extraAreaWidth - 3, e->rect().top(), extraAreaWidth - 3, e->rect().bottom());
    //painter.drawLine(e->rect().width()-1, e->rect().top(), e->rect().width()-1, e->rect().bottom());

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    qreal top = blockBoundingGeometry(block).translated(contentOffset()).top();
    qreal bottom = top;

    painter.setPen(QPen(m_extraForeground,1));
    while (block.isValid() && top <= e->rect().bottom()) {

        top = bottom;
        const qreal height = blockBoundingRect(block).height();
        bottom = top + height;
        QTextBlock nextBlock = block.next();

        QTextBlock nextVisibleBlock = nextBlock;
        int nextVisibleBlockNumber = blockNumber + 1;

        if (!nextVisibleBlock.isVisible()) {
            // invisible blocks do have zero line count
            nextVisibleBlock = doc->findBlockByLineNumber(nextVisibleBlock.firstLineNumber());
            nextVisibleBlockNumber = nextVisibleBlock.blockNumber();
        }

        if (bottom < e->rect().top()) {
            block = nextVisibleBlock;
            blockNumber = nextVisibleBlockNumber;
            continue;
        }

        if (m_codeFoldingVisible || m_marksVisible) {
            painter.save();
            painter.setRenderHint(QPainter::Antialiasing, false);

            int previousBraceDepth = block.previous().userState();
            if (previousBraceDepth >= 0)
                previousBraceDepth >>= 8;
            else
                previousBraceDepth = 0;

            int braceDepth = block.userState();
            if (!nextBlock.isVisible()) {
                QTextBlock lastInvisibleBlock = nextVisibleBlock.previous();
                if (!lastInvisibleBlock.isValid())
                    lastInvisibleBlock = doc->lastBlock();
                braceDepth = lastInvisibleBlock.userState();
            }
            if (braceDepth >= 0)
                braceDepth >>= 8;
            else
                braceDepth = 0;

            if (TextEditor::TextBlockUserData *userData = static_cast<TextEditor::TextBlockUserData*>(block.userData())) {
                if (m_marksVisible) {
                    int xoffset = 0;
                    foreach (TextEditor::ITextMark *mrk, userData->marks()) {
                        int x = 0;
                        int radius = fmLineSpacing - 1;
                        QRect r(x + xoffset, top, radius, radius);
                        mrk->paint(&painter, r);
                        xoffset += 2;
                    }
                }
            }

            if (m_codeFoldingVisible) {
                TextEditor::TextBlockUserData *nextBlockUserData = TextEditor::BaseTextDocumentLayout::testUserData(nextBlock);

                bool drawBox = nextBlockUserData
                               && TextEditor::BaseTextDocumentLayout::foldingIndent(block) < nextBlockUserData->foldingIndent();

                int boxWidth = foldBoxWidth(fm)+1;
                if (drawBox) {
                    bool expanded = nextBlock.isVisible();
                    QRect box(extraAreaWidth-2, top + (fm.lineSpacing()-boxWidth)/2,
                              boxWidth-1,boxWidth-1);
                    drawFoldingMarker(&painter, pal, box, expanded);
                }
            }

            painter.restore();
        }


        if (block.revision() != m_lastSaveRevision) {
            painter.save();
            painter.setRenderHint(QPainter::Antialiasing, false);
            if (block.revision() < 0)
                painter.setPen(QPen(Qt::darkGreen, 2));
            else
                painter.setPen(QPen(Qt::red, 2));
            painter.drawLine(extraAreaWidth - 1, top, extraAreaWidth - 1, bottom - 1);
            painter.restore();
        }


//        if (/*m_marksVisible &&*/ m_editorMark) {
//            m_editorMark->paint(&painter,blockNumber,0,top,fmLineSpacing-0.5,fmLineSpacing-1);
//        }
        if (m_lineNumbersVisible) {

            painter.setPen(QPen(m_extraForeground,2));//pal.color(QPalette::BrightText));
            //int indent = TextEditor::BaseTextDocumentLayout::foldingIndent(block);
            const QString &number = QString::number(blockNumber + 1);
            //const QString &number2 = QString::number(indent);
            bool selected = (
                    (selStart < block.position() + block.length()
                    && selEnd > block.position())
                    || (selStart == selEnd && selStart == block.position())
                    );            
            if (selected) {
                painter.save();
                QFont f = painter.font();
                f.setBold(true);
                painter.setFont(f);
                //painter.setPen(QPen(Qt::black,2));
            }
            painter.drawText(QRectF(markWidth, top, extraAreaWidth - markWidth - 4, height), Qt::AlignRight, number);
            if (selected)
                painter.restore();
            painter.setPen(QPen(m_extraForeground,1));//pal.color(QPalette::BrightText));
        }
        block = nextVisibleBlock;
        blockNumber = nextVisibleBlockNumber;
    }

}

void LiteEditorWidgetBase::extraAreaMouseEvent(QMouseEvent *e)
{
    QTextCursor cursor = cursorForPosition(QPoint(0, e->pos().y()));
    if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonDblClick) {
        if (e->button() == Qt::LeftButton) {
            int boxWidth = foldBoxWidth(fontMetrics());
            QTextBlock block = cursor.block();
            bool canFold = TextEditor::BaseTextDocumentLayout::canFold(block);
            if (m_codeFoldingVisible && canFold && e->pos().x() >= extraAreaWidth() - boxWidth-4) {
                if (!cursor.block().next().isVisible()) {
                    toggleBlockVisible(cursor.block());
                    //moveCursorVisible(false);
                } else {
                    QTextBlock c = cursor.block();
                    toggleBlockVisible(c);
                    moveCursorVisible(false);
                }
            } else {
                QTextCursor selection = cursor;
                selection.setVisualNavigation(true);
                m_extraAreaSelectionNumber = selection.blockNumber();
                selection.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                selection.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                setTextCursor(selection);
            }
        }
    } else if (m_extraAreaSelectionNumber >= 0) {
        QTextCursor selection = cursor;
        selection.setVisualNavigation(true);
        if (e->type() == QEvent::MouseMove) {
            QTextBlock anchorBlock = document()->findBlockByNumber(m_extraAreaSelectionNumber);
            selection.setPosition(anchorBlock.position());
            if (cursor.blockNumber() < m_extraAreaSelectionNumber) {
                selection.movePosition(QTextCursor::EndOfBlock);
                selection.movePosition(QTextCursor::Right);
            }
            selection.setPosition(cursor.block().position(), QTextCursor::KeepAnchor);
            if (cursor.blockNumber() >= m_extraAreaSelectionNumber) {
                selection.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                selection.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            }
        } else {
            m_extraAreaSelectionNumber = -1;
            return;
        }
        setTextCursor(selection);
    }
}

void LiteEditorWidgetBase::extraAreaLeaveEvent(QEvent *)
{

}

int LiteEditorWidgetBase::navigateAreaWidth()
{
    return 16;
}

void LiteEditorWidgetBase::navigateAreaPaintEvent(QPaintEvent *e)
{
    QPalette pal = m_navigateArea->palette();
    pal.setCurrentColorGroup(QPalette::Active);
    QPainter painter(m_navigateArea);

    //painter.fillRect(e->rect(), pal.color(QPalette::Base));
    //painter.fillRect(e->rect().intersected(QRect(0, 0, m_navigateArea->width(), m_navigateArea->height())),
    //                 m_extraBackground);
    painter.fillRect(e->rect(),m_extraBackground);

    int width = this->navigateAreaWidth();
    painter.fillRect(2,2,width-4,width-4,m_navigateManager->headMarkColor());
    int count = this->blockCount();
    int height = this->viewport()->rect().height()-2*m_navigateArea->width();
    QMapIterator<int,NavigateMark*> i(m_navigateManager->markMap);
    while(i.hasNext()) {
        i.next();
        if (!i.value()->isEmpty()) {
            int pos = i.key()*height*1.0/count;
            //QColor markColor;
            NavigateMark::Node *node;
            const LiteApi::EditorNaviagteType * pList = MARKTYPE_PRIORITYLIST;
            for (int j=0; j<PRIORITYLIST_LENGTH; j++) {
                node = i.value()->findNode(pList[j]);
                if (node) {
                    painter.fillRect(2,width+pos,width-4,4,node->markColor());
                    break;
                }
            }
        }
    }
}

int LiteEditorWidgetBase::isInNavigateMark(const QPoint &pos, int *poffset)
{
    int count = this->blockCount();
    int height = this->viewport()->rect().height()-2*m_navigateArea->width();
    int width = m_navigateArea->width();
    QMapIterator<int,NavigateMark*> i(m_navigateManager->markMap);
    while(i.hasNext()) {
        i.next();
        if (!i.value()->isEmpty()) {
            int offset = i.key()*height*1.0/count;
            QRect rect(0,width+offset-1,width,4+1);
            if (rect.contains(pos)) {
                if (poffset) {
                    *poffset = offset+width;
                }
                return i.key();
            }
        }
    }
    return -1;
}

bool LiteEditorWidgetBase::isInNavigateHead(const QPoint &pos)
{
    int width = m_navigateArea->width();
    QRect rect(0,0,width,width);
    if (rect.contains(pos)) {
        return true;
    }
    return false;
}

void LiteEditorWidgetBase::navigateAreaMouseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton &&
        (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonDblClick)) {
        int line = isInNavigateMark(e->pos(),0);
        if (line != -1) {
            this->gotoLine(line,0,true);
        }
    } else if (e->type() == QEvent::MouseMove) {
        bool tooltip = false;
        if (isInNavigateHead(e->pos())) {
            tooltip = true;
            QPoint pos(1,1);
            QToolTip::showText(m_navigateArea->mapToGlobal(pos),this->m_navigateManager->m_msg,this->m_navigateArea);
        } else {
            int offset = 0;
            int line = isInNavigateMark(e->pos(),&offset);
            if (line != -1) {
                NavigateMark *mark = m_navigateManager->markMap.value(line);
//                NavigateMark::Node *node = mark->findNode(LiteApi::EditorNavigateError);
//                if (node) {
//                    tooltip = true;
//                    QPoint pos(1,offset);
//                    QToolTip::showText(m_navigateArea->mapToGlobal(pos),node->msg,this->m_navigateArea);
//                }
                const LiteApi::EditorNaviagteType * pList = MARKTYPE_PRIORITYLIST;
                for (int j=0; j<PRIORITYLIST_LENGTH; j++) {
                    NavigateMark::Node *node = mark->findNode(pList[j]);
                    if (node) {
                        tooltip = true;
                        QPoint pos(1,offset);
                        QToolTip::showText(m_navigateArea->mapToGlobal(pos),node->msg,this->m_navigateArea);
                        break;
                    }
                }
            }
        }
        if (!tooltip) {
            QToolTip::hideText();
        }
    }
}

void LiteEditorWidgetBase::navigateAreaLeaveEvent(QEvent */*e*/)
{
}

void LiteEditorWidgetBase::navigateAreaEnterEvent(QEvent */*e*/)
{
}

void LiteEditorWidgetBase::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    m_extraArea->setGeometry(
                QStyle::visualRect(layoutDirection(), cr,
                                   QRect(cr.left(), cr.top(), extraAreaWidth(), cr.height())));
    m_navigateArea->setGeometry(
                QStyle::visualRect(layoutDirection(), cr,
                                   QRect(cr.left()+extraAreaWidth()+viewport()->rect().width(), cr.top(), navigateAreaWidth(), cr.height())));
    m_navigateArea->update();
}


void LiteEditorWidgetBase::slotUpdateExtraAreaWidth()
{
    if (isLeftToRight())
        setViewportMargins(extraAreaWidth(), 0, navigateAreaWidth(), 0);
    else
        setViewportMargins(navigateAreaWidth(), 0, extraAreaWidth(), 0);
}

void LiteEditorWidgetBase::slotModificationChanged(bool m)
{
    if (m)
        return;

    int oldLastSaveRevision = m_lastSaveRevision;
    m_lastSaveRevision = document()->revision();

    if (oldLastSaveRevision != m_lastSaveRevision) {
        QTextBlock block = document()->begin();
        while (block.isValid()) {
            if (block.revision() < 0 || block.revision() != oldLastSaveRevision) {
                block.setRevision(-m_lastSaveRevision - 1);
            } else {
                block.setRevision(m_lastSaveRevision);
            }
            block = block.next();
        }
    }
    m_extraArea->update();
}

void LiteEditorWidgetBase::slotUpdateRequest(const QRect &r, int dy)
{
    if (dy)
        m_extraArea->scroll(0, dy);
    else if (r.width() > 4) { // wider than cursor width, not just cursor blinking
        m_extraArea->update(0, r.y(), m_extraArea->width(), r.height());
        //if (!d->m_searchExpr.isEmpty()) {
        //    const int m = d->m_searchResultOverlay->dropShadowWidth();
        //    viewport()->update(r.adjusted(-m, -m, m, m));
        //}
    }
    if (r.contains(viewport()->rect()))
        slotUpdateExtraAreaWidth();
}

static bool convertPosition(const QTextDocument *document, int pos, int *line, int *column)
{
    QTextBlock block = document->findBlock(pos);
    if (!block.isValid()) {
        (*line) = -1;
        (*column) = -1;
        return false;
    } else {
        (*line) = block.blockNumber() + 1;
        (*column) = pos - block.position();
        return true;
    }
}

QByteArray LiteEditorWidgetBase::saveState() const
{
    QByteArray state;
    QDataStream stream(&state, QIODevice::WriteOnly);
    stream << 2; // version number
    stream << verticalScrollBar()->value();
    stream << horizontalScrollBar()->value();
    int line, column;
    convertPosition(document(),textCursor().position(), &line, &column);
    stream << line-1;
    stream << column;

    // store code folding state
    QList<int> foldedBlocks;
    QTextBlock block = document()->firstBlock();
    while (block.isValid()) {
        if (block.userData() && static_cast<TextEditor::TextBlockUserData*>(block.userData())->folded()) {
            int number = block.blockNumber();
            foldedBlocks += number;
        }
        block = block.next();
    }
    stream << foldedBlocks;

    // store word wrap state
    stream << m_wordWrapOverridden;
    stream << m_wordWrap;

    return state;
}

bool LiteEditorWidgetBase::restoreState(const QByteArray &state)
{
    if (state.isEmpty()) {
        return false;
    }
    int version;
    int vval;
    int hval;
    int lval;
    int cval;
    QDataStream stream(state);
    stream >> version;
    stream >> vval;
    stream >> hval;
    stream >> lval;
    stream >> cval;

    if (version >= 1) {
        QList<int> collapsedBlocks;
        stream >> collapsedBlocks;
        QTextDocument *doc = document();
        foreach(int blockNumber, collapsedBlocks) {
            QTextBlock block = doc->findBlockByNumber(qMax(0, blockNumber));
            if (block.isValid())
                TextEditor::BaseTextDocumentLayout::doFoldOrUnfold(block, false);
        }
        if (!collapsedBlocks.empty()) {
            this->update();
        }
    }

    m_lastCursorChangeWasInteresting = false; // avoid adding last position to history

    gotoLine(lval, cval,false);
    verticalScrollBar()->setValue(vval);
    horizontalScrollBar()->setValue(hval);
    saveCurrentCursorPositionForNavigation();

    if (version >= 2) {
        stream >> m_wordWrapOverridden;
        stream >> m_wordWrap;
        setWordWrap(m_wordWrap);
    }

    return true;
}

void LiteEditorWidgetBase::saveCurrentCursorPositionForNavigation()
{
    m_lastCursorChangeWasInteresting = true;
    m_tempNavigationState = saveState();
}

void LiteEditorWidgetBase::slotCursorPositionChanged()
{
    if (m_lastCursorChangeWasInteresting) {
        //navigate change
        int lastLine = this->textCursor().blockNumber();
        if (lastLine != m_lastLine) {
            m_lastLine = lastLine;
            emit navigationStateChanged(m_tempNavigationState);
            m_lastCursorChangeWasInteresting = false;
        }
    } else {
        this->saveCurrentCursorPositionForNavigation();
    }
    //emit navigationStateChanged(saveState());
    /*
    if (!m_contentsChanged && m_lastCursorChangeWasInteresting) {
        //navigate change
        emit navigationStateChanged(m_tempNavigationState);
        m_lastCursorChangeWasInteresting = false;
    } else if (m_contentsChanged) {
        this->saveCurrentCursorPositionForNavigation();
    }
    */
    highlightCurrentLine();
}

void LiteEditorWidgetBase::slotSelectionChanged()
{
    QString pattern;
    QTextCursor cur = this->textCursor();

    if (cur.hasSelection()) {
        QString text = cur.selectedText();
        cur.setPosition(cur.selectionStart());
        cur.select(QTextCursor::WordUnderCursor);
        if (text == cur.selectedText() && isIdentifierChar(*text.begin())) {
            pattern = text;
        }
    }

    if (m_selectionExpression.pattern() != pattern) {
        m_selectionExpression.setPattern(pattern);
        viewport()->update();
    }

    if (m_inBlockSelectionMode && !textCursor().hasSelection()) {
        m_inBlockSelectionMode = false;
        m_blockSelection.clear();
        viewport()->update();
    }
    //clearLink();
}

void LiteEditorWidgetBase::slotUpdateBlockNotify(const QTextBlock &)
{

}

void LiteEditorWidgetBase::maybeSelectLine()
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection()) {
        const QTextBlock &block = cursor.block();
        if (block.next().isValid()) {
            cursor.setPosition(block.position());
            cursor.setPosition(block.next().position(), QTextCursor::KeepAnchor);
        } else {
            cursor.movePosition(QTextCursor::EndOfBlock);
            cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
        }
        setTextCursor(cursor);
    }
}

void LiteEditorWidgetBase::gotoLine(int line, int column, bool center)
{
    m_lastCursorChangeWasInteresting = false;
    const int blockNumber = line;
    const QTextBlock &block = document()->findBlockByNumber(blockNumber);
    if (block.isValid()) {
        QTextCursor cursor(block);
        if (column > 0) {
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
        } else {
            int pos = cursor.position();
            while (document()->characterAt(pos).category() == QChar::Separator_Space) {
                ++pos;
            }
            cursor.setPosition(pos);
        }
        setTextCursor(cursor);
        if (center) {
            centerCursor();
        } else {
            ensureCursorVisible();
        }
    }
}

QChar LiteEditorWidgetBase::characterAt(int pos) const
{
    return document()->characterAt(pos);
}
void LiteEditorWidgetBase::handleHomeKey(bool anchor)
{
    QTextCursor cursor = textCursor();
    QTextCursor::MoveMode mode = QTextCursor::MoveAnchor;

    if (anchor)
        mode = QTextCursor::KeepAnchor;

    const int initpos = cursor.position();
    int pos = cursor.block().position();
    QChar character = characterAt(pos);
    const QLatin1Char tab = QLatin1Char('\t');

    while (character == tab || character.category() == QChar::Separator_Space) {
        ++pos;
        if (pos == initpos)
            break;
        character = characterAt(pos);
    }

    // Go to the start of the block when we're already at the start of the text
    if (pos == initpos)
        pos = cursor.block().position();

    cursor.setPosition(pos, mode);
    setTextCursor(cursor);
}

void LiteEditorWidgetBase::setFindOption(LiteApi::FindOption *opt)
{
    if (!opt) {
        m_findExpression.setPattern("");
    } else {
        m_findExpression.setPattern(opt->findText);
        if (opt->useRegexp) {
            m_findExpression.setPatternSyntax(QRegExp::RegExp);
        } else {
            m_findExpression.setPatternSyntax(QRegExp::FixedString);
        }
        m_findFlags = 0;
        if (opt->backWard) {
            m_findFlags |= QTextDocument::FindBackward;
        }

        if (opt->matchCase) {
            m_findFlags |= QTextDocument::FindCaseSensitively;
            m_findExpression.setCaseSensitivity(Qt::CaseSensitive);
        } else {
            m_findExpression.setCaseSensitivity(Qt::CaseInsensitive);
        }
        if (opt->matchWord) {
            m_findFlags |= QTextDocument::FindWholeWords;
        }
        if (!m_findExpression.isValid()) {
            m_findExpression.setPattern("");
        }
    }
    viewport()->update();
}

void LiteEditorWidgetBase::setWordWrap(bool wrap)
{
    setLineWrapMode(wrap ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
    m_wordWrap = wrap;
    emit wordWrapChanged(wrap);
}

bool LiteEditorWidgetBase::isWordWrap() const
{
    return this->lineWrapMode() != QPlainTextEdit::NoWrap;
}

void LiteEditorWidgetBase::setWordWrapOverride(bool wrap)
{
    m_wordWrapOverridden = true;
    this->setWordWrap(wrap);
}

void LiteEditorWidgetBase::setDefaultWordWrap(bool wrap)
{
    if (!m_wordWrapOverridden) {
        this->setWordWrap(wrap);
    }
}

void LiteEditorWidgetBase::gotoLineStart()
{
    handleHomeKey(false);
}

void LiteEditorWidgetBase::gotoLineStartWithSelection()
{
    handleHomeKey(true);
}

void LiteEditorWidgetBase::gotoLineEnd()
{
    moveCursor(QTextCursor::EndOfLine);
}

void LiteEditorWidgetBase::gotoLineEndWithSelection()
{
    moveCursor(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
}

void LiteEditorWidgetBase::duplicate()
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    if (cursor.hasSelection()) {
        QString text = cursor.selectedText();
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd();
        cursor.setPosition(end);
        cursor.insertText(text);
        cursor.setPosition(start,QTextCursor::MoveAnchor);
        cursor.setPosition(end,QTextCursor::KeepAnchor);
    } else {
        int pos = cursor.positionInBlock();
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        QString text = cursor.selectedText();
        cursor.movePosition(QTextCursor::EndOfBlock);
        cursor.insertBlock();
        int start = cursor.position();
        cursor.insertText(text);
        cursor.setPosition(start);
        cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,pos);
    }
    cursor.endEditBlock();
    setTextCursor(cursor);
}

// shift+del
void LiteEditorWidgetBase::cutLine()
{
    maybeSelectLine();
    cut();
}

// ctrl+ins
void LiteEditorWidgetBase::copyLine()
{
    QTextCursor prevCursor = textCursor();
    maybeSelectLine();
    copy();
    setTextCursor(prevCursor);
}

void LiteEditorWidgetBase::deleteLine()
{
    maybeSelectLine();
    textCursor().removeSelectedText();
}

void LiteEditorWidgetBase::deleteEndOfWord()
{
    moveCursor(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    textCursor().removeSelectedText();
    setTextCursor(textCursor());
}

void LiteEditorWidgetBase::moveLineUpDown(bool up)
{
    QTextCursor cursor = textCursor();
    QTextCursor move = cursor;

    move.setVisualNavigation(false); // this opens folded items instead of destroying them

    if (m_moveLineUndoHack)
        move.joinPreviousEditBlock();
    else
        move.beginEditBlock();

    bool hasSelection = cursor.hasSelection();

    if (hasSelection) {
        move.setPosition(cursor.selectionStart());
        move.movePosition(QTextCursor::StartOfBlock);
        move.setPosition(cursor.selectionEnd(), QTextCursor::KeepAnchor);
        move.movePosition(move.atBlockStart() ? QTextCursor::Left: QTextCursor::EndOfBlock,
                          QTextCursor::KeepAnchor);
    } else {
        move.movePosition(QTextCursor::StartOfBlock);
        move.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    }
    QString text = move.selectedText();

    move.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    move.removeSelectedText();

    if (up) {
        move.movePosition(QTextCursor::PreviousBlock);
        move.insertBlock();
        move.movePosition(QTextCursor::Left);
    } else {
        move.movePosition(QTextCursor::EndOfBlock);
        if (move.atBlockStart()) { // empty block
            move.movePosition(QTextCursor::NextBlock);
            move.insertBlock();
            move.movePosition(QTextCursor::Left);
        } else {
            move.insertBlock();
        }
    }

    int start = move.position();
    move.clearSelection();
    move.insertText(text);
    int end = move.position();

    if (hasSelection) {
        move.setPosition(end);
        move.setPosition(start, QTextCursor::KeepAnchor);
    } else {
        move.setPosition(start);
    }

    move.endEditBlock();

    setTextCursor(move);
    m_moveLineUndoHack = true;
}

void LiteEditorWidgetBase::copyLineUpDown(bool up)
{
    QTextCursor cursor = textCursor();
    QTextCursor move = cursor;
    move.beginEditBlock();

    bool hasSelection = cursor.hasSelection();

    if (hasSelection) {
        move.setPosition(cursor.selectionStart());
        move.movePosition(QTextCursor::StartOfBlock);
        move.setPosition(cursor.selectionEnd(), QTextCursor::KeepAnchor);
        move.movePosition(move.atBlockStart() ? QTextCursor::Left: QTextCursor::EndOfBlock,
                          QTextCursor::KeepAnchor);
    } else {
        move.movePosition(QTextCursor::StartOfBlock);
        move.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    }

    QString text = move.selectedText();

    if (up) {
        move.setPosition(cursor.selectionStart());
        move.movePosition(QTextCursor::StartOfBlock);
        move.insertBlock();
        move.movePosition(QTextCursor::Left);
    } else {
        move.movePosition(QTextCursor::EndOfBlock);
        if (move.atBlockStart()) {
            move.movePosition(QTextCursor::NextBlock);
            move.insertBlock();
            move.movePosition(QTextCursor::Left);
        } else {
            move.insertBlock();
        }
    }

    int start = move.position();
    move.clearSelection();
    move.insertText(text);
    int end = move.position();

    move.setPosition(start);
    move.setPosition(end, QTextCursor::KeepAnchor);

    move.endEditBlock();

    setTextCursor(move);
}

void LiteEditorWidgetBase::joinLines()
{
    QTextCursor cursor = textCursor();
    QTextCursor start = cursor;
    QTextCursor end = cursor;

    start.setPosition(cursor.selectionStart());
    end.setPosition(cursor.selectionEnd() - 1);

    int lineCount = qMax(1, end.blockNumber() - start.blockNumber());

    cursor.beginEditBlock();
    cursor.setPosition(cursor.selectionStart());
    while (lineCount--) {
        cursor.movePosition(QTextCursor::NextBlock);
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        QString cutLine = cursor.selectedText();

        // Collapse leading whitespaces to one or insert whitespace
        cutLine.replace(QRegExp(QLatin1String("^\\s*")), QLatin1String(" "));
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();

        cursor.movePosition(QTextCursor::PreviousBlock);
        cursor.movePosition(QTextCursor::EndOfBlock);

        cursor.insertText(cutLine);
    }
    cursor.endEditBlock();

    setTextCursor(cursor);
}

void LiteEditorWidgetBase::deleteStartOfWord()
{
    moveCursor(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    textCursor().removeSelectedText();
    setTextCursor(textCursor());
}

void LiteEditorWidgetBase::moveLineUp()
{
    moveLineUpDown(true);
}

void LiteEditorWidgetBase::moveLineDown()
{
    moveLineUpDown(false);
}

void LiteEditorWidgetBase::copyLineUp()
{
    copyLineUpDown(true);
}

void LiteEditorWidgetBase::copyLineDown()
{
    copyLineUpDown(false);
}

void LiteEditorWidgetBase::insertLineBefore()
{
    QTextCursor cur = this->textCursor();
    cur.movePosition(QTextCursor::PreviousBlock);
    cur.movePosition(QTextCursor::EndOfLine);

    if (m_autoIndent) {
        indentEnter(cur);
    } else {
        cur.insertText("\n");
    }

    this->setTextCursor(cur);
}

void LiteEditorWidgetBase::insertLineAfter()
{
    QTextCursor cur = this->textCursor();
    cur.movePosition(QTextCursor::EndOfLine);
    if (m_autoIndent) {
        indentEnter(cur);
    } else {
        cur.insertText("\n");
    }
    this->setTextCursor(cur);
}

bool LiteEditorWidgetBase::findPrevBlock(QTextCursor &cursor, int indent, const QString &skip) const
{
    QTextBlock block = cursor.block().previous();
    while(block.isValid()) {
        TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
        if (data && data->foldingIndent() == indent) {
            QString text = block.text().trimmed();
            if (text.isEmpty() || text.startsWith(skip)) {
                block = block.previous();
                continue;
            }
            cursor.setPosition(block.position());
            return true;
        }
        block = block.previous();
    }
    return false;
}

bool LiteEditorWidgetBase::findStartBlock(QTextCursor &cursor, int indent) const
{
    QTextBlock block = cursor.block();
    while(block.isValid()) {
        TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
        if (data && data->foldingIndent() == indent) {
            cursor.setPosition(block.position());
            return true;
        }
        block = block.previous();
    }
    return false;
}


bool LiteEditorWidgetBase::findEndBlock(QTextCursor &cursor, int indent) const
{
    QTextBlock block = cursor.block().next();
    while(block.isValid()) {
        TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
        if (data && data->foldingIndent() == indent) {
            cursor.setPosition(block.previous().position());
            return true;
        }
        block = block.next();
    }
    return false;
}

bool LiteEditorWidgetBase::findNextBlock(QTextCursor &cursor, int indent, const QString &skip) const
{
    QTextBlock block = cursor.block().next();
    while(block.isValid()) {
        TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
        if (data && data->foldingIndent() == indent) {
            QString text = block.text().trimmed();
            if (text.isEmpty() || text.startsWith(skip)) {
                block = block.next();
                continue;
            }
            cursor.setPosition(block.position());
            return true;
        }
        block = block.next();
    }
    return false;
}


void LiteEditorWidgetBase::gotoPrevBlock()
{
    QTextCursor cursor = this->textCursor();
    if (!findPrevBlock(cursor,0)) {
        cursor.movePosition(QTextCursor::Start);
    }
    this->setTextCursor(cursor);
}

void LiteEditorWidgetBase::gotoNextBlock()
{
    QTextCursor cursor = this->textCursor();
    if (!findNextBlock(cursor,0)) {
        cursor.movePosition(QTextCursor::End);
    }
    this->setTextCursor(cursor);
}

void LiteEditorWidgetBase::selectBlock()
{
    QTextCursor cursor = this->textCursor();
    if (!findStartBlock(cursor,0)) {
        return;
    }
    QTextCursor end = this->textCursor();
    if (!findEndBlock(end,0)) {
        return;
    }
    cursor.setPosition(end.position()+end.block().length()-1,QTextCursor::KeepAnchor);
    this->setTextCursor(cursor);
}

bool LiteEditorWidgetBase::event(QEvent *e)
{
    m_contentsChanged = false;
    return QPlainTextEdit::event(e);
}

QString LiteEditorWidgetBase::autoCompleteSurroundText(QTextCursor &cursor, const QString &textToInsert) const
{
    if (textToInsert == QLatin1String("("))
        return cursor.selectedText() + QLatin1Char(')');
    if (textToInsert == QLatin1String("{")) {
        //If the text span multiple lines, insert on different lines
        QString str = cursor.selectedText();
        if (str.contains(QChar::ParagraphSeparator)) {
            //Also, try to simulate auto-indent
            str = (str.startsWith(QChar::ParagraphSeparator) ? QString() : QString(QChar::ParagraphSeparator)) +
                  str;
            if (str.endsWith(QChar::ParagraphSeparator))
                str += QLatin1Char('}') + QString(QChar::ParagraphSeparator);
            else
                str += QString(QChar::ParagraphSeparator) + QLatin1Char('}');
        } else {
            str += QLatin1Char('}');
        }
        return str;
    }
    if (textToInsert == QLatin1String("["))
        return cursor.selectedText() + QLatin1Char(']');
    if (textToInsert == QLatin1String("\""))
        return cursor.selectedText() + QLatin1Char('"');
    if (textToInsert == QLatin1String("'"))
        return cursor.selectedText() + QLatin1Char('\'');
    return "";
}

static void countBracket(QChar open, QChar close, QChar c, int *errors, int *stillopen)
{
    if (c == open)
        ++*stillopen;
    else if (c == close)
        --*stillopen;

    if (*stillopen < 0) {
        *errors += -1 * (*stillopen);
        *stillopen = 0;
    }
}

static void countBrackets(QTextCursor cursor,
                          int from,
                          int end,
                          QChar open,
                          QChar close,
                          int *errors,
                          int *stillopen)
{
    cursor.setPosition(from);
    QTextBlock block = cursor.block();
    while (block.isValid() && block.position() < end) {
        TextEditor::Parentheses parenList = TextEditor::BaseTextDocumentLayout::parentheses(block);
        if (!parenList.isEmpty() && !TextEditor::BaseTextDocumentLayout::ifdefedOut(block)) {
            for (int i = 0; i < parenList.count(); ++i) {
                TextEditor::Parenthesis paren = parenList.at(i);
                int position = block.position() + paren.pos;
                if (position < from || position >= end)
                    continue;
                countBracket(open, close, paren.chr, errors, stillopen);
            }
        }
        block = block.next();
    }
}


bool LiteEditorWidgetBase::checkIsMatchBraces(QTextCursor &cursor, const QString &textToInsert) const
{
    const QChar character = textToInsert.at(0);
    const QString parentheses1 = QLatin1String("()");
    const QString parentheses2 = QLatin1String("[]");
    const QString parentheses3 = QLatin1String("{}");
    if (parentheses1.contains(character) ||
            parentheses2.contains(character) ||
            parentheses3.contains(character) ) {
        QTextCursor tmp= cursor;
        bool foundBlockStart = TextEditor::TextBlockUserData::findPreviousBlockOpenParenthesis(&tmp);
        int blockStart = foundBlockStart ? tmp.position() : 0;
        tmp = cursor;
        bool foundBlockEnd = TextEditor::TextBlockUserData::findNextBlockClosingParenthesis(&tmp);
        int blockEnd = foundBlockEnd ? tmp.position() : (cursor.document()->characterCount() - 1);
        QChar openChar;
        QChar closeChar;
        if (parentheses1.contains(character)) {
            openChar = QLatin1Char('(');
            closeChar = QLatin1Char(')');
        } else {
            if (parentheses2.contains(character)) {
                openChar = QLatin1Char('[');
                closeChar = QLatin1Char(']');
            } else if (parentheses3.contains(character)) {
                openChar = QLatin1Char('{');
                closeChar = QLatin1Char('}');
            }
            if (cursor.document()->blockCount() < 5000) {
                blockStart = 0;
                blockEnd = cursor.document()->characterCount()-1;
            } else {
                blockStart = cursor.block().position();
                blockEnd = cursor.block().position()+cursor.block().length();
            }
        }
        int errors = 0;
        int stillopen = 0;
        countBrackets(cursor, blockStart, blockEnd, openChar, closeChar, &errors, &stillopen);
        int errorsBeforeInsertion = errors + stillopen;
        errors = 0;
        stillopen = 0;
        countBrackets(cursor, blockStart, cursor.position(), openChar, closeChar, &errors, &stillopen);
        countBracket(openChar, closeChar, character, &errors, &stillopen);
        countBrackets(cursor, cursor.position(), blockEnd, openChar, closeChar, &errors, &stillopen);
        int errorsAfterInsertion = errors + stillopen;
        if (errorsAfterInsertion < errorsBeforeInsertion)
            return false; // insertion fixes parentheses or bracket errors, do not auto complete
    }
    return true;
}

bool LiteEditorWidgetBase::autoBackspace(QTextCursor &cursor)
{
    int pos = cursor.position();
    if (pos == 0)
        return false;

    if (this->m_textLexer->isInComment(cursor)) {
        return false;
    }

    QTextCursor c = cursor;
    c.setPosition(pos - 1);

    QTextDocument *doc = cursor.document();
    const QChar lookAhead = doc->characterAt(pos);
    const QChar lookBehind = doc->characterAt(pos - 1);
    //const QChar lookFurtherBehind = doc->characterAt(pos - 2);

    if ( (lookBehind == QLatin1Char('(') && lookAhead == QLatin1Char(')')) ||
         (lookBehind == QLatin1Char('[') && lookAhead == QLatin1Char(']')) ||
         (lookBehind == QLatin1Char('{') && lookAhead == QLatin1Char('}')) ) {
        if (m_textLexer->isInString(cursor)) {
            return false;
        }
    } else if ( (lookBehind == QLatin1Char('\"') && lookAhead == QLatin1Char('\"')) ||
               (lookBehind == QLatin1Char('\'') && lookAhead == QLatin1Char('\'')) ||
                (lookBehind == QLatin1Char('`') && lookAhead == QLatin1Char('`'))) {
        if (!this->m_textLexer->isLangSupport()) {
            return false;
        }
        if(!m_textLexer->isInEmptyString(cursor)) {
             return false;
        }
    } else {
        return false;
    }

    const QChar character = lookBehind;
    if (character == QLatin1Char('(') ||
            character == QLatin1Char('[') ||
            character == QLatin1Char('{') ) {

        QTextCursor tmp = cursor;
        TextEditor::TextBlockUserData::findPreviousBlockOpenParenthesis(&tmp);
        int blockStart = tmp.isNull() ? 0 : tmp.position();
        tmp = cursor;
        TextEditor::TextBlockUserData::findNextBlockClosingParenthesis(&tmp);
        int blockEnd = tmp.isNull() ? (cursor.document()->characterCount()-1) : tmp.position();
        QChar openChar = character;
        QChar closeChar;
        if (openChar == QLatin1Char('(')) {
            closeChar = ')';
        } else  {
            if (openChar == QLatin1Char('[')) {
                closeChar = ']';
            } else {
                closeChar = '}';
            }
            if (cursor.document()->blockCount() < 5000) {
                blockStart = 0;
                blockEnd = cursor.document()->characterCount()-1;
            } else {
                blockStart = cursor.block().position();
                blockEnd = cursor.block().position()+cursor.block().length();
            }
        }

        int errors = 0;
        int stillopen = 0;
        countBrackets(cursor, blockStart, blockEnd, openChar, closeChar, &errors, &stillopen);
        int errorsBeforeDeletion = errors + stillopen;
        errors = 0;
        stillopen = 0;
        countBrackets(cursor, blockStart, pos - 1, openChar, closeChar, &errors, &stillopen);
        countBrackets(cursor, pos, blockEnd, openChar, closeChar, &errors, &stillopen);
        int errorsAfterDeletion = errors + stillopen;
        if (errorsAfterDeletion < errorsBeforeDeletion)
            return false; // insertion fixes parentheses or bracket errors, do not auto complete
    }


//    if    ((lookBehind == QLatin1Char('(') && lookAhead == QLatin1Char(')'))
//        || (lookBehind == QLatin1Char('[') && lookAhead == QLatin1Char(']'))
//        || (lookBehind == QLatin1Char('`') && lookAhead == QLatin1Char('`'))
//        || (lookBehind == QLatin1Char('"') && lookAhead == QLatin1Char('"')
//            && lookFurtherBehind != QLatin1Char('\\'))
//        || (lookBehind == QLatin1Char('\'') && lookAhead == QLatin1Char('\'')
//            && lookFurtherBehind != QLatin1Char('\\'))) {
         cursor.beginEditBlock();
         cursor.deleteChar();
         cursor.deletePreviousChar();
         cursor.endEditBlock();
         return true;
//    }
//    return false;
}

void LiteEditorWidgetBase::handleBackspaceKey()
{
}

void LiteEditorWidgetBase::setVisualizeWhitespace(bool b)
{
    m_visualizeWhitespace = b;
}

void LiteEditorWidgetBase::autoIndent()
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    ::autoIndent(this->document(),cursor,this->tabSettings());
    cursor.endEditBlock();
}

void LiteEditorWidgetBase::keyPressEvent(QKeyEvent *e)
{
    if ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_S) {
        return;
    }
    if (e->key() == Qt::Key_Insert ) {
        if (e->modifiers() == Qt::NoModifier) {
            this->setOverwriteMode(!this->overwriteMode());
            emit overwriteModeChanged(this->overwriteMode());
            return;
        } else if (e->modifiers() == Qt::ControlModifier) {
            this->copyLine();
            return;
        }
        QPlainTextEdit::keyPressEvent(e);
        return;
    }
    QToolTip::hideText();
    this->m_moveLineUndoHack = false;
    bool ro = isReadOnly();

    if (m_inBlockSelectionMode) {
        if (e == QKeySequence::Cut) {
            if (!ro) {
                cut();
                e->accept();
                return;
            }
        } else if (e == QKeySequence::Delete || e->key() == Qt::Key_Backspace) {
            if (!ro) {
                removeBlockSelection();
                e->accept();
                return;
            }
        } else if (e == QKeySequence::Paste) {
            if (!ro) {
                paste();
                e->accept();
                return;
            }
        } else if (e == QKeySequence::SelectAll) {
            selectAll();
            e->accept();
            return;
        }
    }

    if ( e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return ) {
        if (e->modifiers() == Qt::NoModifier) {
            if (m_autoIndent) {
                indentEnter(textCursor());
                return;
            }
        } else if (e->modifiers() & Qt::ControlModifier) {
            if (e->modifiers() & Qt::ShiftModifier) {
                insertLineBefore();
            } else {
                insertLineAfter();
            }
            return;
        } else if (e->modifiers() & Qt::ShiftModifier) {
            QTextCursor cur = this->textCursor();
            if (m_autoIndent) {
                indentEnter(cur);
            } else {
                cur.insertText("\n");
            }
            this->setTextCursor(cur);
            return;
        }
    }
    if (e == QKeySequence::MoveToStartOfBlock
            || e == QKeySequence::SelectStartOfBlock){
        if ((e->modifiers() & (Qt::AltModifier | Qt::ShiftModifier)) == (Qt::AltModifier | Qt::ShiftModifier)) {
            e->accept();
            return;
        }
        handleHomeKey(e == QKeySequence::SelectStartOfBlock);
        e->accept();
    } else if (e == QKeySequence::MoveToStartOfLine
               || e == QKeySequence::SelectStartOfLine){
        if ((e->modifiers() & (Qt::AltModifier | Qt::ShiftModifier)) == (Qt::AltModifier | Qt::ShiftModifier)) {
            e->accept();
            return;
        }
        QTextCursor cursor = textCursor();
        if (QTextLayout *layout = cursor.block().layout()) {
            if (layout->lineForTextPosition(cursor.position() - cursor.block().position()).lineNumber() == 0) {
                handleHomeKey(e == QKeySequence::SelectStartOfLine);
                e->accept();
                return;
            }
        }
    } else if (!ro && e == QKeySequence::DeleteStartOfWord && !textCursor().hasSelection()) {
        e->accept();
        QTextCursor c = textCursor();
        c.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
        c.removeSelectedText();
        return;
    } else if (!ro && e == QKeySequence::DeleteEndOfWord && !textCursor().hasSelection()) {
        e->accept();
        QTextCursor c = textCursor();
        c.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
        c.removeSelectedText();
        return;
    } else {
        switch (e->key()) {
        case Qt::Key_Tab:
        case Qt::Key_Backtab: {
            if (ro) break;
            QTextCursor cursor = textCursor();
            indentText(cursor, e->key() == Qt::Key_Tab);
            e->accept();
            return;
        }
        case Qt::Key_Backspace: {
            QTextCursor cursor = this->textCursor();
            if ((e->modifiers() & (Qt::ControlModifier
                                   | Qt::ShiftModifier
                                   | Qt::AltModifier
                                   | Qt::MetaModifier)) == Qt::NoModifier
                && !cursor.hasSelection()) {
                if (this->autoBackspace(cursor)) {
                    this->setTextCursor(cursor);
                    e->accept();
                    return;
                }
            }
            break;
        }
        case Qt::Key_Up:
        case Qt::Key_Down:
            if ((e->modifiers() & Qt::ControlModifier) && (e->modifiers()&Qt::ShiftModifier)) {
                this->moveLineUpDown(e->key() == Qt::Key_Up);
                return;
            }
            if (e->modifiers() & Qt::ControlModifier) {
                verticalScrollBar()->triggerAction(
                        e->key() == Qt::Key_Up ? QAbstractSlider::SliderSingleStepSub :
                                                 QAbstractSlider::SliderSingleStepAdd);
                e->accept();
                return;
            }
        case Qt::Key_Right:
        case Qt::Key_Left:
#ifdef Q_OS_MAC
            break;
#endif
            if ((e->modifiers()
                 & (Qt::AltModifier | Qt::ShiftModifier)) == (Qt::AltModifier | Qt::ShiftModifier)) {
                int diff_row = 0;
                int diff_col = 0;
                if (e->key() == Qt::Key_Up)
                    diff_row = -1;
                else if (e->key() == Qt::Key_Down)
                    diff_row = 1;
                else if (e->key() == Qt::Key_Left)
                    diff_col = -1;
                else if (e->key() == Qt::Key_Right)
                    diff_col = 1;
                handleBlockSelection(diff_row, diff_col);
                e->accept();
                return;
            } else {
                // leave block selection mode
                if (m_inBlockSelectionMode) {
                    m_inBlockSelectionMode = false;
                    m_blockSelection.clear();
                    viewport()->update();
                }
            }
            break;
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            if (e->modifiers() == Qt::ControlModifier) {
                verticalScrollBar()->triggerAction(
                        e->key() == Qt::Key_PageUp ? QAbstractSlider::SliderPageStepSub :
                                                     QAbstractSlider::SliderPageStepAdd);
                e->accept();
                return;
            }
            break;
        default:
            break;
        }
    }

    if (!ro && m_inBlockSelectionMode) {
        QString text = e->text();
        if (!text.isEmpty() && (text.at(0).isPrint() || text.at(0) == QLatin1Char('\t'))) {
            removeBlockSelection();
        }
    }

    if (((e->modifiers() & (Qt::ControlModifier|Qt::AltModifier)) != Qt::ControlModifier) &&
            m_textLexer->isLangSupport() && m_textLexer->isEndOfString(this->textCursor())) {
        QString keyText = e->text();
        if (keyText == "\"" || keyText == "\'" || keyText == "`") {
            QTextCursor cursor = textCursor();
            if (!cursor.atBlockEnd()) {
                QString text = cursor.block().text();
                if (text.mid(cursor.positionInBlock(),1) == keyText) {
                    if (this->checkIsMatchBraces(cursor,keyText)) {
                        cursor.movePosition(QTextCursor::Right);
                        setTextCursor(cursor);
                        return;
                    }
                }
            }
        }
    }


    if (((e->modifiers() & (Qt::ControlModifier|Qt::AltModifier)) != Qt::ControlModifier) &&
            (m_bLastBraces ||m_textLexer->isCanAutoCompleter(this->textCursor())) ) {        
        if (m_bLastBraces) {
            if (e->text() == m_lastBraceText) {
                QTextCursor cursor = textCursor();
                cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor);
                setTextCursor(cursor);
                m_bLastBraces = false;
                return;
            }
        }

        if (textCursor().hasSelection()) {
            QPlainTextEdit::keyPressEvent(e);
            return;
        }
        m_bLastBraces = false;
        QString keyText = e->text();
        QString mr;
        if (m_autoBraces0 && keyText == "{") {
            mr = "}";
        } else if (m_autoBraces1 && keyText == "(") {
            mr = ")";
        } else if (m_autoBraces2 && keyText == "[") {
            mr = "]";
        } else if (m_autoBraces3 && keyText == "\'") {
            mr = "\'";
        } else if (m_autoBraces4 && keyText == "\"") {
            mr = "\"";
        } else if (m_autoBraces5 && keyText == "`") {
            if (m_mimeType == "text/x-gosrc") {
                mr = "`";
            }
        }
        if (m_textLexer->isInStringOrComment(this->textCursor())) {
            QPlainTextEdit::keyPressEvent(e);
            return;
        }

        if (keyText == ")" || keyText == "]" || keyText == "}") {
            QTextCursor cursor = textCursor();
            if (!cursor.atBlockEnd()) {
                QString text = cursor.block().text();
                if (text.mid(cursor.positionInBlock(),1) == keyText) {
                    if (this->checkIsMatchBraces(cursor,keyText)) {
                        cursor.movePosition(QTextCursor::Right);
                        setTextCursor(cursor);
                        return;
                    }
                }
            }
        } else if (keyText == "(" || keyText == "[" || keyText == "{") {
            QTextCursor cursor = textCursor();
            if (!cursor.atBlockEnd()) {
                QString text = cursor.block().text();
                if (text.mid(cursor.positionInBlock(),1) == keyText) {
                    if (this->checkIsMatchBraces(cursor,keyText)) {
                        cursor.movePosition(QTextCursor::Right);
                        setTextCursor(cursor);
                        return;
                    }
                } else if (text.at(cursor.positionInBlock()).isLetterOrNumber()) {
                     QPlainTextEdit::keyPressEvent(e);
                     return;
                }
            }
        }
        if (!mr.isNull()) {
            QTextCursor cursor = textCursor();
            if (this->checkIsMatchBraces(cursor,keyText)) {
                cursor.beginEditBlock();
                int pos = cursor.position();
                if (cursor.hasSelection()) {
                    pos = cursor.selectionStart();
                }
                cursor.insertText(keyText+mr);
                cursor.setPosition(pos+1);
                cursor.endEditBlock();
                setTextCursor(cursor);
                m_bLastBraces = true;
                m_lastBraceText = mr;
                return;
            }
        }
    }

    QPlainTextEdit::keyPressEvent(e);
}

void LiteEditorWidgetBase::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Control) {
        clearLink();
    }
    QPlainTextEdit::keyReleaseEvent(e);
}

void LiteEditorWidgetBase::leaveEvent(QEvent *e)
{
    clearLink();
    QPlainTextEdit::leaveEvent(e);
}

void LiteEditorWidgetBase::indentBlock(QTextBlock block, bool bIndent)
{
    QTextCursor cursor(block);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.removeSelectedText();
    if (bIndent) {
        cursor.insertText(this->tabText());
    } else {
        QString text = block.text();
        if (!text.isEmpty()) {
            if (text.at(0) == '\t') {
                cursor.deleteChar();
            } else if (m_bTabUseSpace && text.startsWith(QString(m_nTabSize,' '))) {
                int count = m_nTabSize;
                while (count--) {
                    cursor.deleteChar();
                }
            } else if (text.at(0) == ' ') {
                cursor.deleteChar();
            }
        }
    }
    cursor.endEditBlock();
}

void LiteEditorWidgetBase::indentCursor(QTextCursor cur, bool bIndent)
{
   cur.beginEditBlock();
    if (bIndent) {
        cur.insertText(this->tabText());
    } else {         
        QString text = cur.block().text();
        int pos = cur.positionInBlock()-1;
        if (pos >= 0) {
            if (text.at(pos) == '\t') {
                cur.deletePreviousChar();
            } else if (m_bTabUseSpace &&
                       (pos-m_nTabSize+1 >= 0) &&
                       (text.mid(pos-m_nTabSize+1,m_nTabSize) == QString(m_nTabSize,' '))) {
                int count = m_nTabSize;
                while (count--) {
                    cur.deletePreviousChar();
                }
            } else if (text.at(pos) == ' ') {
                cur.deletePreviousChar();
            }
       }
    }
    cur.endEditBlock();
}

void LiteEditorWidgetBase::indentText(QTextCursor cur,bool bIndent)
{
    QTextDocument *doc = document();
    cur.beginEditBlock();
    if (!cur.hasSelection()) {
        indentCursor(cur,bIndent);
    } else {
        QTextBlock block = doc->findBlock(cur.selectionStart());
        QTextBlock end = doc->findBlock(cur.selectionEnd());
        if (end.position() == cur.selectionEnd()) {
            end = end.previous();
        }
        if (block == end && cur.selectionStart() != block.position() ) {
            cur.removeSelectedText();
            //indentCursor(cur,bIndent);
            if (bIndent) {
                cur.insertText(this->tabText());
            }
            goto end;
        }
        bool bResetPos = bIndent && cur.selectionStart() == block.position();
        bool bStart = cur.position() == cur.selectionStart();
        int startPos = cur.selectionStart();

        do {
            indentBlock(block,bIndent);
            block = block.next();
        } while (block.isValid() && block.position() <= end.position());
        int endPos = cur.selectionEnd();
        if (bResetPos) {
            if (bStart) {
                cur.setPosition(endPos);
                cur.movePosition(QTextCursor::Left,QTextCursor::KeepAnchor,endPos-startPos);
            } else {
                cur.setPosition(startPos);
                cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,endPos-startPos);
            }
        }
    }
end:
    cur.endEditBlock();
    setTextCursor(cur);
}

QString LiteEditorWidgetBase::tabText(int n) const
{
    if (m_bTabUseSpace) {
        return QString(m_nTabSize*n,' ');
    }
    return QString(n,'\t');
}

void LiteEditorWidgetBase::indentEnter(QTextCursor cur)
{
    QTextBlock block = cur.block();
    if (block.isValid() && block.next().isValid() && !block.next().isVisible()) {
        unfold();
    }
    int pos = cur.position()-cur.block().position();
    QString text = cur.block().text();
    int i = 0;
    int tab = 0;
    int space = 0;
    QString inText = "\n";
    while (i < pos) {
        if (!text.at(i).isSpace())
            break;
        if (text.at(i) == ' ') {
            space++;
        } else if (text.at(i) == '\t') {
            tab++;
        }
        i++;
    }
    tab += space/m_nTabSize;
    inText += this->tabText(tab);
    if (!text.isEmpty()) {
        if (pos >= text.size()) {
            const QChar ch = text.at(text.size()-1);
            if (ch == '{' || ch == '(') {
                inText += this->tabText();
            }
        } else if (pos >= 1 && pos < text.size()) {
            const QChar l = text.at(pos-1);
            const QChar r = text.at(pos);
            if ( (l == '{' && r == '}') ||
                 (l == '(' && r== ')') ) {
                cur.beginEditBlock();
                cur.insertText(inText);
                int pos = cur.position();
                cur.insertText(inText);
                cur.setPosition(pos);
                this->setTextCursor(cur);
                cur.insertText(this->tabText());
                cur.endEditBlock();
                return;
            }
        }
    }
    cur.beginEditBlock();
    cur.insertText(inText);
    cur.endEditBlock();
    ensureCursorVisible();
}

static QString simpleInfo(const QString &info, int maxLine)
{
    QStringList lines = info.split("\n");
    if (lines.size() <= maxLine) {
        return info;
    }
    QStringList out;
    for (int i = 0; i < maxLine; i++) {
        out += lines[i];
    }
    return out.join("\n")+"\n...";
}

void LiteEditorWidgetBase::showToolTipInfo(const QPoint &pos, const QString &text)
{
    QToolTip::showText(pos,simpleInfo(text,m_maxTipInfoLines),this);
}

void LiteEditorWidgetBase::cleanWhitespace(bool wholeDocument)
{
    QTextCursor cursor = this->textCursor();
    bool hasSelection = cursor.hasSelection();
    QTextCursor copyCursor = cursor;
    copyCursor.setVisualNavigation(false);
    if (wholeDocument) {
        copyCursor.select(QTextCursor::Document);
    }
    copyCursor.beginEditBlock();
    cleanWhitespace(copyCursor, true);
    if (!hasSelection)
        ensureFinalNewLine(copyCursor);
    copyCursor.endEditBlock();
}

static int trailingWhitespaces(const QString &text)
{
    int i = 0;
    while (i < text.size()) {
        if (!text.at(text.size()-1-i).isSpace())
            return i;
        ++i;
    }
    return i;
}

void LiteEditorWidgetBase::cleanWhitespace(QTextCursor &cursor, bool inEntireDocument)
{
    QTextDocument *document = this->document();
    TextEditor::BaseTextDocumentLayout *documentLayout = qobject_cast<TextEditor::BaseTextDocumentLayout*>(document->documentLayout());
    Q_ASSERT(cursor.visualNavigation() == false);

    QTextBlock block = document->findBlock(cursor.selectionStart());
    QTextBlock end;
    if (cursor.hasSelection())
        end = document->findBlock(cursor.selectionEnd()-1).next();

    while (block.isValid() && block != end) {
        if (inEntireDocument || block.revision() != documentLayout->lastSaveRevision) {
            QString blockText = block.text();
            if (int trailing = trailingWhitespaces(blockText)) {
                cursor.setPosition(block.position() + block.length() - 1);
                cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, trailing);
                cursor.removeSelectedText();
            }
        }

        block = block.next();
    }
}

void LiteEditorWidgetBase::ensureFinalNewLine(QTextCursor &cursor)
{
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    bool emptyFile = !cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

    if (!emptyFile && cursor.selectedText().at(0) != QChar::ParagraphSeparator)
    {
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        cursor.insertText(QLatin1String("\n"));
    }
}

void LiteEditorWidgetBase::setNavigateHead(LiteApi::EditorNaviagteType type, const QString &msg)
{
    m_navigateManager->setType(type,msg);
    m_navigateArea->update();
}

void LiteEditorWidgetBase::insertNavigateMark(int line, LiteApi::EditorNaviagteType type, const QString &msg, const char* tag = "")
{
    m_navigateManager->insertMark(line,msg,type,tag);
    m_navigateArea->update();
}

void LiteEditorWidgetBase::clearAllNavigateMark(LiteApi::EditorNaviagteType types, const char *tag = "")
{
    m_navigateManager->clearAllNavigateMark(types, tag);
}

void LiteEditorWidgetBase::clearAllNavigateMarks()
{
    m_navigateManager->clearAll();
}

void LiteEditorWidgetBase::moveCursorVisible(bool ensureVisible)
{
    QTextCursor cursor = this->textCursor();
    if (!cursor.block().isVisible()) {
        cursor.setVisualNavigation(true);
        cursor.movePosition(QTextCursor::Up);
        this->setTextCursor(cursor);
    }
    if (ensureVisible)
        this->ensureCursorVisible();
}

void LiteEditorWidgetBase::toggleBlockVisible(const QTextBlock &block)
{
    TextEditor::BaseTextDocumentLayout *documentLayout = qobject_cast<TextEditor::BaseTextDocumentLayout*>(document()->documentLayout());

    bool visible = block.next().isVisible();
    TextEditor::BaseTextDocumentLayout::doFoldOrUnfold(block, !visible);
    documentLayout->requestUpdate();
    documentLayout->emitDocumentSizeChanged();
}

void LiteEditorWidgetBase::foldIndentChanged(QTextBlock block)
{
    if (!block.isVisible()) {
        QTextDocument *doc = document();
        TextEditor::BaseTextDocumentLayout *documentLayout = qobject_cast<TextEditor::BaseTextDocumentLayout*>(doc->documentLayout());
        block.setVisible(true);
        documentLayout->requestUpdate();
    }
}

void LiteEditorWidgetBase::updateBlock(QTextBlock)
{

}

void LiteEditorWidgetBase::documentSizeChanged(QSizeF)
{

}

void LiteEditorWidgetBase::fold()
{
    QTextDocument *doc = document();
    TextEditor::BaseTextDocumentLayout *documentLayout = qobject_cast<TextEditor::BaseTextDocumentLayout*>(doc->documentLayout());
    QTextBlock block = textCursor().block();
    if (!(TextEditor::BaseTextDocumentLayout::canFold(block) && block.next().isVisible())) {
        // find the closest previous block which can fold
        int indent = TextEditor::BaseTextDocumentLayout::foldingIndent(block);
        while (block.isValid() && (TextEditor::BaseTextDocumentLayout::foldingIndent(block) >= indent || !block.isVisible()))
            block = block.previous();
    }
    if (block.isValid()) {
        TextEditor::BaseTextDocumentLayout::doFoldOrUnfold(block, false);
        this->moveCursorVisible(true);
        documentLayout->requestUpdate();
        documentLayout->emitDocumentSizeChanged();
    }
}

void LiteEditorWidgetBase::unfold()
{
    QTextDocument *doc = document();
    TextEditor::BaseTextDocumentLayout *documentLayout = qobject_cast<TextEditor::BaseTextDocumentLayout*>(doc->documentLayout());

    QTextBlock block = textCursor().block();
    while (block.isValid() && !block.isVisible())
        block = block.previous();
    TextEditor::BaseTextDocumentLayout::doFoldOrUnfold(block, true);
    this->moveCursorVisible(true);
    documentLayout->requestUpdate();
    documentLayout->emitDocumentSizeChanged();
}

void LiteEditorWidgetBase::foldAll()
{
    QTextDocument *doc = document();
    TextEditor::BaseTextDocumentLayout *documentLayout = qobject_cast<TextEditor::BaseTextDocumentLayout*>(doc->documentLayout());

    QTextBlock block = doc->firstBlock();

    while (block.isValid()) {
        if (TextEditor::BaseTextDocumentLayout::canFold(block))
            TextEditor::BaseTextDocumentLayout::doFoldOrUnfold(block, false);
        block = block.next();
    }

    moveCursorVisible(true);
    documentLayout->requestUpdate();
    documentLayout->emitDocumentSizeChanged();
    centerCursor();
}


void LiteEditorWidgetBase::unfoldAll()
{
    QTextDocument *doc = document();
    TextEditor::BaseTextDocumentLayout *documentLayout = qobject_cast<TextEditor::BaseTextDocumentLayout*>(doc->documentLayout());

    QTextBlock block = doc->firstBlock();
    while (block.isValid()) {
        if (TextEditor::BaseTextDocumentLayout::canFold(block))
            TextEditor::BaseTextDocumentLayout::doFoldOrUnfold(block, true);
        block = block.next();
    }

    moveCursorVisible(true);
    documentLayout->requestUpdate();
    documentLayout->emitDocumentSizeChanged();
    centerCursor();
}

QTextBlock LiteEditorWidgetBase::foldedBlockAt(const QPoint &pos, QRect *box) const
{
    QPointF offset(contentOffset());
    QTextBlock block = firstVisibleBlock();
    qreal top = blockBoundingGeometry(block).translated(offset).top();
    qreal bottom = top + blockBoundingRect(block).height();

    int viewportHeight = viewport()->height();

    while (block.isValid() && top <= viewportHeight) {
        QTextBlock nextBlock = block.next();
        if (block.isVisible() && bottom >= 0) {
            if (nextBlock.isValid() && !nextBlock.isVisible()) {
                QTextLayout *layout = block.layout();
                QTextLine line = layout->lineAt(layout->lineCount()-1);
                QRectF lineRect = line.naturalTextRect().translated(offset.x(), top);
                lineRect.adjust(0, 0, -1, -1);

                QRectF collapseRect(lineRect.right() + 12,
                                    lineRect.top(),
                                    fontMetrics().width(QLatin1String(" {...}; ")),
                                    lineRect.height());
                if (collapseRect.contains(pos)) {
                    QTextBlock result = block;
                    if (box)
                        *box = collapseRect.toAlignedRect();
                    return result;
                } else {
                    block = nextBlock;
                    while (nextBlock.isValid() && !nextBlock.isVisible()) {
                        block = nextBlock;
                        nextBlock = block.next();
                    }
                }
            }
        }

        block = nextBlock;
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
    }
    return QTextBlock();
}

void LiteEditorWidgetBase::uplinkDeployTimeout()
{
    m_lastUpToolTipPos = m_upToolTipPos;
    m_upToolTipTimer->start(m_upToolTipTime+100);
}

void LiteEditorWidgetBase::uplinkInfoTimeout()
{
    if (m_lastUpToolTipPos != m_upToolTipPos) {
        QToolTip::hideText();
        return;
    }
    QTextCursor cursor = cursorForPosition(m_upToolTipPos);
    bool findLink = false;
    if (!cursor.isNull()) {
        int pos = cursor.position();
        QRect rc = this->cursorRect(cursor);
        LiteApi::selectWordUnderCursor(cursor);
        if (cursor.hasSelection()) {
            rc.setLeft(rc.left()-(pos-cursor.selectionStart())*m_averageCharWidth);
            rc.setRight(rc.right()+(cursor.selectionEnd()-pos)*m_averageCharWidth);
            if (rc.contains(m_upToolTipPos)) {
                findLink = true;
                m_showLinkInfomation = true;
                QToolTip::hideText();
                emit updateLink(cursor,m_upToolTipPos,false);
            }
        }
    }
    if (!findLink) {
        QToolTip::hideText();
    }
}

void LiteEditorWidgetBase::stopUplinkTimer()
{
    m_showLinkInfomation = false;
    QToolTip::hideText();
    m_upToolTipTimer->stop();
    m_upToolTipDeployTimer->stop();
}

bool LiteEditorWidgetBase::isSpellCheckingAt(QTextCursor cur) const
{
    QTextBlock block = cur.block();
    if (!block.userData()) {
        return false;
    }
    TextEditor::TextBlockUserData *data = static_cast<TextEditor::TextBlockUserData*>(block.userData());
    return data->shouldSpellCheck(cur.positionInBlock());
}


void LiteEditorWidgetBase::showLink(const LiteApi::Link &link)
{
    if (link.showNav && !link.sourceInfo.isEmpty()) {
        QPoint pt = this->mapToGlobal(link.cursorPos);
        this->showToolTipInfo(pt,link.sourceInfo);

    } else if (link.showTip && !link.targetInfo.isEmpty()) {
        QPoint pt = this->mapToGlobal(link.cursorPos);
        this->showToolTipInfo(pt,link.targetInfo);
    }

    if (!link.showNav) {
        return;
    }

    if (!m_showLinkNavigation) {
        return;
    }

    if (m_currentLink == link) {
        return;
    }

    if (link.targetFileName.isEmpty()) {
        clearLink();
        return;
    }

    QTextEdit::ExtraSelection sel;
    sel.cursor = textCursor();
    sel.cursor.setPosition(link.linkTextStart);
    sel.cursor.setPosition(link.linkTextEnd, QTextCursor::KeepAnchor);
    sel.format.setForeground(Qt::blue);
    sel.format.setFontUnderline(true);
    setExtraSelections(LiteApi::LinkSelection,QList<QTextEdit::ExtraSelection>() << sel);
    viewport()->setCursor(Qt::PointingHandCursor);

    m_currentLink = link;
    m_linkPressed = false;
}

void LiteEditorWidgetBase::clearLink()
{
    m_showLinkNavigation = false;
    m_linkPressed = false;
    if (!m_currentLink.hasValidLinkText())
        return;

    setExtraSelections(LiteApi::LinkSelection, QList<QTextEdit::ExtraSelection>());
    viewport()->setCursor(Qt::IBeamCursor);
    m_currentLink = LiteApi::Link();
    QToolTip::hideText();
}

bool LiteEditorWidgetBase::openLink(const LiteApi::Link &link)
{
    if (!link.hasValidTarget()) {
        return false;
    }            
    LiteApi::gotoLine(m_liteApp,link.targetFileName,link.targetLine,link.targetColumn,true,true);
    return true;
}

void LiteEditorWidgetBase::setExtraSelections(LiteApi::ExtraSelectionKind kind, const QList<QTextEdit::ExtraSelection> &selections)
{
    m_extralSelectionMap[kind] = selections;
    QList<QTextEdit::ExtraSelection> all;
    QMapIterator<LiteApi::ExtraSelectionKind,QList<QTextEdit::ExtraSelection> > i(m_extralSelectionMap);
    while(i.hasNext()) {
        i.next();
        all += i.value();
    }
    QPlainTextEdit::setExtraSelections(all);
}

QRectF LiteEditorWidgetBase::selectionRect(const QTextCursor &cursor)
{
    QRectF r;
    if (!cursor.hasSelection()) {
        r = this->cursorRect(cursor);
    } else {
        QTextDocument *doc = this->document();
        const int position = cursor.selectionStart();
        const int anchor = cursor.selectionEnd();
        const QTextBlock posBlock = doc->findBlock(position);
        const QTextBlock anchorBlock = doc->findBlock(anchor);
        if (posBlock == anchorBlock && posBlock.isValid() && posBlock.layout()->lineCount()) {
            const QTextLine posLine = posBlock.layout()->lineForTextPosition(position - posBlock.position());
            const QTextLine anchorLine = anchorBlock.layout()->lineForTextPosition(anchor - anchorBlock.position());

            const int firstLine = qMin(posLine.lineNumber(), anchorLine.lineNumber());
            const int lastLine = qMax(posLine.lineNumber(), anchorLine.lineNumber());
            const QTextLayout *layout = posBlock.layout();
            r = QRectF();
            for (int i = firstLine; i <= lastLine; ++i) {
                r |= layout->lineAt(i).rect();
                r |= layout->lineAt(i).naturalTextRect(); // might be bigger in the case of wrap not enabled
            }
            r.translate(blockBoundingRect(posBlock).topLeft());
        } else {
            qDebug() << "error";
//            QRectF anchorRect = rectForPosition(cursor.selectionEnd());
//            r |= anchorRect;
//            r |= boundingRectOfFloatsInSelection(cursor);
//            QRectF frameRect(doc->documentLayout()->frameBoundingRect(cursor.currentFrame()));
//            r.setLeft(frameRect.left());
//            r.setRight(frameRect.right());
        }
        if (r.isValid())
            r.adjust(-1, -1, 1, 1);
    }
    return r;
}

void LiteEditorWidgetBase::testUpdateLink(QMouseEvent *e)
{
    if (!m_mouseNavigation) {
        return;
    }
    bool findLink = false;
    if (e->modifiers() & Qt::ControlModifier) {
        // Link emulation behaviour for 'go to definition'
        QTextCursor cursor = cursorForPosition(e->pos());
        if (!cursor.isNull()) {
            int pos = cursor.position();
            QRect rc = this->cursorRect(cursor);
            LiteApi::selectWordUnderCursor(cursor);
            if (cursor.hasSelection()) {
                rc.setLeft(rc.left()-(pos-cursor.selectionStart())*m_averageCharWidth);
                rc.setRight(rc.right()+(cursor.selectionEnd()-pos)*m_averageCharWidth);
                if (rc.contains(e->pos())) {
                    findLink = true;
                    m_showLinkNavigation = true;
                    emit updateLink(cursor,e->pos(),true);
                }
            }
        }
    }
    if (!findLink) {
        clearLink();
    }
}

void LiteEditorWidgetBase::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        this->clearBlockSelection();

        QTextBlock foldedBlock = foldedBlockAt(e->pos());
        if (foldedBlock.isValid()) {
            toggleBlockVisible(foldedBlock);
            viewport()->setCursor(Qt::IBeamCursor);
        }
        if (m_mouseNavigation) {
            if (this->m_currentLink.hasValidTarget()) {
                this->m_linkPressed = true;
            }
            m_uplinkSkip = true;
            this->stopUplinkTimer();
        }
    } else if (e->button() == Qt::RightButton) {
        int eventCursorPosition = cursorForPosition(e->pos()).position();
        if (eventCursorPosition < textCursor().selectionStart()
                || eventCursorPosition > textCursor().selectionEnd()) {
            setTextCursor(cursorForPosition(e->pos()));
        }
    }
    QPlainTextEdit::mousePressEvent(e);
}

void LiteEditorWidgetBase::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_mouseNavigation && m_linkPressed
        && (e->modifiers() & Qt::ControlModifier)
        && !(e->modifiers() & Qt::ShiftModifier)
        && e->button() == Qt::LeftButton) {
        if (openLink(m_currentLink)) {
            clearLink();
            return;
        }
    }
    QPlainTextEdit::mouseReleaseEvent(e);
}

const TextEditor::TabSettings &LiteEditorWidgetBase::tabSettings() const
{
    TextEditor::BaseTextDocumentLayout *layout = (TextEditor::BaseTextDocumentLayout*)this->document()->documentLayout();
    return layout->m_tabSettings;
}

void LiteEditorWidgetBase::clearBlockSelection()
{
    if (m_inBlockSelectionMode) {
        m_inBlockSelectionMode = false;
        m_blockSelection.clear();
        QTextCursor cursor = this->textCursor();
        cursor.clearSelection();
        setTextCursor(cursor);
    }
}

QString LiteEditorWidgetBase::copyBlockSelection() const
{
    QString selection;
    QTextCursor cursor = this->textCursor();
    if (!m_inBlockSelectionMode)
        return selection;
    const TextEditor::TabSettings &ts = this->tabSettings();
    QTextBlock block = m_blockSelection.firstBlock.block();
    QTextBlock lastBlock = m_blockSelection.lastBlock.block();
    bool textInserted = false;
    for (;;) {
        if (this->selectionVisible(block.blockNumber())) {
            if (textInserted)
                selection += QLatin1Char('\n');
            textInserted = true;

            QString text = block.text();
            int startOffset = 0;
            int startPos = ts.positionAtColumn(text, m_blockSelection.firstVisualColumn, &startOffset);
            int endOffset = 0;
            int endPos = ts.positionAtColumn(text, m_blockSelection.lastVisualColumn, &endOffset);

            if (startPos == endPos) {
                selection += QString(endOffset - startOffset, QLatin1Char(' '));
            } else {
                if (startOffset < 0)
                    selection += QString(-startOffset, QLatin1Char(' '));
                if (endOffset < 0)
                    --endPos;
                selection += text.mid(startPos, endPos - startPos);
                if (endOffset < 0)
                    selection += QString(ts.m_tabSize + endOffset, QLatin1Char(' '));
                else if (endOffset > 0)
                    selection += QString(endOffset, QLatin1Char(' '));
            }
        }
        if (block == lastBlock)
            break;

        block = block.next();
    }
    return selection;
}

void LiteEditorWidgetBase::removeBlockSelection(const QString &text)
{
    QTextCursor cursor = this->textCursor();
    if (!cursor.hasSelection() || !m_inBlockSelectionMode)
        return;

    int cursorPosition = cursor.selectionStart();
    cursor.clearSelection();
    cursor.beginEditBlock();

    const TextEditor::TabSettings &ts = this->tabSettings();
    QTextBlock block = m_blockSelection.firstBlock.block();
    QTextBlock lastBlock = m_blockSelection.lastBlock.block();
    for (;;) {
        QString text = block.text();
        int startOffset = 0;
        int startPos = ts.positionAtColumn(text, m_blockSelection.firstVisualColumn, &startOffset);
        int endOffset = 0;
        int endPos = ts.positionAtColumn(text, m_blockSelection.lastVisualColumn, &endOffset);

        cursor.setPosition(block.position() + startPos);
        cursor.setPosition(block.position() + endPos, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();

        if (startOffset < 0)
            cursor.insertText(QString(ts.m_tabSize + startOffset, QLatin1Char(' ')));
        if (endOffset < 0)
            cursor.insertText(QString(-endOffset, QLatin1Char(' ')));

        if (block == lastBlock)
            break;
        block = block.next();
    }

    cursor.setPosition(cursorPosition);
    if (!text.isEmpty())
        cursor.insertText(text);
    cursor.endEditBlock();
    this->setTextCursor(cursor);
}

bool LiteEditorWidgetBase::selectionVisible(int blockNumber) const
{
    Q_UNUSED(blockNumber);
    return true;
}

void LiteEditorWidgetBase::handleBlockSelection(int diff_row, int diff_col)
{
    if (!m_inBlockSelectionMode) {
        m_blockSelection.fromSelection(this->tabSettings(), textCursor());
        m_inBlockSelectionMode = true;
    }

    m_blockSelection.moveAnchor(m_blockSelection.anchorBlockNumber() + diff_row,
                                   m_blockSelection.anchorColumnNumber() + diff_col);
    setTextCursor(m_blockSelection.selection(this->tabSettings()));

    viewport()->update();
}

void LiteEditorWidgetBase::copy()
{
    if (!textCursor().hasSelection())
        return;

    QPlainTextEdit::copy();
}

void LiteEditorWidgetBase::paste()
{
    if (m_inBlockSelectionMode)
        removeBlockSelection();
    QPlainTextEdit::paste();
}

void LiteEditorWidgetBase::cut()
{
    if (m_inBlockSelectionMode) {
        copy();
        removeBlockSelection();
        return;
    }
    QPlainTextEdit::cut();
    //collectToCircularClipboard();
}

void LiteEditorWidgetBase::selectAll()
{
    this->clearBlockSelection();
    QPlainTextEdit::selectAll();
}

void LiteEditorWidgetBase::mouseMoveEvent(QMouseEvent *e)
{
    testUpdateLink(e);

    if (e->buttons() == Qt::NoButton) {
        const QTextBlock collapsedBlock = foldedBlockAt(e->pos());
        if (collapsedBlock.isValid() && !m_mouseOnFoldedMarker) {
            m_mouseOnFoldedMarker = true;
            viewport()->setCursor(Qt::PointingHandCursor);
        } else if (!collapsedBlock.isValid() && m_mouseOnFoldedMarker) {
            m_mouseOnFoldedMarker = false;
            viewport()->setCursor(Qt::IBeamCursor);
        }
    } else {
        QPlainTextEdit::mouseMoveEvent(e);
        if (e->modifiers() & Qt::AltModifier) {
            if (!m_inBlockSelectionMode) {
                m_blockSelection.fromSelection(this->tabSettings(), textCursor());
                m_inBlockSelectionMode = true;
            } else {
                QTextCursor cursor = textCursor();

                // get visual column
                int column = this->tabSettings().columnAt(
                            cursor.block().text(), cursor.positionInBlock());
                if (cursor.positionInBlock() == cursor.block().length()-1)
                    column += (e->pos().x() - cursorRect().center().x())/QFontMetricsF(font()).width(QLatin1Char(' '));
                m_blockSelection.moveAnchor(cursor.blockNumber(), column);
                setTextCursor(m_blockSelection.selection(this->tabSettings()));
                viewport()->update();
            }
        }
    }
    if (viewport()->cursor().shape() == Qt::BlankCursor)
        viewport()->setCursor(Qt::IBeamCursor);
}

bool LiteEditorWidgetBase::viewportEvent(QEvent *e)
{
    if (e->type() == QEvent::ToolTip) {
        if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
            return true;
        }
        const QHelpEvent *he = static_cast<QHelpEvent*>(e);
        m_upToolTipPos = he->pos();
        m_upToolTipDeployTimer->start(m_upToolTipTime);
        return true;
    }
    return QPlainTextEdit::viewportEvent(e);
}

void LiteEditorWidgetBase::inputMethodEvent(QInputMethodEvent *e)
{
    m_inputCursorOffset = e->preeditString().length();
    QPlainTextEdit::inputMethodEvent(e);
}

static void fillBackground(QPainter *p, const QRectF &rect, QBrush brush, QRectF gradientRect = QRectF())
{
    p->save();
    if (brush.style() >= Qt::LinearGradientPattern && brush.style() <= Qt::ConicalGradientPattern) {
        if (!gradientRect.isNull()) {
            QTransform m = QTransform::fromTranslate(gradientRect.left(), gradientRect.top());
            m.scale(gradientRect.width(), gradientRect.height());
            brush.setTransform(m);
            const_cast<QGradient *>(brush.gradient())->setCoordinateMode(QGradient::LogicalMode);
        }
    } else {
        p->setBrushOrigin(rect.topLeft());
    }
    p->fillRect(rect, brush);
    p->restore();
}

//copy of QTextDocument
static bool findInBlock(const QTextBlock &block, const QRegExp &expression, int offset,
                        QTextDocument::FindFlags options, QTextCursor &cursor)
{
    const QRegExp expr(expression);
    QString text = block.text();
    text.replace(QChar::Nbsp, QLatin1Char(' '));

    int idx = -1;
    while (offset >=0 && offset <= text.length()) {
        idx = (options & QTextDocument::FindBackward) ?
               expr.lastIndexIn(text, offset) : expr.indexIn(text, offset);

        if (idx == -1 || expr.matchedLength() == 0)
            return false;

        if (options & QTextDocument::FindWholeWords) {
            const int start = idx;
            const int end = start + expr.matchedLength();
            if ((start != 0 && isIdentifierChar(text.at(start - 1)))
                || (end != text.length() && isIdentifierChar(text.at(end)))) {
                //if this is not a whole word, continue the search in the string
                offset = (options & QTextDocument::FindBackward) ? idx-1 : end+1;
                idx = -1;
                continue;
            }
        }
        //we have a hit, return the cursor for that.
        break;
    }
    if (idx == -1)
        return false;
    cursor = QTextCursor(block.docHandle(), block.position() + idx);
    cursor.setPosition(cursor.position() + expr.matchedLength(), QTextCursor::KeepAnchor);
    return true;
}

void LiteEditorWidgetBase::paintEvent(QPaintEvent *e)
{  
    //QPlainTextEdit::paintEvent(e);
    //return;
    QPainter painter(viewport());
    QTextDocument *doc = this->document();
    QTextCursor cursor = textCursor();

    const QFontMetrics fm(this->font());
    int averageCharWidth = fm.averageCharWidth();
    int charOffsetX = this->document()->documentMargin()- this->horizontalScrollBar()->value();

    bool hasSelection = cursor.hasSelection();
    int selectionStart = cursor.selectionStart();
    int selectionEnd = cursor.selectionEnd();

    QTextBlock block = firstVisibleBlock();
    QPointF offset = contentOffset();
    qreal offsetX = offset.x();

    //QPlainTextEdit::paintEvent
    QRect er = e->rect();
    QRect viewportRect = viewport()->rect();

    painter.setPen(this->palette().color(QPalette::Text));
    painter.fillRect(er,this->palette().brush(QPalette::Base));

    bool editable = !isReadOnly();

    qreal maximumWidth = document()->documentLayout()->documentSize().width();

    // Set a brush origin so that the WaveUnderline knows where the wave started
    painter.setBrushOrigin(offset);

    // keep right margin clean from full-width selection
    int maxX = offset.x() + qMax((qreal)viewportRect.width(), maximumWidth)
               - document()->documentMargin();
    er.setRight(qMin(er.right(), maxX));
    painter.setClipRect(er);


    QAbstractTextDocumentLayout::PaintContext context = getPaintContext();

    static bool bc = true;
    bc = !bc;
    context.cursorPosition = bc ? -1 : cursor.position();
    if (!hasFocus()) {
        context.cursorPosition = -1;
    }

    int blockSelectionIndex = -1;
    if (m_inBlockSelectionMode
        && context.selections.count() && context.selections.last().cursor == textCursor()) {
        blockSelectionIndex = context.selections.size()-1;
        context.selections[blockSelectionIndex].format.clearBackground();
    }

    while (block.isValid()) {
        QRectF r = blockBoundingRect(block).translated(offset);
        QTextLayout *layout = block.layout();

        if (!block.isVisible()) {
            offset.ry() += r.height();
            block = block.next();
            continue;
        }

        if (r.bottom() >= er.top() && r.top() <= er.bottom()) {
            QTextBlockFormat blockFormat = block.blockFormat();

            QBrush bg = blockFormat.background();
            if (bg != Qt::NoBrush) {
                QRectF contentsRect = r;
                contentsRect.setWidth(qMax(r.width(), maximumWidth));
                fillBackground(&painter, contentsRect, bg);
            }

            QVector<QTextLayout::FormatRange> selections;
            QVector<QTextLayout::FormatRange> prioritySelections;
            int blpos = block.position();
            int bllen = block.length();
            for (int i = 0; i < context.selections.size(); ++i) {
                const QAbstractTextDocumentLayout::Selection &range = context.selections.at(i);
                const int selStart = range.cursor.selectionStart() - blpos;
                const int selEnd = range.cursor.selectionEnd() - blpos;
                if (selStart < bllen && selEnd >= 0
                    && selEnd >= selStart) {
                    QTextLayout::FormatRange o;
                    o.start = selStart;
                    o.length = selEnd - selStart;
                    o.format = range.format;
                    if (i == blockSelectionIndex) {
                        QString text = block.text();
                        const TextEditor::TabSettings &ts = this->tabSettings();
                        o.start = ts.positionAtColumn(text, m_blockSelection.firstVisualColumn);
                        o.length = ts.positionAtColumn(text,m_blockSelection.lastVisualColumn) - o.start;
                    }
//                    if ((hasSelection && i == context.selections.size()-1)
//                        || (o.format.foreground().style() == Qt::NoBrush
//                        && o.format.underlineStyle() != QTextCharFormat::NoUnderline
//                        && o.format.background() == Qt::NoBrush)) {
//                        prioritySelections.append(o);
//                    } else {
                        selections.append(o);
                   // }
                } else if (!range.cursor.hasSelection() && range.format.hasProperty(QTextFormat::FullWidthSelection)
                           && block.contains(range.cursor.position())) {
                    // for full width selections we don't require an actual selection, just
                    // a position to specify the line. that's more convenience in usage.
                    QTextLayout::FormatRange o;
                    QTextLine l = layout->lineForTextPosition(range.cursor.position() - blpos);
                    o.start = l.textStart();
                    o.length = l.textLength();
                    if (o.start + o.length == bllen - 1)
                        ++o.length; // include newline
                    o.format = range.format;
                    selections.append(o);
                }
            }
            selections += prioritySelections;
//            for (int i = 0; i < context.selections.size(); ++i) {
//                const QAbstractTextDocumentLayout::Selection &range = context.selections.at(i);
//                const int selStart = range.cursor.selectionStart() - blpos;
//                const int selEnd = range.cursor.selectionEnd() - blpos;
//                if (!hasSelection && range.cursor.selectionEnd() == selectionEnd) {
//                    continue;
//                }
//                if (selStart < bllen && selEnd > 0
//                    && selEnd > selStart) {
//                    QTextLayout::FormatRange o;
//                    o.start = selStart;
//                    o.length = selEnd - selStart;
//                    QTextCharFormat formatCopy(range.format);
//                    formatCopy.clearForeground();
//                    o.format = formatCopy;
//                    if (!o.format.hasProperty(LiteEditorWidgetBase::MatchBrace)) {
//                        //o.format.setForeground(palette().highlightedText());
//                        o.format.setBackground(palette().highlight());
//                    }
//                    selections.append(o);
//                } else if (!range.cursor.hasSelection() && range.format.hasProperty(QTextFormat::FullWidthSelection)
//                           && block.contains(range.cursor.position())) {
//                    // for full width selections we don't require an actual selection, just
//                    // a position to specify the line. that's more convenience in usage.
//                    QTextLayout::FormatRange o;
//                    QTextLine l = layout->lineForTextPosition(range.cursor.position() - blpos);
//                    o.start = l.textStart();
//                    o.length = l.textLength();
//                    if (o.start + o.length == bllen - 1)
//                        ++o.length; // include newline
//                    o.format = range.format;
//                    selections.append(o);
//                }
//            }

            if (block == textCursor().block()) {
                QTextLine l = layout->lineForTextPosition(textCursor().positionInBlock());
                QRectF rr = l.rect();
                rr.moveTop(rr.top() + r.top());
                rr.setLeft(offset.x());
                rr.setRight(viewportRect.width() - offset.x());
                painter.fillRect(rr, m_currentLineBackground);
            }
            bool findSelectionMark = false;
            if (!m_findExpression.isEmpty()) {
                painter.save();
                QColor color(this->palette().color(QPalette::Text));
                color.setAlpha(128);
                painter.setPen(color);
                int pos = 0;
                while (true) {
                    QTextCursor cur;
                    if (!findInBlock(block,m_findExpression,pos,m_findFlags,cur)) {
                        break;
                    }
                    findSelectionMark = true;
                    pos = cur.selectionEnd()-block.position();
                    QTextLine l = layout->lineForTextPosition(cur.selectionStart()-blpos);
                    qreal left = l.cursorToX(cur.selectionStart()-blpos);
                    qreal right = l.cursorToX(cur.selectionEnd()-blpos);
                    painter.drawRoundedRect(offsetX+left,r.top()+l.y(),right-left,l.height(),3,3);
                }
                painter.restore();
            } else if (!m_selectionExpression.isEmpty()) {
                painter.save();
                QColor color(this->palette().color(QPalette::Text));
                color.setAlpha(128);
                painter.setPen(color);
                int pos = 0;
                while (true) {
                    QTextCursor cur;
                    if (!findInBlock(block,m_selectionExpression,pos,QTextDocument::FindWholeWords,cur)) {
                        break;
                    }
                    findSelectionMark = true;
                    pos = cur.selectionEnd()-block.position();
                    if (selectionStart == cur.selectionStart() &&
                            selectionEnd == cur.selectionEnd()) {
                        continue;
                    }
                    QTextLine l = layout->lineForTextPosition(cur.selectionStart()-blpos);
                    qreal left = l.cursorToX(cur.selectionStart()-blpos);
                    qreal right = l.cursorToX(cur.selectionEnd()-blpos);
                    painter.drawRoundedRect(offsetX+left,r.top()+l.y(),right-left,l.height(),3,3);
                }
                painter.restore();
            }
            TextEditor::BaseTextDocumentLayout::userData(block)->setFindExpressionMark(findSelectionMark);

            QRectF blockSelectionCursorRect;
            if (m_inBlockSelectionMode
                    && block.position() >= m_blockSelection.firstBlock.block().position()
                    && block.position() <= m_blockSelection.lastBlock.block().position()) {
                QString text = block.text();
                const TextEditor::TabSettings &ts = this->tabSettings();
                qreal spacew = QFontMetricsF(font()).width(QLatin1Char(' '));

                int offset = 0;
                int relativePos  =  ts.positionAtColumn(text, m_blockSelection.firstVisualColumn, &offset);
                QTextLine line = layout->lineForTextPosition(relativePos);
                qreal x = line.cursorToX(relativePos) + offset * spacew;

                int eoffset = 0;
                int erelativePos  =  ts.positionAtColumn(text, m_blockSelection.lastVisualColumn, &eoffset);
                QTextLine eline = layout->lineForTextPosition(erelativePos);
                qreal ex = eline.cursorToX(erelativePos) + eoffset * spacew;

                QRectF rr = line.naturalTextRect();
                rr.moveTop(rr.top() + r.top());
                rr.setLeft(r.left() + x);
                if (line.lineNumber() == eline.lineNumber())
                    rr.setRight(r.left() + ex);
                painter.fillRect(rr, palette().highlight());
                if ((m_blockSelection.anchor == TextEditor::BaseTextBlockSelection::TopLeft
                        && block == m_blockSelection.firstBlock.block())
                        || (m_blockSelection.anchor == TextEditor::BaseTextBlockSelection::BottomLeft
                            && block == m_blockSelection.lastBlock.block())
                        ) {
                    rr.setRight(rr.left()+2);
                    blockSelectionCursorRect = rr;
                }
                for (int i = line.lineNumber() + 1; i < eline.lineNumber(); ++i) {
                    rr = layout->lineAt(i).naturalTextRect();
                    rr.moveTop(rr.top() + r.top());
                    rr.setLeft(r.left() + x);
                    painter.fillRect(rr, palette().highlight());
                }

                rr = eline.naturalTextRect();
                rr.moveTop(rr.top() + r.top());
                rr.setRight(r.left() + ex);
                if (line.lineNumber() != eline.lineNumber())
                    painter.fillRect(rr, palette().highlight());
                if ((m_blockSelection.anchor == TextEditor::BaseTextBlockSelection::TopRight
                     && block == m_blockSelection.firstBlock.block())
                        || (m_blockSelection.anchor == TextEditor::BaseTextBlockSelection::BottomRight
                            && block == m_blockSelection.lastBlock.block())) {
                    rr.setLeft(rr.right()-2);
                    blockSelectionCursorRect = rr;
                }
            }

            bool drawCursor = ( editable
                               && context.cursorPosition >= blpos
                               && context.cursorPosition < blpos + bllen);

            bool drawCursorAsBlock = drawCursor && overwriteMode() ;

            if (drawCursorAsBlock) {
                if (context.cursorPosition == blpos + bllen - 1) {
                    drawCursorAsBlock = false;
                } else {
                    QTextLayout::FormatRange o;
                    o.start = context.cursorPosition - blpos;
                    o.length = 1;
                    o.format.setForeground(palette().base());
                    o.format.setBackground(palette().text());                    
                    selections.append(o);
                }
            }


            layout->draw(&painter, offset, selections, er);

            if (!m_inBlockSelectionMode) {
                if ((drawCursor && !drawCursorAsBlock)
                    || (editable && context.cursorPosition < -1
                        && !layout->preeditAreaText().isEmpty())) {
                    int cpos = context.cursorPosition+m_inputCursorOffset;
                    if (cpos < -1)
                        cpos = layout->preeditAreaPosition() - (cpos + 2);
                    else
                        cpos -= blpos;
                    layout->drawCursor(&painter, offset, cpos, cursorWidth());
                }
            }

#ifndef Q_OS_MAC
            if (blockSelectionCursorRect.isValid())
                painter.fillRect(blockSelectionCursorRect, palette().text());
#endif
        }

        //draw indent line
        if (m_indentLineVisible) {
            QString text = block.text();
            int pos = text.length();
            for (int i = 0; i < pos; i++) {
                if (!text.at(i).isSpace()) {
                    pos = i;
                    break;
                }
            }

            QTextLine line = layout->lineForTextPosition(pos);
            int kt = r.top()+1;
            int kb = r.top()+line.height()-1;
            int k = line.cursorToX(pos)/averageCharWidth;

            painter.save();
            painter.setPen(QPen(m_indentLineForeground,1,Qt::DotLine));
            for (int i = 0; i < k; i+=m_nTabSize) {
                int xoff = charOffsetX+averageCharWidth*i;
                painter.drawLine(xoff,kt,xoff,kb);
            }
            painter.restore();
        }
        if (m_visualizeWhitespace) {
            QString text = block.text();
            painter.save();
            for (int i = 0; i < text.length(); i++) {
                if (text.at(i) == '\t') {
                     painter.setPen(QPen(m_visualizeWhitespaceForeground,1));
                     QTextLine line = layout->lineForTextPosition(i);
                     QRectF lineRect = line.naturalTextRect().translated(offset.x(), r.top());
                     qreal left = line.cursorToX(i)+offset.x()+2;
                     qreal right = line.cursorToX(i+1)+offset.x()-4;
                     qreal y = lineRect.top()+line.height()/2;
                     painter.drawLine(left,y,right,y);
                     painter.drawLine(right-4,y-4,right,y);
                     painter.drawLine(right-4,y+4,right,y);
                } else if (text.at(i).isSpace()) {
                    painter.setPen(QPen(m_visualizeWhitespaceForeground,2));
                    QTextLine line = layout->lineForTextPosition(i);
                    QRectF lineRect = line.naturalTextRect().translated(offset.x(), r.top());
                    qreal left = line.cursorToX(i)+offset.x();
                    qreal right = line.cursorToX(i+1)+offset.x();
                    qreal y = lineRect.top()+line.height()/2;
                    painter.drawPoint(left+(right-left)/2,y);
                }
            }
            painter.restore();
        }
        QTextBlock nextBlock = block.next();
        //draw wrap
        int lineCount = layout->lineCount();
        if (lineCount >= 2 || !nextBlock.isValid()) {
            painter.save();
            painter.setPen(Qt::lightGray);
            for (int i = 0; i < lineCount-1; ++i) { // paint line wrap indicator
                QTextLine line = layout->lineAt(i);
                QRectF lineRect = line.naturalTextRect().translated(offset.x(), r.top());
                QChar visualArrow((ushort)0x21b5);
                painter.drawText(QPointF(lineRect.right(),
                                         lineRect.top() + line.ascent()),
                                 visualArrow);
            }
            if (m_eofVisible && !nextBlock.isValid()) { // paint EOF symbol
                QTextLine line = layout->lineAt(lineCount-1);
                QRectF lineRect = line.naturalTextRect().translated(offset.x(), r.top());
                int h = 4;
                lineRect.adjust(0, 0, -1, -1);
                QPainterPath path;
                QPointF pos(lineRect.topRight() + QPointF(h+4, line.ascent()));
                path.moveTo(pos);
                path.lineTo(pos + QPointF(-h, -h));
                path.lineTo(pos + QPointF(0, -2*h));
                path.lineTo(pos + QPointF(h, -h));
                path.closeSubpath();
                painter.setBrush(painter.pen().color());
                painter.drawPath(path);
            }
            painter.restore();
        }

        //draw fold text ...
        QTextBlock nextVisibleBlock = nextBlock;

        if (!nextVisibleBlock.isVisible()) {
            // invisible blocks do have zero line count
            nextVisibleBlock = doc->findBlockByLineNumber(nextVisibleBlock.firstLineNumber());
            // paranoia in case our code somewhere did not set the line count
            // of the invisible block to 0
            while (nextVisibleBlock.isValid() && !nextVisibleBlock.isVisible())
                nextVisibleBlock = nextVisibleBlock.next();
        }

        if (nextBlock.isValid() && !nextBlock.isVisible()) {

            bool selectThis = (hasSelection
                               && nextBlock.position() >= selectionStart
                               && nextBlock.position() < selectionEnd);
            if (selectThis) {
                painter.save();
                painter.setBrush(palette().highlight());
            }

            QTextLayout *layout = block.layout();
            QTextLine line = layout->lineAt(layout->lineCount()-1);
            QRectF lineRect = line.naturalTextRect().translated(offset.x(), r.top());
            lineRect.adjust(0, 0, -1, -1);

            QRectF collapseRect(lineRect.right() + 12,
                                lineRect.top(),
                                fontMetrics().width(QLatin1String(" {...}; ")),
                                lineRect.height());
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.translate(.5, .5);
            painter.drawRoundedRect(collapseRect.adjusted(0, 0, 0, -1), 3, 3);
            painter.setRenderHint(QPainter::Antialiasing, false);
            painter.translate(-.5, -.5);

            QString replacement = QLatin1String("...");

            if (TextEditor::TextBlockUserData *nextBlockUserData = TextEditor::BaseTextDocumentLayout::testUserData(nextBlock)) {
                if (nextBlockUserData->foldingStartIncluded())
                    replacement.prepend(nextBlock.text().trimmed().left(1));
            }

            block = nextVisibleBlock.previous();
            if (!block.isValid())
                block = doc->lastBlock();

            if (TextEditor::TextBlockUserData *blockUserData = TextEditor::BaseTextDocumentLayout::testUserData(block)) {
                if (blockUserData->foldingEndIncluded()) {
                    QString right = block.text().trimmed();
                    if (right.endsWith(QLatin1Char(';'))) {
                        right.chop(1);
                        right = right.trimmed();
                        replacement.append(right.right(right.endsWith(QLatin1Char('/')) ? 2 : 1));
                        replacement.append(QLatin1Char(';'));
                    } else {
                        replacement.append(right.right(right.endsWith(QLatin1Char('/')) ? 2 : 1));
                    }
                }
            }

            if (selectThis)
                painter.setPen(palette().highlightedText().color());
            painter.drawText(collapseRect, Qt::AlignCenter, replacement);
            if (selectThis)
                painter.restore();
        }

        offset.ry() += r.height();

        if (offset.y() > viewportRect.height())
            break;
        block = block.next();
    }

    if (backgroundVisible() && !block.isValid() && offset.y() <= er.bottom()
        && (centerOnScroll() || verticalScrollBar()->maximum() == verticalScrollBar()->minimum())) {
        painter.fillRect(QRect(QPoint((int)er.left(), (int)offset.y()), er.bottomRight()), palette().background());
    }

    if (m_rightLineVisible) {
        int xoff = charOffsetX+averageCharWidth*m_rightLineWidth;
        painter.save();
        painter.setPen(QPen(m_indentLineForeground,1,Qt::DotLine));
        painter.drawLine(xoff,0,xoff,rect().height());
        painter.restore();
    }
}
