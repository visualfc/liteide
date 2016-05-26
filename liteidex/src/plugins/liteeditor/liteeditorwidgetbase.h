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
// Module: liteeditorwidgetbase.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEEDITORWIDGETBASE_H
#define LITEEDITORWIDGETBASE_H

#include <QPlainTextEdit>
#include <QTextBlock>
#include "liteeditorapi/liteeditorapi.h"
#include "qtc_texteditor/autocompleter.h"
#include "qtc_texteditor/basetextblockselection.h"

class NavigateManager;
struct NavigateMark;

class LiteEditorWidgetBase : public QPlainTextEdit
{
    Q_OBJECT
public:
    enum TextFormatProperty {
        MatchBrace = QTextFormat::UserProperty+1,
        CurrentLine
    };

    LiteEditorWidgetBase(LiteApi::IApplication *app, QWidget *parent = 0);
    virtual ~LiteEditorWidgetBase();
    void initLoadDocument();
    void setTabSize(int n);
    int tabSize() const;
    void updateTabWidth();
    void setTabToSpaces(bool b);
    void setEditorMark(LiteApi::IEditorMark *mark);
    void setTextLexer(LiteApi::ITextLexer *lexer);
public:
    QWidget* extraArea();
    QWidget* navigateArea();
    void setExtraColor(const QColor &foreground,const QColor &background);
    void setCurrentLineColor(const QColor &background);
    void setIndentLineColor(const QColor &foreground);
    void setVisualizeWhitespaceColor(const QColor &foreground);
    int extraAreaWidth();
    void extraAreaPaintEvent(QPaintEvent *e);
    void extraAreaMouseEvent(QMouseEvent *e);
    void extraAreaLeaveEvent(QEvent *e);
    int navigateAreaWidth();
    int isInNavigateMark(const QPoint &pos, int *poffset);
    bool isInNavigateHead(const QPoint &pos);
    void navigateAreaPaintEvent(QPaintEvent *e);
    void navigateAreaMouseEvent(QMouseEvent *e);
    void navigateAreaLeaveEvent(QEvent *e);
    void navigateAreaEnterEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);
    void showToolTipInfo(const QPoint & pos, const QString & text);
    void cleanWhitespace(QTextCursor &cursor, bool inEntireDocument);
    void ensureFinalNewLine(QTextCursor& cursor);
    void setNavigateHead(LiteApi::EditorNaviagteType type, const QString &msg);
    void insertNavigateMark(int line, LiteApi::EditorNaviagteType type, const QString &msg, const char* tag);
    void clearAllNavigateMark(LiteApi::EditorNaviagteType types, const char *tag);
    void clearAllNavigateMarks();
signals:
    void navigationStateChanged(const QByteArray &array);
    void overwriteModeChanged(bool);
    void wordWrapChanged(bool);
    void updateLink(const QTextCursor &curosr, const QPoint &pos, bool nav);
public:
    void saveCurrentCursorPositionForNavigation();
    bool restoreState(const QByteArray &state);
    QByteArray saveState() const;
protected:
    QByteArray m_tempNavigationState;
public slots:
    void cleanWhitespace(bool wholeDocument = false);
    void editContentsChanged(int,int,int);
    virtual void highlightCurrentLine();
    virtual void slotUpdateExtraAreaWidth();
    virtual void slotModificationChanged(bool);
    virtual void slotUpdateRequest(const QRect &r, int dy);
    virtual void slotCursorPositionChanged();
    virtual void slotSelectionChanged();
    virtual void slotUpdateBlockNotify(const QTextBlock &);
    QChar characterAt(int pos) const;
    void handleHomeKey(bool anchor);    
    void setFindOption(LiteApi::FindOption *opt);
    void setWordWrapOverride(bool wrap);
    void setDefaultWordWrap(bool wrap);
