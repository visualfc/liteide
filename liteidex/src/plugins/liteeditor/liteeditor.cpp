/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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

    m_colorStyleScheme = new ColorStyleScheme(this);
    m_defPalette = m_editorWidget->palette();

    createActions();
    createToolBars();
    createMenu();

    m_editorWidget->setContextMenu(m_contextMenu);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

//    m_toolBar->setStyleSheet("QToolBar {border: 1px ; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #eeeeee, stop: 1 #ababab); }"\
//                             "QToolBar QToolButton { border:1px ; border-radius: 1px; }"\
//                             "QToolBar QToolButton::hover { background-color: #ababab;}"\
//                             "QToolBar::separator {width:2px; margin-left:2px; margin-right:2px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #dedede, stop: 1 #a0a0a0);}");

    QHBoxLayout *toolLayout = new QHBoxLayout;
    toolLayout->setMargin(0);
    toolLayout->setSpacing(0);
    toolLayout->addWidget(m_toolBar);
    toolLayout->addWidget(m_infoToolBar);

    layout->addLayout(toolLayout);
    layout->addWidget(m_editorWidget);
    m_widget->setLayout(layout);
    m_file = new LiteEditorFile(m_liteApp,this);
    m_file->setDocument(m_editorWidget->document());

//    QTextOption option =  m_editorWidget->document()->defaultTextOption();
//    option.setFlags(option.flags() | QTextOption::ShowTabsAndSpaces);
//    option.setFlags(option.flags() | QTextOption::AddSpaceForLineAndParagraphSeparators);
//    option.setTabs(tabs);

//    m_editorWidget->document()->setDefaultTextOption(option);

    connect(m_file->document(),SIGNAL(modificationChanged(bool)),this,SIGNAL(modificationChanged(bool)));
    connect(m_file->document(),SIGNAL(contentsChanged()),this,SIGNAL(contentsChanged()));
    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));

    //applyOption("option/liteeditor");

    m_extension->addObject("LiteApi.ITextEditor",this);
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

