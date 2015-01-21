/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: liteeditor.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEEDITOR_H
#define LITEEDITOR_H

#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "extension/extension.h"
#include "elidedlabel/elidedlabel.h"
#include <QSet>
#include <QHash>
#include <QStack>
#include <QTextBlock>
#include <QLabel>

//#define LITEEDITOR_FIND

class TreeModelCompleter;
class QAbstractItemModel;
class QStandardItemModel;
class QStandardItem;
class LiteEditorWidget;
class LiteEditorFile;
class QComboBox;
class QLabel;
class QToolButton;
class LiteCompleter;
class ColorStyleScheme;
class FunctionTooltip;

class QLabelEx : public QLabel
{
    Q_OBJECT
public:
    QLabelEx(const QString &text, QWidget *parent = 0);
signals:
    void doubleClickEvent();
protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
};

class LiteEditor : public LiteApi::ILiteEditor
{
    Q_OBJECT
public:
    LiteEditor(LiteApi::IApplication *app);
    virtual ~LiteEditor();
    virtual QTextDocument* document() const;
    virtual void setCompleter(LiteApi::ICompleter *complter);
    virtual void setEditorMark(LiteApi::IEditorMark *mark);
    virtual void setTextLexer(LiteApi::ITextLexer *lexer);
    void setSyntaxHighlighter(TextEditor::SyntaxHighlighter *syntax);
    TextEditor::SyntaxHighlighter *syntaxHighlighter() const;
    void createActions();
    void createToolBars();
    void createMenu();
    virtual LiteApi::IExtension *extension();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QIcon icon() const;
    virtual bool createNew(const QString &contents, const QString &mimeType);
    virtual bool open(const QString &filePath, const QString &mimeType);
    virtual bool reload();
    virtual bool save();
    virtual bool saveAs(const QString &filePath);
    virtual void setReadOnly(bool b);
    virtual bool isReadOnly() const;
    virtual bool isModified() const;
    virtual QString filePath() const;
    virtual QString mimeType() const;
    virtual LiteApi::IFile *file();
    virtual int line() const;
    virtual int column() const;
    virtual int utf8Position(bool realFile) const;
    virtual QByteArray utf8Data() const;
    virtual void setWordWrap(bool wrap);
    virtual bool wordWrap() const;
    virtual void gotoLine(int line, int column, bool center);
    virtual int position(PositionOperation posOp = Current, int at = -1) const;
    virtual QString textAt(int pos, int length) const;
    virtual QRect cursorRect(int pos = -1) const;
    virtual QTextCursor textCursor() const;
    virtual LiteEditorWidget *editorWidget() const;
    virtual QString textCodec() const;
    virtual void setTextCodec(const QString &codec);
    virtual QByteArray saveState() const;
    virtual bool restoreState(const QByteArray &state);
    virtual void onActive();
    virtual void setFindOption(LiteApi::FindOption *opt);
    virtual void setSpellCheckZoneDontComplete(bool b);
    virtual void setNavigateHead(LiteApi::EditorNaviagteType type, const QString &msg);
    virtual void insertNavigateMark(int line, LiteApi::EditorNaviagteType type, const QString &msg, const char* tag);
    virtual void clearNavigateMarak(int line);
    virtual void clearAllNavigateMarks();
    virtual void clearAllNavigateMark(LiteApi::EditorNaviagteType types, const char *tag);
    virtual void showLink(const LiteApi::Link &link);
    virtual void clearLink();
    virtual void setTabOption(int tabSize, bool tabToSpace);
    virtual void setEnableAutoIndentAction(bool b);
    virtual bool isLineEndUnix() const;
    virtual void setLineEndUnix(bool b);
signals:
    void colorStyleChanged();
    void tabSettingChanged(int);
public slots:
    void requestFontZoom(int zoom);
    void loadColorStyleScheme();
    void applyOption(QString);
    void clipbordDataChanged();
#ifdef LITEEDITOR_FIND
    void findNextText();
#endif
    void updateTip(const QString &func,const QString &kind,const QString &info);
    void exportPdf();
    void exportHtml();
    void filePrint();
    void filePrintPreview();
    void printPreview(QPrinter *printer);
    void codecComboBoxChanged(QString);
    void editPositionChanged();
    void navigationStateChanged(const QByteArray &state);
    void gotoLine();
    void selectNextParam();
    void increaseFontSize();
    void decreaseFontSize();
    void resetFontSize();
    void setEditToolbarVisible(bool visible);
    void comment();
    void blockComment();
    void autoIndent();
    void tabToSpacesToggled(bool b);
    void toggledVisualizeWhitespace(bool b);
    void triggeredLineEnding(QAction *action);
    void broadcast(const QString &module, const QString &id, const QString &param);
public:
    void updateFont();
    void sendUpdateFont();
    void initLoad();
    void findCodecs();
    QList<QTextCodec *> m_codecs;
    LiteApi::IApplication *m_liteApp;
    Extension   *m_extension;
    QWidget *m_widget;
    QToolBar *m_editToolBar;
    QToolBar *m_buildToolBar;
    QToolBar *m_infoToolBar;
    LiteEditorWidget    *m_editorWidget;
    LiteApi::ICompleter *m_completer;
    TextEditor::SyntaxHighlighter *m_syntax;
    FunctionTooltip     *m_funcTip;
    QAction *m_undoAct;
    QAction *m_redoAct;
    QAction *m_cutAct;
    QAction *m_copyAct;
    QAction *m_pasteAct;
    QAction *m_selectAllAct;
    QAction *m_exportHtmlAct;
    QAction *m_exportPdfAct;
    QAction *m_filePrintAct;
    QAction *m_filePrintPreviewAct;
#ifdef LITEEDITOR_FIND
    QComboBox *m_findComboBox;
#endif
    QAction *m_gotoPrevBlockAct;
    QAction *m_gotoNextBlockAct;
    QAction *m_gotoMatchBraceAct;
    QAction *m_selectBlockAct;
    QAction *m_gotoLineAct;
    QAction *m_duplicateAct;
    QAction *m_deleteLineAct;
    QAction *m_insertLineBeforeAct;
    QAction *m_insertLineAfterAct;
    QAction *m_increaseFontSizeAct;
    QAction *m_decreaseFontSizeAct;
    QAction *m_resetFontSizeAct;
    QAction *m_foldAct;
    QAction *m_unfoldAct;
    QAction *m_foldAllAct;
    QAction *m_unfoldAllAct;
    QAction *m_lockAct;
    QAction *m_cleanWhitespaceAct;
    QAction *m_wordWrapAct;
    QAction *m_codeCompleteAct;
    LiteEditorFile *m_file;
    QMenu   *m_editMenu;
    QMenu   *m_contextMenu;
    QString  m_colorStyle;
    QPalette m_defEditorPalette;
    bool       m_bReadOnly;
    bool     m_offsetVisible;
    QLabelEx  *m_lineInfo;
    QAction *m_overInfoAct;
    QAction *m_closeEditorAct;
    QAction *m_commentAct;
    QAction *m_blockCommentAct;
    QAction *m_autoIndentAct;
    QAction *m_tabToSpacesAct;
    QAction *m_lineEndingWindowAct;
    QAction *m_lineEndingUnixAct;
    QAction *m_visualizeWhitespaceAct;
};

class EditContext : public LiteApi::IEditContext
{
    Q_OBJECT
public:
    EditContext(LiteEditor *editor, QObject *parent);
    virtual QWidget *focusWidget() const;
    virtual QMenu   *focusMenu() const;
    virtual QToolBar *focusToolBar() const;
protected:
    LiteEditor  *m_editor;
};

#endif //LITEEDITOR_H