public slots:
    void gotoMatchBrace();
    void gotoLine(int line, int column, bool center);
    void gotoLineStart();
    void gotoLineStartWithSelection();
    void gotoLineEnd();
    void gotoLineEndWithSelection();

    void duplicate();

    void cutLine();
    void copyLine();
    void deleteLine();

    void deleteEndOfWord();
    void deleteStartOfWord();

    void moveLineUp();
    void moveLineDown();

    void copyLineUp();
    void copyLineDown();

    void joinLines();

    void insertLineBefore();
    void insertLineAfter();

    void gotoPrevBlock();
    void gotoNextBlock();
    void selectBlock();
    bool findPrevBlock(QTextCursor &cursor, int indent, const QString &skip = "//") const;
    bool findNextBlock(QTextCursor &cursor, int indent, const QString &skip = "//") const;
    bool findStartBlock(QTextCursor &cursor, int indent) const;
    bool findEndBlock(QTextCursor &cursor, int indent) const;
    void fold();
    void unfold();
    void foldAll();
    void unfoldAll();
    void updateBlock(QTextBlock);
    void documentSizeChanged(QSizeF);
    void moveCursorVisible(bool ensureVisible);
    void toggleBlockVisible(const QTextBlock &block);
    void foldIndentChanged(QTextBlock block);
    void autoIndent();
public:
    void setMaxTipInfoLines(int maxLines) {
        m_maxTipInfoLines = maxLines;
    }

    void setMimeType(const QString &mime) {
        m_mimeType = mime;
    }

    void setAutoIndent(bool b){
        m_autoIndent = b;
    }
    void setAutoBraces0(bool b) {
        m_autoBraces0 = b;
    }
    void setAutoBraces1(bool b) {
        m_autoBraces1 = b;
    }
    void setAutoBraces2(bool b) {
        m_autoBraces2 = b;
    }
    void setAutoBraces3(bool b) {
        m_autoBraces3 = b;
    }
    void setAutoBraces4(bool b) {
        m_autoBraces4 = b;
    }
    void setAutoBraces5(bool b) {
        m_autoBraces5 = b;
    }
    void setLineNumberVisible(bool b) {
        m_lineNumbersVisible = b;
        slotUpdateExtraAreaWidth();
    }
    void setNatigateWidgetVisible(bool b) {
        m_navigateWidgetVisible = b;
        slotUpdateExtraAreaWidth();
    }
    void setMarksVisible(bool b) {
        m_marksVisible = b;
        slotUpdateExtraAreaWidth();
    }
    bool lineNumberVisible() const {
        return m_lineNumbersVisible;
    }
    bool marksVisiable() const {
        return m_marksVisible;
    }
    void setRightLineVisible(bool b) {
        m_rightLineVisible = b;
    }
    bool rightLineVisible() const {
        return m_rightLineVisible;
    }
    void setRightLineWidth(int w) {
        m_rightLineWidth = w;
    }
    int rightLineWidth() const {
        return m_rightLineWidth;
    }
    void setEofVisible(bool b) {
        m_eofVisible = b;
    }
    void setCodeFoldVisible(bool b) {
        m_codeFoldingVisible = b;
    }
    bool codeFoldVisible() const {
        return m_codeFoldingVisible;
    }
    void setIndentLineVisible(bool b) {
        m_indentLineVisible = b;
    }
    bool indentLineVisible() const {
        return m_indentLineVisible;
    }
    bool isWordWrap() const;
    void setWordWrap(bool wrap);
    void maybeSelectLine();
    bool isSpellCheckingAt(QTextCursor cur) const;
    void showLink(const LiteApi::Link &link);
    void clearLink();
    bool openLink(const LiteApi::Link &link);
    void testUpdateLink(QMouseEvent *e);
    void setExtraSelections(LiteApi::ExtraSelectionKind kind, const QList<QTextEdit::ExtraSelection> &selections);
    LiteApi::ITextLexer * textLexer() const {
        return m_textLexer.data();
    }
    QString autoCompleteSurroundText(QTextCursor &cursor, const QString &textToInsert) const;
    bool checkIsMatchBraces(QTextCursor &cursor, const QString &textToInsert) const;
    bool autoBackspace(QTextCursor &cursor);
    void handleBackspaceKey();
    void setVisualizeWhitespace(bool b);
    QRectF selectionRect(const QTextCursor &cursor);