const ColorStyleScheme *LiteEditor::colorStyleScheme() const
{
    return m_colorStyleScheme;
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

void LiteEditor::setCompleter(LiteCompleter *complter)
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

    connect(m_editorWidget,SIGNAL(completionPrefixChanged(QString)),m_completer,SLOT(completionPrefixChanged(QString)));
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
    m_undoAct = new QAction(QIcon("icon:liteeditor/images/undo.png"),tr("Undo"),this);
    m_liteApp->actionManager()->regAction(m_undoAct,"LiteEditor.Undo",QKeySequence::Undo);

    m_redoAct = new QAction(QIcon("icon:liteeditor/images/redo.png"),tr("Redo"),this);
    m_liteApp->actionManager()->regAction(m_redoAct,"LiteEditor.Redo","Ctrl+Shift+Z; Ctrl+Y");

    m_cutAct = new QAction(QIcon("icon:liteeditor/images/cut.png"),tr("Cut"),this);
    m_liteApp->actionManager()->regAction(m_cutAct,"LiteEditor.Cut",QKeySequence::Cut);

    m_copyAct = new QAction(QIcon("icon:liteeditor/images/copy.png"),tr("Copy"),this);
    m_liteApp->actionManager()->regAction(m_copyAct,"LiteEditor.Copy",QKeySequence::Copy);

    m_pasteAct = new QAction(QIcon("icon:liteeditor/images/paste.png"),tr("Paste"),this);
    m_liteApp->actionManager()->regAction(m_pasteAct,"LiteEditor.Paste",QKeySequence::Paste);

    m_selectAllAct = new QAction(tr("Select All"),this);
    m_liteApp->actionManager()->regAction(m_selectAllAct,"LiteEditor.SelectAll",QKeySequence::SelectAll);

    m_exportHtmlAct = new QAction(QIcon("icon:liteeditor/images/exporthtml.png"),tr("Export HTML"),this);
#ifndef QT_NO_PRINTER
    m_exportPdfAct = new QAction(QIcon("icon:liteeditor/images/exportpdf.png"),tr("Export PDF"),this);
    m_filePrintAct = new QAction(QIcon("icon:liteeditor/images/fileprint.png"),tr("Print Document"),this);
    m_filePrintPreviewAct = new QAction(QIcon("icon:liteeditor/images/fileprintpreview.png"),tr("Print Preview Document"),this);
#endif
    m_gotoPrevBlockAct = new QAction(tr("Goto Previous Block"),this);
    m_liteApp->actionManager()->regAction(m_gotoPrevBlockAct,"LiteEditor.GotoPreviousBlock","Ctrl+[");

    m_gotoNextBlockAct = new QAction(tr("Goto Next Block"),this);
    m_liteApp->actionManager()->regAction(m_gotoNextBlockAct,"LiteEditor.GotoNextBlock","Ctrl+]");


    m_selectBlockAct = new QAction(tr("Select Block"),this);
    m_liteApp->actionManager()->regAction(m_selectBlockAct,"LiteEditor.SelectBlock","Ctrl+U");

    m_gotoMatchBraceAct = new QAction(tr("Goto Match Brace"),this);
    m_liteApp->actionManager()->regAction(m_gotoMatchBraceAct,"LiteEditor.GotoMatchBrace","Ctrl+E");

    m_foldAct = new QAction(tr("Fold"),this);   
    m_liteApp->actionManager()->regAction(m_foldAct,"LiteEditor.Fold","Ctrl+<");

    m_unfoldAct = new QAction(tr("Unfold"),this);
    m_liteApp->actionManager()->regAction(m_unfoldAct,"LiteEditor.Unfold","Ctrl+>");

    m_foldAllAct = new QAction(tr("Fold All"),this);
    m_liteApp->actionManager()->regAction(m_foldAllAct,"LiteEditor.FoldAll","");

    m_unfoldAllAct = new QAction(tr("Unfold All"),this);
    m_liteApp->actionManager()->regAction(m_unfoldAllAct,"LiteEditor.UnfoldAll","");

    connect(m_foldAct,SIGNAL(triggered()),m_editorWidget,SLOT(fold()));
    connect(m_unfoldAct,SIGNAL(triggered()),m_editorWidget,SLOT(unfold()));
    connect(m_foldAllAct,SIGNAL(triggered()),m_editorWidget,SLOT(foldAll()));
    connect(m_unfoldAllAct,SIGNAL(triggered()),m_editorWidget,SLOT(unfoldAll()));

    m_gotoLineAct = new QAction(tr("Goto Line"),this);
    m_liteApp->actionManager()->regAction(m_gotoLineAct,"LiteEditor.GotoLine","Ctrl+L");

    m_lockAct = new QAction(QIcon("icon:liteeditor/images/lock.png"),tr("Locked"),this);
    m_lockAct->setEnabled(false);

    m_duplicateAct = new QAction(tr("Duplicate"),this);
    m_liteApp->actionManager()->regAction(m_duplicateAct,"LiteEditor.Duplicate","Ctrl+D");

    connect(m_duplicateAct,SIGNAL(triggered()),m_editorWidget,SLOT(duplicate()));

    m_deleteLineAct = new QAction(tr("Delete Line"),this);
    m_liteApp->actionManager()->regAction(m_deleteLineAct,"LiteEditor.DeleteLine","Ctrl+Shift+K");

    connect(m_deleteLineAct,SIGNAL(triggered()),m_editorWidget,SLOT(deleteLine()));

    m_increaseFontSizeAct = new QAction(tr("Increase Font Size"),this);
    m_liteApp->actionManager()->regAction(m_increaseFontSizeAct,"LiteEditor.IncreaseFontSize","Ctrl++;Ctrl+=");

    m_decreaseFontSizeAct = new QAction(tr("Decrease Font Size"),this);
    m_liteApp->actionManager()->regAction(m_decreaseFontSizeAct,"LiteEditor.DecreaseFontSize","Ctrl+-");

    m_resetFontSizeAct = new QAction(tr("Reset Font Size"),this);
    m_liteApp->actionManager()->regAction(m_resetFontSizeAct,"LiteEditor.ResetFontSize","Ctrl+0");


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
    m_toolBar->setIconSize(LiteApi::getToolBarIconSize());
    m_toolBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    m_infoToolBar = new QToolBar("info",m_widget);
    m_infoToolBar->setIconSize(LiteApi::getToolBarIconSize());

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
    m_editMenu->addAction(m_duplicateAct);
    m_editMenu->addAction(m_deleteLineAct);
    m_editMenu->addSeparator();
    QMenu *expMenu = m_editMenu->addMenu(tr("Print"));
    //expMenu->addAction(m_exportHtmlAct);
#ifndef QT_NO_PRINTER
    expMenu->addAction(m_exportPdfAct);
    expMenu->addSeparator();
    expMenu->addAction(m_filePrintPreviewAct);
    expMenu->addAction(m_filePrintAct);
#endif
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_foldAct);
    m_editMenu->addAction(m_unfoldAct);
    m_editMenu->addAction(m_foldAllAct);
    m_editMenu->addAction(m_unfoldAllAct);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_selectAllAct);
    m_editMenu->addAction(m_selectBlockAct);
    m_editMenu->addAction(m_gotoLineAct);
    m_editMenu->addAction(m_gotoMatchBraceAct);
    m_editMenu->addAction(m_gotoPrevBlockAct);
    m_editMenu->addAction(m_gotoNextBlockAct);

    m_editMenu->addSeparator();
    m_editMenu->addAction(m_increaseFontSizeAct);
    m_editMenu->addAction(m_decreaseFontSizeAct);
    m_editMenu->addAction(m_resetFontSizeAct);

    //context menu
    m_contextMenu->addAction(m_cutAct);
    m_contextMenu->addAction(m_copyAct);
    m_contextMenu->addAction(m_pasteAct);
    m_contextMenu->addAction(m_duplicateAct);
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
    bool success = open(filePath(),mimeType());
    if (success) {
        emit reloaded();
    }
    return success;
}

