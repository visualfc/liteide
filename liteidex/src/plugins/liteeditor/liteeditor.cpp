/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: liteeditor.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteeditor.h"
#include "liteeditorwidget.h"
#include "liteeditorfile.h"
#include "litecompleter.h"
#include "litewordcompleter.h"
#include "liteeditor_global.h"
#include "colorstyle/colorstyle.h"
#include "qtc_texteditor/generichighlighter/highlighter.h"

#include <QFileInfo>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextDocument>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTextCursor>
#include <QToolTip>
#include <QFileDialog>
#include <QPrinter>
#include <QTextDocumentWriter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPageSetupDialog>
#include <QTextCodec>
#include <QDebug>
#include <QPalette>
#include <QTimer>
#include <QMessageBox>
#include <QTextBlock>
#include <QMenu>
#include <QInputDialog>
#include <QToolButton>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


LiteEditor::LiteEditor(LiteApi::IApplication *app)
    : m_liteApp(app),
      m_extension(new Extension),
      m_completer(0),
      m_bReadOnly(false)
{
    m_widget = new QWidget;
    m_editorWidget = new LiteEditorWidget(m_widget);
    m_editorWidget->setCursorWidth(2);
    //m_editorWidget->setAcceptDrops(false);

    m_defEditorPalette = m_editorWidget->palette();

    createActions();
    createToolBars();
    createMenu();

    m_editorWidget->setContextMenu(m_contextMenu);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
/*
    m_toolBar->setStyleSheet("QToolBar {border: 1px ; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #eeeeee, stop: 1 #ababab); }"\
                             "QToolBar QToolButton { border:1px ; border-radius: 1px; }"\
                             "QToolBar QToolButton::hover { background-color: #ababab;}"\
                             "QToolBar::separator {width:2px; margin-left:2px; margin-right:2px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #dedede, stop: 1 #a0a0a0);}");
*/
    QHBoxLayout *toolLayout = new QHBoxLayout;
    toolLayout->setMargin(0);
    toolLayout->setSpacing(0);
    toolLayout->addWidget(m_toolBar);
    toolLayout->addWidget(m_infoToolBar);

    layout->addLayout(toolLayout);
//    QHBoxLayout *hlayout = new QHBoxLayout;
//    hlayout->addWidget(m_editorWidget);
//    hlayout->addWidget(m_editorWidget->navigateArea());
    //layout->addLayout(hlayout);
    layout->addWidget(m_editorWidget);
    m_widget->setLayout(layout);
    m_file = new LiteEditorFile(m_liteApp,this);
    m_file->setDocument(m_editorWidget->document());

//    QTextOption option =  m_editorWidget->document()->defaultTextOption();
//    option.setFlags(option.flags() | QTextOption::ShowTabsAndSpaces);
//    option.setFlags(option.flags() | QTextOption::AddSpaceForLineAndParagraphSeparators);
//    option.setTabs(tabs);

//    m_editorWidget->document()->setDefaultTextOption(option);

    setEditToolbarVisible(true);

    connect(m_file->document(),SIGNAL(modificationChanged(bool)),this,SIGNAL(modificationChanged(bool)));
    connect(m_file->document(),SIGNAL(contentsChanged()),this,SIGNAL(contentsChanged()));
    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));
    connect(m_liteApp->editorManager(),SIGNAL(colorStyleSchemeChanged()),this,SLOT(loadColorStyleScheme()));
    connect(m_liteApp->editorManager(),SIGNAL(editToolbarVisibleChanged(bool)),this,SLOT(setEditToolbarVisible(bool)));

    //applyOption("option/liteeditor");

    m_extension->addObject("LiteApi.ITextEditor",this);
    m_extension->addObject("LiteApi.ILiteEditor",this);
    m_extension->addObject("LiteApi.QToolBar",m_toolBar);
    m_extension->addObject("LiteApi.QPlainTextEdit",m_editorWidget);
    m_extension->addObject("LiteApi.ContextMenu",m_contextMenu);
    m_extension->addObject("LiteApi.Menu.Edit",m_editMenu);

    m_editorWidget->installEventFilter(m_liteApp->editorManager());
    connect(m_editorWidget,SIGNAL(cursorPositionChanged()),this,SLOT(editPositionChanged()));
    connect(m_editorWidget,SIGNAL(navigationStateChanged(QByteArray)),this,SLOT(navigationStateChanged(QByteArray)));
    connect(m_editorWidget,SIGNAL(overwriteModeChanged(bool)),m_overInfoAct,SLOT(setVisible(bool)));
    connect(m_editorWidget,SIGNAL(requestFontZoom(int)),this,SLOT(requestFontZoom(int)));
    connect(m_lineInfo,SIGNAL(doubleClickEvent()),this,SLOT(gotoLine()));
    connect(m_closeEditorAct,SIGNAL(triggered()),m_liteApp->editorManager(),SLOT(closeEditor()));
}

LiteEditor::~LiteEditor()
{
    if (m_completer) {
        delete m_completer;
    }
    delete m_contextMenu;
    delete m_editMenu;
    delete m_extension;
    delete m_editorWidget;
    delete m_widget;
    delete m_file;
}

LiteEditorWidget *LiteEditor::editorWidget() const
{
    return m_editorWidget;
}