protected:
    void drawFoldingMarker(QPainter *painter, const QPalette &pal,
                           const QRect &rect,
                           bool expanded) const;
    bool event(QEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void leaveEvent(QEvent *e);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    bool viewportEvent(QEvent *);
    void inputMethodEvent(QInputMethodEvent *e);
    void indentBlock(QTextBlock block, bool bIndent);
    void indentCursor(QTextCursor cur, bool bIndent);
    void indentText(QTextCursor cur, bool bIndent);
    void indentEnter(QTextCursor cur);
    QString tabText(int n = 1) const;
    QTextBlock foldedBlockAt(const QPoint &pos, QRect *box = 0) const;
    void moveLineUpDown(bool up);
    void copyLineUpDown(bool up);
    const TextEditor::TabSettings &tabSettings() const;
    void clearBlockSelection();
    QString copyBlockSelection() const;
    void removeBlockSelection(const QString &text = QString());
    bool selectionVisible(int blockNumber) const;
    void handleBlockSelection(int diff_row, int diff_col);
public slots:
    virtual void copy();
    virtual void paste();
    virtual void cut();
    virtual void selectAll();
protected slots:
    void uplinkDeployTimeout();
    void uplinkInfoTimeout();
    void stopUplinkTimer();
protected:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_extraArea;
    QWidget *m_navigateArea;
    LiteApi::IEditorMark *m_editorMark;
    QScopedPointer<LiteApi::ITextLexer> m_textLexer;
    QMap<LiteApi::ExtraSelectionKind,QList<QTextEdit::ExtraSelection> > m_extralSelectionMap;
    QTextCursor m_lastSelection;
    QColor  m_extraForeground;
    QColor  m_extraBackground;
    QColor  m_indentLineForeground;
    QColor  m_currentLineBackground;
    QColor  m_visualizeWhitespaceForeground;
    QRegExp m_selectionExpression;
    QRegExp m_findExpression;
    QTextDocument::FindFlags m_findFlags;
    LiteApi::Link       m_currentLink;
    QString m_mimeType;
    bool m_moveLineUndoHack;
    bool m_defaultWordWrap;
    bool m_wordWrapOverridden;
    bool m_wordWrap;
    bool m_lineNumbersVisible;
    bool m_navigateWidgetVisible;
    bool m_marksVisible;    
    bool m_codeFoldingVisible;
    bool m_rightLineVisible;
    bool m_eofVisible;
    int  m_rightLineWidth;
    int  m_maxTipInfoLines;
    int  m_inputCursorOffset;
    bool m_indentLineVisible;
    bool m_autoIndent;
    bool m_autoBraces0; //{
    bool m_autoBraces1; //(
    bool m_autoBraces2; //[
    bool m_autoBraces3; //'
    bool m_autoBraces4; //"
    bool m_autoBraces5; //`
    bool m_bLastBraces;
    bool m_bTabUseSpace;
    bool m_mouseNavigation;
    bool m_showLinkNavigation;
    bool m_showLinkInfomation;
    QString m_lastBraceText;
    int  m_nTabSize;
    int m_lastSaveRevision;
    int m_extraAreaSelectionNumber;
    int m_averageCharWidth;
    int m_lastLine;
    int m_upToolTipTime;
    QTimer *m_upToolTipDeployTimer;
    QTimer *m_upToolTipTimer;
    QPoint m_upToolTipPos;
    QPoint m_lastUpToolTipPos;
    bool m_linkPressed;
    bool m_uplinkSkip;
    bool m_mouseOnFoldedMarker;
    bool m_contentsChanged;
    bool m_lastCursorChangeWasInteresting;
    bool m_visualizeWhitespace;
    NavigateManager *m_navigateManager;
    bool m_inBlockSelectionMode;
    TextEditor::BaseTextBlockSelection m_blockSelection;
};

inline bool isIdentifierChar(const QChar &ch)
{
    return ch.isLetterOrNumber() || ch == QChar('_');
}


#endif // LITEEDITORWIDGETBASE_H