bool LiteEditor::save()
{
    if (m_bReadOnly) {
        return false;
    }
    return m_file->save(m_file->filePath());
}

bool LiteEditor::saveAs(const QString &fileName)
{
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
    bool caseSensitive = m_liteApp->settings()->value(EDITOR_COMPLETER_CASESENSITIVE,true).toBool();
    bool lineNumberVisible = m_liteApp->settings()->value(EDITOR_LINENUMBERVISIBLE,true).toBool();
    bool rightLineVisible = m_liteApp->settings()->value(EDITOR_RIGHTLINEVISIBLE,true).toBool();
    bool eofVisible = m_liteApp->settings()->value(EDITOR_EOFVISIBLE,false).toBool();
    int rightLineWidth = m_liteApp->settings()->value(EDITOR_RIGHTLINEWIDTH,80).toInt();
    int min = m_liteApp->settings()->value(EDITOR_PREFIXLENGTH,1).toInt();
    m_editorWidget->setPrefixMin(min);

    m_editorWidget->setAutoIndent(autoIndent);
    m_editorWidget->setAutoBraces0(autoBraces0);
    m_editorWidget->setAutoBraces1(autoBraces1);
    m_editorWidget->setAutoBraces2(autoBraces2);
    m_editorWidget->setAutoBraces3(autoBraces3);
    m_editorWidget->setAutoBraces4(autoBraces4);
    m_editorWidget->setLineNumberVisible(lineNumberVisible);
    m_editorWidget->setEofVisible(eofVisible);
    m_editorWidget->setRightLineVisible(rightLineVisible);
    m_editorWidget->setRightLineWidth(rightLineWidth);

    if (m_completer) {
        m_completer->completer()->setCaseSensitivity(caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
    }

#if defined(Q_OS_WIN)
    QString fontFamily = m_liteApp->settings()->value(EDITOR_FAMILY,"Courier").toString();
#elif defined(Q_OS_LINUX)
    QString fontFamily = m_liteApp->settings()->value(EDITOR_FAMILY,"Monospace").toString();
#elif defined(Q_OS_MAC)
    QString fontFamily = m_liteApp->settings()->value(EDITOR_FAMILY,"Menlo").toString();
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

    QString mime = this->m_file->mimeType();
    int tabWidth = m_liteApp->settings()->value(EDITOR_TABWIDTH+mime,4).toInt();
    m_editorWidget->setTabSize(tabWidth);
    bool useSpace = m_liteApp->settings()->value(EDITOR_TABUSESPACE+mime,false).toBool();
    m_editorWidget->setTabUseSpace(useSpace);

    QString style = m_liteApp->settings()->value(EDITOR_STYLE,"default.xml").toString();
    if (style != m_colorStyle) {
        m_colorStyle = style;
        m_colorStyleScheme->clear();
        QString styleFileName = m_liteApp->resourcePath()+"/liteeditor/color/"+m_colorStyle;
        bool b = m_colorStyleScheme->load(styleFileName);
        if (b) {
            const ColorStyle *extra = m_colorStyleScheme->findStyle("Extra");
            const ColorStyle *indentLine = m_colorStyleScheme->findStyle("IndentLine");
            const ColorStyle *style = m_colorStyleScheme->findStyle("Text");
            const ColorStyle *selection = m_colorStyleScheme->findStyle("Selection");
            const ColorStyle *inactiveSelection = m_colorStyleScheme->findStyle("InactiveSelection");
            const ColorStyle *currentLine = m_colorStyleScheme->findStyle("CurrentLine");
            if (extra) {
                m_editorWidget->setExtraColor(extra->foregound(),extra->background());
            }
            if (indentLine) {
                m_editorWidget->setIndentLineColor(indentLine->foregound());
            }
            if (currentLine) {
                m_editorWidget->setCurrentLineColor(currentLine->background());
            }
            if (style || selection || inactiveSelection) {
                QPalette p = m_defPalette;
                if (style) {
                    if (style->foregound().isValid()) {
                        p.setColor(QPalette::Text,style->foregound());
                        p.setColor(QPalette::Foreground, style->foregound());
                    }
                    if (style->background().isValid()) {
                        //p.setColor(QPalette::Background, style->background());
                        p.setBrush(QPalette::Base, style->background());
                    }
                }
                if (selection) {
                    if (selection->foregound().isValid()) {
                        p.setBrush(QPalette::HighlightedText,selection->foregound().isValid()?
                                       selection->foregound() : m_defPalette.foreground());
                    }
                    if (selection->background().isValid()) {
                        p.setBrush(QPalette::Highlight,selection->background());
                    }
                }
                if (inactiveSelection) {
                    if (inactiveSelection->foregound().isValid()) {
                        p.setBrush(QPalette::Inactive,QPalette::HighlightedText,inactiveSelection->foregound());
                    }
                    if (inactiveSelection->background().isValid()) {
                        p.setBrush(QPalette::Inactive,QPalette::Highlight,inactiveSelection->background());
                    }
                }
                m_editorWidget->setPalette(p);
            } else {
                m_editorWidget->setPalette(m_defPalette);
            }
            emit colorStyleChanged();
        }
    }
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
                                  tr("LiteIDE"),
                                  QString(tr("Can not write file %1")).arg(fileName)
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
        QString text = QString(tr("Cancel file %1 modify and reload ?")).arg(m_file->filePath());
        int ret = QMessageBox::question(m_liteApp->mainWindow(),"LiteIDE X",text,QMessageBox::Yes|QMessageBox::No);
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
     m_lineInfo->setText(QString("%1:%2 ").arg(cur.blockNumber()+1,3).arg(cur.columnNumber()+1,3));
}

void LiteEditor::gotoLine()
{
    int min = 1;
    int max = m_editorWidget->document()->lineCount();
    int v = m_editorWidget->textCursor().blockNumber()+1;
    bool ok = false;
    v = QInputDialog::getInt(this->m_widget,tr("Goto Line"),tr("Line: ")+QString("%1-%2").arg(min).arg(max),v,min,max,1,&ok);
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
    //clipbordDataChanged();
    //editPositionChanged();
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