void LiteEditor::setEditorMark(LiteApi::IEditorMark *mark)
{
    m_editorWidget->setEditorMark(mark);
    m_extension->addObject("LiteApi.IEditorMark",mark);
}

void LiteEditor::setCompleter(LiteApi::ICompleter *complter)
{
    if (m_completer) {
        QObject::disconnect(m_completer, 0, m_editorWidget, 0);
        delete m_completer;
        m_completer = 0;
    }
    m_completer = complter;
    if (!m_completer) {
        return;
    }
    m_completer->setEditor(m_editorWidget);
    m_editorWidget->setCompleter(m_completer->completer());

    m_extension->addObject("LiteApi.ICompleter",complter);

    connect(m_editorWidget,SIGNAL(completionPrefixChanged(QString,bool)),m_completer,SLOT(completionPrefixChanged(QString,bool)));
    connect(m_completer,SIGNAL(wordCompleted(QString,QString)),this,SLOT(updateTip(QString,QString)));
}

void LiteEditor::clipbordDataChanged()
{
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard->mimeData()->hasText() ||
            clipboard->mimeData()->hasHtml()) {
        m_pasteAct->setEnabled(true);
    } else {
        m_pasteAct->setEnabled(false);
    }
}

void LiteEditor::createActions()
{
    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"Editor");

    m_undoAct = new QAction(QIcon("icon:liteeditor/images/undo.png"),tr("Undo"),this);
    actionContext->regAction(m_undoAct,"Undo",QKeySequence::Undo);

    m_redoAct = new QAction(QIcon("icon:liteeditor/images/redo.png"),tr("Redo"),this);
    actionContext->regAction(m_redoAct,"Redo","Ctrl+Shift+Z; Ctrl+Y");

    m_cutAct = new QAction(QIcon("icon:liteeditor/images/cut.png"),tr("Cut"),this);
    actionContext->regAction(m_cutAct,"Cut",QKeySequence::Cut);

    m_copyAct = new QAction(QIcon("icon:liteeditor/images/copy.png"),tr("Copy"),this);
    actionContext->regAction(m_copyAct,"Copy",QKeySequence::Copy);

    m_pasteAct = new QAction(QIcon("icon:liteeditor/images/paste.png"),tr("Paste"),this);
    actionContext->regAction(m_pasteAct,"Paste",QKeySequence::Paste);

    m_selectAllAct = new QAction(tr("Select All"),this);
    actionContext->regAction(m_selectAllAct,"SelectAll",QKeySequence::SelectAll);

    m_exportHtmlAct = new QAction(QIcon("icon:liteeditor/images/exporthtml.png"),tr("Export HTML..."),this);
#ifndef QT_NO_PRINTER
    m_exportPdfAct = new QAction(QIcon("icon:liteeditor/images/exportpdf.png"),tr("Export PDF..."),this);
    m_filePrintAct = new QAction(QIcon("icon:liteeditor/images/fileprint.png"),tr("Print..."),this);
    m_filePrintPreviewAct = new QAction(QIcon("icon:liteeditor/images/fileprintpreview.png"),tr("Print Preview..."),this);
#endif
    m_gotoPrevBlockAct = new QAction(tr("Go To Previous Block"),this);
    actionContext->regAction(m_gotoPrevBlockAct,"GotoPreviousBlock","Ctrl+[");

    m_gotoNextBlockAct = new QAction(tr("Go To Next Block"),this);
    actionContext->regAction(m_gotoNextBlockAct,"GotoNextBlock","Ctrl+]");


    m_selectBlockAct = new QAction(tr("Select Block"),this);
    actionContext->regAction(m_selectBlockAct,"SelectBlock","Ctrl+U");

    m_gotoMatchBraceAct = new QAction(tr("Go To Matching Brace"),this);
    actionContext->regAction(m_gotoMatchBraceAct,"GotoMatchBrace","Ctrl+E");

    m_foldAct = new QAction(tr("Fold"),this);   
    actionContext->regAction(m_foldAct,"Fold","Ctrl+<");

    m_unfoldAct = new QAction(tr("Unfold"),this);
    actionContext->regAction(m_unfoldAct,"Unfold","Ctrl+>");

    m_foldAllAct = new QAction(tr("Fold All"),this);
    actionContext->regAction(m_foldAllAct,"FoldAll","");

    m_unfoldAllAct = new QAction(tr("Unfold All"),this);
    actionContext->regAction(m_unfoldAllAct,"UnfoldAll","");

    connect(m_foldAct,SIGNAL(triggered()),m_editorWidget,SLOT(fold()));
    connect(m_unfoldAct,SIGNAL(triggered()),m_editorWidget,SLOT(unfold()));
    connect(m_foldAllAct,SIGNAL(triggered()),m_editorWidget,SLOT(foldAll()));
    connect(m_unfoldAllAct,SIGNAL(triggered()),m_editorWidget,SLOT(unfoldAll()));

    m_gotoLineAct = new QAction(tr("Go To Line"),this);
    actionContext->regAction(m_gotoLineAct,"GotoLine","Ctrl+L");

    m_lockAct = new QAction(QIcon("icon:liteeditor/images/lock.png"),tr("Locked"),this);
    m_lockAct->setEnabled(false);

    m_duplicateAct = new QAction(tr("Duplicate"),this);
    actionContext->regAction(m_duplicateAct,"Duplicate","Ctrl+D");

    connect(m_duplicateAct,SIGNAL(triggered()),m_editorWidget,SLOT(duplicate()));

    m_deleteLineAct = new QAction(tr("Delete Line"),this);
    actionContext->regAction(m_deleteLineAct,"DeleteLine","Ctrl+Shift+K");

    connect(m_deleteLineAct,SIGNAL(triggered()),m_editorWidget,SLOT(deleteLine()));

    m_insertLineBeforeAct = new QAction(tr("Insert Line Before"),this);
    actionContext->regAction(m_insertLineBeforeAct,"InsertLineBefore","Ctrl+Shift+Return");
    connect(m_insertLineBeforeAct,SIGNAL(triggered()),m_editorWidget,SLOT(insertLineBefore()));

    m_insertLineAfterAct = new QAction(tr("Insert Line After"),this);
    actionContext->regAction(m_insertLineAfterAct,"InsertLineAfter","Ctrl+Return");
    connect(m_insertLineAfterAct,SIGNAL(triggered()),m_editorWidget,SLOT(insertLineAfter()));

    m_increaseFontSizeAct = new QAction(tr("Increase Font Size"),this);
    actionContext->regAction(m_increaseFontSizeAct,"IncreaseFontSize","Ctrl++;Ctrl+=");

    m_decreaseFontSizeAct = new QAction(tr("Decrease Font Size"),this);
    actionContext->regAction(m_decreaseFontSizeAct,"DecreaseFontSize","Ctrl+-");

    m_resetFontSizeAct = new QAction(tr("Reset Font Size"),this);
    actionContext->regAction(m_resetFontSizeAct,"ResetFontSize","Ctrl+0");

    m_cleanWhitespaceAct = new QAction(tr("Clean Whitespace"),this);
    actionContext->regAction(m_cleanWhitespaceAct,"CleanWhitespace","");

    m_wordWrapAct = new QAction(tr("Word Wrap"),this);
    m_wordWrapAct->setCheckable(true);
    actionContext->regAction(m_wordWrapAct,"WordWrap","");

    m_codeCompleteAct = new QAction(tr("Code Complete"),this);
#ifdef Q_OS_MAC
    actionContext->regAction(m_codeCompleteAct,"CodeComplete","Meta+Space");
#else
    actionContext->regAction(m_codeCompleteAct,"CodeComplete","Ctrl+Space");
#endif
    connect(m_codeCompleteAct,SIGNAL(triggered()),m_editorWidget,SLOT(codeCompleter()));
//    m_widget->addAction(m_foldAct);
//    m_widget->addAction(m_unfoldAct);
//    m_widget->addAction(m_gotoLineAct);

//    m_widget->addAction(m_gotoPrevBlockAct);
//    m_widget->addAction(m_gotoNextBlockAct);
//    m_widget->addAction(m_selectBlockAct);
//    m_widget->addAction(m_gotoMatchBraceAct);

    connect(m_editorWidget,SIGNAL(undoAvailable(bool)),m_undoAct,SLOT(setEnabled(bool)));
    connect(m_editorWidget,SIGNAL(redoAvailable(bool)),m_redoAct,SLOT(setEnabled(bool)));
    connect(m_editorWidget,SIGNAL(copyAvailable(bool)),m_cutAct,SLOT(setEnabled(bool)));
    connect(m_editorWidget,SIGNAL(copyAvailable(bool)),m_copyAct,SLOT(setEnabled(bool)));
    connect(m_editorWidget,SIGNAL(wordWrapChanged(bool)),m_wordWrapAct,SLOT(setChecked(bool)));

    connect(m_undoAct,SIGNAL(triggered()),m_editorWidget,SLOT(undo()));
    connect(m_redoAct,SIGNAL(triggered()),m_editorWidget,SLOT(redo()));
    connect(m_cutAct,SIGNAL(triggered()),m_editorWidget,SLOT(cut()));
    connect(m_copyAct,SIGNAL(triggered()),m_editorWidget,SLOT(copy()));
    connect(m_pasteAct,SIGNAL(triggered()),m_editorWidget,SLOT(paste()));
    connect(m_selectAllAct,SIGNAL(triggered()),m_editorWidget,SLOT(selectAll()));
    connect(m_selectBlockAct,SIGNAL(triggered()),m_editorWidget,SLOT(selectBlock()));

    connect(m_exportHtmlAct,SIGNAL(triggered()),this,SLOT(exportHtml()));
#ifndef QT_NO_PRINTER
    connect(m_exportPdfAct,SIGNAL(triggered()),this,SLOT(exportPdf()));
    connect(m_filePrintAct,SIGNAL(triggered()),this,SLOT(filePrint()));
    connect(m_filePrintPreviewAct,SIGNAL(triggered()),this,SLOT(filePrintPreview()));
#endif
    connect(m_gotoPrevBlockAct,SIGNAL(triggered()),m_editorWidget,SLOT(gotoPrevBlock()));
    connect(m_gotoNextBlockAct,SIGNAL(triggered()),m_editorWidget,SLOT(gotoNextBlock()));
    connect(m_gotoMatchBraceAct,SIGNAL(triggered()),m_editorWidget,SLOT(gotoMatchBrace()));
    connect(m_gotoLineAct,SIGNAL(triggered()),this,SLOT(gotoLine()));
    connect(m_increaseFontSizeAct,SIGNAL(triggered()),this,SLOT(increaseFontSize()));
    connect(m_decreaseFontSizeAct,SIGNAL(triggered()),this,SLOT(decreaseFontSize()));
    connect(m_resetFontSizeAct,SIGNAL(triggered()),this,SLOT(resetFontSize()));
    connect(m_cleanWhitespaceAct,SIGNAL(triggered()),m_editorWidget,SLOT(cleanWhitespace()));
    connect(m_wordWrapAct,SIGNAL(triggered(bool)),m_editorWidget,SLOT(setWordWrapOverride(bool)));

    QClipboard *clipboard = QApplication::clipboard();
    connect(clipboard,SIGNAL(dataChanged()),this,SLOT(clipbordDataChanged()));
    clipbordDataChanged();
}

void LiteEditor::findCodecs()
 {
     QMap<QString, QTextCodec *> codecMap;
     QRegExp iso8859RegExp("ISO[- ]8859-([0-9]+).*");

     foreach (int mib, QTextCodec::availableMibs()) {
         QTextCodec *codec = QTextCodec::codecForMib(mib);

         QString sortKey = codec->name().toUpper();
         int rank;

         if (sortKey.startsWith("UTF-8")) {
             rank = 1;
         } else if (sortKey.startsWith("UTF-16")) {
             rank = 2;
         } else if (iso8859RegExp.exactMatch(sortKey)) {
             if (iso8859RegExp.cap(1).size() == 1)
                 rank = 3;
             else
                 rank = 4;
         } else {
             rank = 5;
         }
         sortKey.prepend(QChar('0' + rank));

         codecMap.insert(sortKey, codec);
     }
     m_codecs = codecMap.values();
}

void LiteEditor::createToolBars()
{
    m_toolBar = new QToolBar("editor",m_widget);
    m_toolBar->setIconSize(LiteApi::getToolBarIconSize(m_liteApp));
    m_toolBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    m_infoToolBar = new QToolBar("info",m_widget);
    m_infoToolBar->setIconSize(LiteApi::getToolBarIconSize(m_liteApp));

    //editor toolbar
    m_toolBar->addAction(m_undoAct);
    m_toolBar->addAction(m_redoAct);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_cutAct);
    m_toolBar->addAction(m_copyAct);
    m_toolBar->addAction(m_pasteAct);
#ifdef LITEEDITOR_FIND
    m_findComboBox = new QComboBox(m_widget);
    m_findComboBox->setEditable(true);
    m_findComboBox->setMinimumWidth(120);
    m_findComboBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    m_toolBar->addWidget(m_findComboBox);
    m_toolBar->addSeparator();
    connect(m_findComboBox->lineEdit(),SIGNAL(returnPressed()),this,SLOT(findNextText()));
#endif

    //info toolbar

    //add lock info
    m_infoToolBar->addAction(m_lockAct);

    //add over info
    QLabel *overInfo = new QLabel("[Over]");
    m_overInfoAct = m_infoToolBar->addWidget(overInfo);
    m_overInfoAct->setVisible(false);

    //add line info
    m_lineInfo = new QLabelEx("000:000");
    m_infoToolBar->addWidget(m_lineInfo);

    //add close
    m_closeEditorAct = new QAction(QIcon("icon:images/closetool.png"),tr("Close Document"),this);
    m_infoToolBar->addAction(m_closeEditorAct);
}

void LiteEditor::createMenu()
{
    m_editMenu = new QMenu(m_editorWidget);
    m_contextMenu = new QMenu(m_editorWidget);

    //editor menu
    m_editMenu->addAction(m_undoAct);
    m_editMenu->addAction(m_redoAct);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_cutAct);
    m_editMenu->addAction(m_copyAct);
    m_editMenu->addAction(m_pasteAct);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_selectAllAct);
    m_editMenu->addSeparator();
    QMenu * subMenu = m_editMenu->addMenu(tr("Advanced"));
    subMenu->addAction(m_duplicateAct);
    subMenu->addAction(m_deleteLineAct);
    subMenu->addAction(m_insertLineBeforeAct);
    subMenu->addAction(m_insertLineAfterAct);
    subMenu->addSeparator();
    subMenu->addAction(m_foldAct);
    subMenu->addAction(m_unfoldAct);
    subMenu->addAction(m_foldAllAct);
    subMenu->addAction(m_unfoldAllAct);
    subMenu->addSeparator();
    subMenu->addAction(m_selectBlockAct);
    subMenu->addAction(m_gotoMatchBraceAct);
    subMenu->addAction(m_gotoPrevBlockAct);
    subMenu->addAction(m_gotoNextBlockAct);
    subMenu->addSeparator();
    subMenu->addAction(m_increaseFontSizeAct);
    subMenu->addAction(m_decreaseFontSizeAct);
    subMenu->addAction(m_resetFontSizeAct);
    subMenu->addSeparator();
    subMenu->addAction(m_cleanWhitespaceAct);
    subMenu->addAction(m_wordWrapAct);
#ifndef QT_NO_PRINTER
    subMenu->addSeparator();
    subMenu->addAction(m_exportPdfAct);
    subMenu->addSeparator();
    subMenu->addAction(m_filePrintPreviewAct);
    subMenu->addAction(m_filePrintAct);
#endif
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_codeCompleteAct);
    m_editMenu->addAction(m_gotoLineAct);

    //context menu
    m_contextMenu->addAction(m_cutAct);
    m_contextMenu->addAction(m_copyAct);
    m_contextMenu->addAction(m_pasteAct);
    m_contextMenu->addAction(m_duplicateAct);
    m_contextMenu->addSeparator();
    subMenu = m_contextMenu->addMenu(tr("Code Folding"));
    subMenu->addAction(m_foldAct);
    subMenu->addAction(m_unfoldAct);
    subMenu->addAction(m_foldAllAct);
    subMenu->addAction(m_unfoldAllAct);
    subMenu = m_contextMenu->addMenu(tr("Line"));
    subMenu->addAction(m_duplicateAct);
    subMenu->addAction(m_deleteLineAct);
    subMenu->addAction(m_insertLineBeforeAct);
    subMenu->addAction(m_insertLineAfterAct);
}

#ifdef LITEEDITOR_FIND
void LiteEditor::findNextText()
{
    QString text = m_findComboBox->lineEdit()->text();
    if (text.isEmpty()) {
        return;
    }
    m_editorWidget->find(text);
}
#endif

LiteApi::IExtension *LiteEditor::extension()
{
    return m_extension;
}

QWidget *LiteEditor::widget()
{
    return m_widget;
}

QString LiteEditor::name() const
{
    return QFileInfo(m_file->filePath()).fileName();
}

QIcon LiteEditor::icon() const
{
    return QIcon();
}

bool LiteEditor::createNew(const QString &contents, const QString &mimeType)
{
    bool success = m_file->create(contents,mimeType);
    if (success) {
        m_editorWidget->initLoadDocument();
        setReadOnly(m_file->isReadOnly());
    }
    return success;
}

bool LiteEditor::open(const QString &fileName,const QString &mimeType)
{
    bool success = m_file->open(fileName,mimeType);
    if (success) {        
        m_editorWidget->initLoadDocument();
        setReadOnly(m_file->isReadOnly());
    }
    return success;
}

bool LiteEditor::reload()
{
    QByteArray state = this->saveState();
    bool success = open(filePath(),mimeType());
    if (success) {
        this->clearAllNavigateMarks();
        this->setNavigateHead(LiteApi::EditorNavigateReload,tr("Reload File"));
        this->restoreState(state);
        emit reloaded();
    }
    return success;
}

bool LiteEditor::save()
{
    if (m_bReadOnly) {
        return false;
    }
    return saveAs(m_file->filePath());
}

bool LiteEditor::saveAs(const QString &fileName)
{
    bool cleanWhitespaceonSave = m_liteApp->settings()->value(EDITOR_CLEANWHITESPACEONSAVE,false).toBool();
    if (cleanWhitespaceonSave) {
        m_editorWidget->cleanWhitespace(true);
    }
    return m_file->save(fileName);
}

void LiteEditor::setReadOnly(bool b)
{
    m_lockAct->setVisible(b);
    m_bReadOnly = b;
}

bool LiteEditor::isReadOnly() const
{
    return m_bReadOnly;
}

bool LiteEditor::isModified() const
{
    if (!m_file) {
        return false;
    }
    return m_file->document()->isModified();
}

QString LiteEditor::filePath() const
{
    if (!m_file) {
        return QString();
    }
    return m_file->filePath();
}

QString LiteEditor::mimeType() const
{
    if (!m_file) {
        return QString();
    }
    return m_file->mimeType();
}

LiteApi::IFile *LiteEditor::file()
{
    return m_file;
}

int LiteEditor::line() const
{
    return m_editorWidget->textCursor().blockNumber();
}

int LiteEditor::column() const
{
    return m_editorWidget->textCursor().columnNumber();
}

int LiteEditor::utf8Position() const
{
    QTextCursor cur = m_editorWidget->textCursor();
    QString src = cur.document()->toPlainText().left(cur.position());
    int offset = 0;
//    if (m_file->m_lineTerminatorMode == LiteEditorFile::CRLFLineTerminator) {
//       offset = cur.blockNumber();
//    }
    return src.toUtf8().length()+offset+1;
}

QByteArray LiteEditor::utf8Data() const {
    QString src = m_editorWidget->document()->toPlainText();
//    if (m_file->m_lineTerminatorMode == LiteEditorFile::CRLFLineTerminator) {
//        src = src.replace("\n","\r\n");
//    }
    return src.toUtf8();
}

void LiteEditor::setWordWrap(bool wrap)
{
    m_editorWidget->setWordWrapOverride(wrap);
}

bool LiteEditor::wordWrap() const
{
    return m_editorWidget->isWordWrap();
}

void LiteEditor::gotoLine(int line, int column, bool center)
{
    m_editorWidget->setFocus();
    m_editorWidget->gotoLine(line,column,center);
}

void LiteEditor::applyOption(QString id)
{
    if (id != OPTION_LITEEDITOR) {
        return;
    }

    bool autoIndent = m_liteApp->settings()->value(EDITOR_AUTOINDENT,true).toBool();
    bool autoBraces0 = m_liteApp->settings()->value(EDITOR_AUTOBRACE0,true).toBool();
    bool autoBraces1 = m_liteApp->settings()->value(EDITOR_AUTOBRACE1,true).toBool();
    bool autoBraces2 = m_liteApp->settings()->value(EDITOR_AUTOBRACE2,true).toBool();
    bool autoBraces3 = m_liteApp->settings()->value(EDITOR_AUTOBRACE3,true).toBool();
    bool autoBraces4 = m_liteApp->settings()->value(EDITOR_AUTOBRACE4,true).toBool();
    bool caseSensitive = m_liteApp->settings()->value(EDITOR_COMPLETER_CASESENSITIVE,false).toBool();
    bool lineNumberVisible = m_liteApp->settings()->value(EDITOR_LINENUMBERVISIBLE,true).toBool();
    bool rightLineVisible = m_liteApp->settings()->value(EDITOR_RIGHTLINEVISIBLE,true).toBool();
    bool eofVisible = m_liteApp->settings()->value(EDITOR_EOFVISIBLE,false).toBool();
    bool defaultWordWrap = m_liteApp->settings()->value(EDITOR_DEFAULTWORDWRAP,false).toBool();
    bool indentLineVisible = m_liteApp->settings()->value(EDITOR_INDENTLINEVISIBLE,true).toBool();
    bool wheelZooming = m_liteApp->settings()->value(EDITOR_WHEEL_SCROLL,true).toBool();
    int rightLineWidth = m_liteApp->settings()->value(EDITOR_RIGHTLINEWIDTH,80).toInt();
    int min = m_liteApp->settings()->value(EDITOR_PREFIXLENGTH,1).toInt();
    m_editorWidget->setPrefixMin(min);

    m_offsetVisible = m_liteApp->settings()->value(EDITOR_OFFSETVISIBLE,false).toBool();

    m_editorWidget->setAutoIndent(autoIndent);
    m_editorWidget->setAutoBraces0(autoBraces0);
    m_editorWidget->setAutoBraces1(autoBraces1);
    m_editorWidget->setAutoBraces2(autoBraces2);
    m_editorWidget->setAutoBraces3(autoBraces3);
    m_editorWidget->setAutoBraces4(autoBraces4);
    m_editorWidget->setLineNumberVisible(lineNumberVisible);
    m_editorWidget->setEofVisible(eofVisible);
    m_editorWidget->setIndentLineVisible(indentLineVisible);
    m_editorWidget->setRightLineVisible(rightLineVisible);
    m_editorWidget->setRightLineWidth(rightLineWidth);
    m_editorWidget->setDefaultWordWrap(defaultWordWrap);
    m_editorWidget->setScrollWheelZooming(wheelZooming);

    if (m_completer) {
        m_completer->completer()->setCaseSensitivity(caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
    }

#if defined(Q_OS_WIN)
    QString fontFamily = m_liteApp->settings()->value(EDITOR_FAMILY,"Courier").toString();
#elif defined(Q_OS_LINUX)
    QString fontFamily = m_liteApp->settings()->value(EDITOR_FAMILY,"Monospace").toString();
#elif defined(Q_OS_MAC)
    QString fontFamily = m_liteApp->settings()->value(EDITOR_FAMILY,"Menlo").toString();
#else
    QString fontFamily = m_liteApp->settings()->value(EDITOR_FAMILY,"Monospace").toString();
#endif
    int fontSize = m_liteApp->settings()->value(EDITOR_FONTSIZE,12).toInt();
    int fontZoom = m_liteApp->settings()->value(EDITOR_FONTZOOM,100).toInt();    
    bool antialias = m_liteApp->settings()->value(EDITOR_ANTIALIAS,true).toBool();
    QFont font = m_editorWidget->font();
    font.setFamily(fontFamily);
    font.setPointSize(fontSize*fontZoom/100.0);
    if (antialias) {
        font.setStyleStrategy(QFont::PreferAntialias);
    } else {
        font.setStyleStrategy(QFont::NoAntialias);
    }
    m_editorWidget->setFont(font);
    m_editorWidget->extraArea()->setFont(font);
    m_editorWidget->slotUpdateExtraAreaWidth();

    QString mime = this->m_file->mimeType();
    int tabWidth = m_liteApp->settings()->value(EDITOR_TABWIDTH+mime,4).toInt();
    m_editorWidget->setTabSize(tabWidth);
    bool useSpace = m_liteApp->settings()->value(EDITOR_TABUSESPACE+mime,false).toBool();
    m_editorWidget->setTabUseSpace(useSpace);

    emit tabSettingChanged(tabWidth);
}

void LiteEditor::updateTip(QString func,QString args)
{
    if (args.isEmpty()) {
        return;
    }
    QString tip = QString("%1 %2").arg(func).arg(args);
    //m_liteApp->outputManager()->setStatusInfo(tip);
}

void LiteEditor::filePrintPreview()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, m_widget);
    preview.resize(this->m_editorWidget->size());
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
#endif
}

void LiteEditor::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    QPlainTextEdit::LineWrapMode mode = m_editorWidget->lineWrapMode();
    m_editorWidget->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_editorWidget->print(printer);
    m_editorWidget->setLineWrapMode(mode);
#endif
}

void LiteEditor::exportHtml()
{
    QString title;
    if (m_file) {
        title = QFileInfo(m_file->filePath()).completeBaseName();
    }
    QString fileName = QFileDialog::getSaveFileName(m_widget, tr("Export HTML"),
                                                    title, "*.html");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".html");
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QMessageBox::critical(m_widget,
                                  tr("Export Failed"),
                                  QString(tr("Could not open %1 for writing.")).arg(fileName)
                                  );
            return;
        }
        QTextCursor cur = m_editorWidget->textCursor();
        cur.select(QTextCursor::Document);
        file.write(m_editorWidget->cursorToHtml(cur).toUtf8());
        file.close();
    }
}

void LiteEditor::exportPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
    QString title;
    if (m_file) {
        title = QFileInfo(m_file->filePath()).completeBaseName();
    }
    QString fileName = QFileDialog::getSaveFileName(m_widget, tr("Export PDF"),
                                                    title, "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty()) {
            fileName.append(".pdf");
        }
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        QPlainTextEdit::LineWrapMode mode = m_editorWidget->lineWrapMode();
        m_editorWidget->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        m_editorWidget->print(&printer);
        m_editorWidget->setLineWrapMode(mode);
    }
//! [0]
#endif
}

void LiteEditor::filePrint()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, m_widget);
    if (m_editorWidget->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        QPlainTextEdit::LineWrapMode mode = m_editorWidget->lineWrapMode();
        m_editorWidget->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        m_editorWidget->print(&printer);
        m_editorWidget->setLineWrapMode(mode);
    }
    delete dlg;
#endif
}

void LiteEditor::codecComboBoxChanged(QString codec)
{
    if (!m_file) {
        return;
    }
    if (m_file->document()->isModified()) {
        QString text = QString(tr("Do you want to permanently discard unsaved modifications and reload %1?")).arg(m_file->filePath());
        int ret = QMessageBox::question(m_liteApp->mainWindow(),"Unsaved Modifications",text,QMessageBox::Yes|QMessageBox::No);
        if (ret != QMessageBox::Yes) {
            return;
        }
    }
    bool success = m_file->reloadByCodec(codec);
    if (success) {
        emit reloaded();
        m_editorWidget->initLoadDocument();
        setReadOnly(m_file->isReadOnly());
    }
    return;
}

void LiteEditor::editPositionChanged()
{
     QTextCursor cur = m_editorWidget->textCursor();
/*
     QString src = cur.document()->toPlainText().left(cur.position());
     int offset = 0;
     if (m_file->m_lineTerminatorMode == LiteEditorFile::CRLFLineTerminator) {
        offset = cur.blockNumber();
     }
*/
     //m_liteApp->editorManager()->updateLine(this,cur.blockNumber()+1,cur.columnNumber()+1, src.toUtf8().length()+offset+1);
     if (m_offsetVisible) {
         m_lineInfo->setText(QString("%1:%2 [%3]").arg(cur.blockNumber()+1,3).arg(cur.columnNumber()+1,3).arg(cur.position()));
     } else {
         m_lineInfo->setText(QString("%1:%2").arg(cur.blockNumber()+1,3).arg(cur.columnNumber()+1,3));
     }
}

void LiteEditor::gotoLine()
{
    int min = 1;
    int max = m_editorWidget->document()->blockCount();
    int v = m_editorWidget->textCursor().blockNumber()+1;
    bool ok = false;
    v = QInputDialog::getInt(this->m_widget,tr("Go To Line"),tr("Line: ")+QString("%1-%2").arg(min).arg(max),v,min,max,1,&ok);
    if (!ok) {
        return;
    }
    this->gotoLine(v-1,0,true);
}

QString LiteEditor::textCodec() const
{
    return m_file->textCodec();
}

void LiteEditor::setTextCodec(const QString &codec)
{
    bool success = m_file->reloadByCodec(codec);
    if (success) {
        m_editorWidget->initLoadDocument();
        setReadOnly(m_file->isReadOnly());
        emit reloaded();
    }
}

QByteArray LiteEditor::saveState() const
{
    return m_editorWidget->saveState();
}

bool LiteEditor::restoreState(const QByteArray &array)
{
    return m_editorWidget->restoreState(array);
}

void LiteEditor::navigationStateChanged(const QByteArray &state)
{
    m_liteApp->editorManager()->addNavigationHistory(this,state);
}

void LiteEditor::onActive()
{
    m_editorWidget->setFocus();
    //clipbordDataChanged();
    //editPositionChanged();
}

void LiteEditor::setFindOption(LiteApi::FindOption *opt)
{
    m_editorWidget->setFindOption(opt);
}

void LiteEditor::setSpellCheckZoneDontComplete(bool b)
{
    m_editorWidget->setSpellCheckZoneDontComplete(b);
}

void LiteEditor::setNavigateHead(LiteApi::EditorNaviagteType type, const QString &msg)
{
    m_editorWidget->setNavigateHead(type,msg);
}

void LiteEditor::insertNavigateMark(int line, LiteApi::EditorNaviagteType type, const QString &msg, const char* tag = "")
{
    m_editorWidget->insertNavigateMark(line,type,msg, tag);
}

void LiteEditor::clearNavigateMarak(int /*line*/)
{
}

void LiteEditor::clearAllNavigateMarks()
{
    m_editorWidget->clearAllNavigateMarks();
}

void LiteEditor::clearAllNavigateMark(LiteApi::EditorNaviagteType types, const char *tag = "")
{
    m_editorWidget->clearAllNavigateMark(types, tag);
}

void LiteEditor::selectNextParam()
{
    QTextCursor cur = m_editorWidget->textCursor();
    int pos = cur.position();
    if (cur.hasSelection()) {
        pos = cur.selectionEnd();
    }
    QTextBlock block = cur.block();
    int offset = pos-block.position();
    QRegExp reg("[\\,\\(\\)\\.\\s](\\s*)([\"\'\\w]+)");
    int index = reg.indexIn(block.text().mid(offset));
    if (index >= 0) {
        //qDebug() << reg.capturedTexts();
        int start = block.position()+offset+index+1+reg.cap(1).length();
        cur.setPosition(start);
        cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,reg.cap(2).length());
        m_editorWidget->setTextCursor(cur);
    }
}

void LiteEditor::increaseFontSize()
{
    this->requestFontZoom(10);
}

void LiteEditor::decreaseFontSize()
{
    this->requestFontZoom(-10);
}

void LiteEditor::resetFontSize()
{
    int fontSize = m_liteApp->settings()->value(EDITOR_FONTSIZE,12).toInt();
    m_liteApp->settings()->setValue(EDITOR_FONTZOOM,100);
    QFont font = m_editorWidget->font();
    font.setPointSize(fontSize);
    m_editorWidget->setFont(font);
    m_editorWidget->extraArea()->setFont(font);
    m_editorWidget->updateTabWidth();
    m_editorWidget->slotUpdateExtraAreaWidth();
}

void LiteEditor::setEditToolbarVisible(bool visible)
{
    m_toolBar->setVisible(visible);
    m_infoToolBar->setVisible(visible);
}


QLabelEx::QLabelEx(const QString &text, QWidget *parent) :
    QLabel(text,parent)
{
}

void QLabelEx::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit doubleClickEvent();
    }
}

void LiteEditor::requestFontZoom(int zoom)
{
    int fontSize = m_liteApp->settings()->value(EDITOR_FONTSIZE,12).toInt();
    int fontZoom = m_liteApp->settings()->value(EDITOR_FONTZOOM,100).toInt();
    fontZoom += zoom;
    if (fontZoom <= 10) {
        return;
    }
    m_liteApp->settings()->setValue(EDITOR_FONTZOOM,fontZoom);

    QFont font = m_editorWidget->font();
    font.setPointSize(fontSize*fontZoom/100.0);
    m_editorWidget->setFont(font);
    m_editorWidget->extraArea()->setFont(font);
    m_editorWidget->updateTabWidth();
    m_editorWidget->slotUpdateExtraAreaWidth();
}

void LiteEditor::loadColorStyleScheme()
{
    const ColorStyleScheme *colorScheme = m_liteApp->editorManager()->colorStyleScheme();
    const ColorStyle *extra = colorScheme->findStyle("Extra");
    const ColorStyle *indentLine = colorScheme->findStyle("IndentLine");
    const ColorStyle *text = colorScheme->findStyle("Text");
    const ColorStyle *selection = colorScheme->findStyle("Selection");
    const ColorStyle *currentLine = colorScheme->findStyle("CurrentLine");
    if (extra) {
        m_editorWidget->setExtraColor(extra->foregound(),extra->background());
    }
    if (indentLine) {
        m_editorWidget->setIndentLineColor(indentLine->foregound());
    }
    if (currentLine) {
        m_editorWidget->setCurrentLineColor(currentLine->background());
    }
    QPalette p = m_defEditorPalette;
    if (text) {
        if (text->foregound().isValid()) {
            p.setColor(QPalette::Text,text->foregound());
            p.setColor(QPalette::Foreground, text->foregound());
        }
        if (text->background().isValid()) {
            p.setColor(QPalette::Base, text->background());
        }
    }
    if (selection) {
        if (selection->foregound().isValid()) {
            p.setColor(QPalette::HighlightedText, selection->foregound());
        }
        if (selection->background().isValid()) {
            p.setColor(QPalette::Highlight, selection->background());
        }
        p.setBrush(QPalette::Inactive, QPalette::Highlight, p.highlight());
        p.setBrush(QPalette::Inactive, QPalette::HighlightedText, p.highlightedText());
    }

    QString sheet = QString("QPlainTextEdit{color:%1;background-color:%2;selection-color:%3;selection-background-color:%4;}")
                .arg(p.text().color().name())
                .arg(p.base().color().name())
                .arg(p.highlightedText().color().name())
                .arg(p.highlight().color().name());
    m_editorWidget->setPalette(p);
#ifdef Q_OS_MAC
    #if QT_VERSION >= 0x050000
        m_editorWidget->setStyleSheet(sheet);
    #else
        if (QSysInfo::MacintoshVersion <= QSysInfo::MV_10_8) {
            m_editorWidget->setStyleSheet(sheet);
        }
    #endif
#else
    m_editorWidget->setStyleSheet(sheet);
#endif
    emit colorStyleChanged();
}
