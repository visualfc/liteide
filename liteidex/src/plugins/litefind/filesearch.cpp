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
// Module: filesearch.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "filesearch.h"
#include <QFile>
#include <QTableWidget>
#include <QTextStream>
#include <QTextDocument>
#include <QRegExp>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDir>
#include <QTextBlock>
#include <QTextCursor>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QFileDialog>
#include <QAction>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


FindThread::FindThread(QObject *parent) :
    QThread(parent),
    useRegExp(true),
    matchWord(true),
    matchCase(true),
    findSub(true)
{
    qRegisterMetaType<LiteApi::FileSearchResult>("LiteApi::FileSearchResult");
}

void FindThread::findDir(const QRegExp &reg, const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    foreach(QFileInfo info, dir.entryInfoList(nameFilter,QDir::Files|QDir::NoSymLinks)) {
        findFile(reg,info.filePath());
        if (!finding) {
            return;
        }
    }
    if (findSub) {
        foreach(QFileInfo info, dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot)) {
            findDir(reg,info.filePath());
            if(!finding) {
                return;
            }
        }
    }
}

void FindThread::findFile(const QRegExp &reg, const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QTextStream stream(&file);
    stream.setCodec("utf-8");
    QString line;
    int lineNr = 1;
    while (!stream.atEnd()) {
        line = stream.readLine();
        int pos = 0;
        while ((pos = reg.indexIn(line, pos)) != -1) {
            if (!useRegExp && matchWord) {
                const int start = pos;
                const int end = start + reg.matchedLength();
                if ((start != 0 && line.at(start - 1).isLetterOrNumber())
                        || (end != line.length() && line.at(end).isLetterOrNumber())) {
                    //if this is not a whole word, continue the search in the string
                    pos = end+1;
                    continue;
                }
            }
            emit findResult(LiteApi::FileSearchResult(fileName, line,lineNr,pos, reg.matchedLength()));
            pos += reg.matchedLength();
        }
        lineNr++;
        if (!finding) {
            break;
        }
    }
}


void FindThread::stop()
{
    finding = false;
    if (this->isRunning()) {
        if (!this->wait(200))
            this->terminate();
    }
}

void FindThread::run()
{
    finding = true;
    QRegExp reg;
    reg.setCaseSensitivity(matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);

    if (useRegExp) {
        if (matchWord) {
            reg.setPattern(QString::fromLatin1("\\b%1\\b").arg(findText));
        } else {
            reg.setPattern(findText);
        }
    } else {
       reg.setPattern(findText);
       reg.setPatternSyntax(QRegExp::FixedString);
    }
    findDir(reg,findPath);
    finding = false;
}

ResultTextEdit::ResultTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    this->setWordWrapMode(QTextOption::NoWrap);

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()));
}

void ResultTextEdit::slotCursorPositionChanged()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(180,200,200,128);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);

    setExtraSelections(extraSelections);
}

void ResultTextEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    QTextCursor cur = cursorForPosition(e->pos());
    cur.select(QTextCursor::LineUnderCursor);

    emit dbclickEvent(cur);
}

FileSearch::FileSearch(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IFileSearch(parent),
    m_liteApp(app)
{
    m_thread = new FindThread;

    m_findWidget = new QWidget;

    QGridLayout *topLayout = new QGridLayout;
    topLayout->setSpacing(1);

    m_findCombo = new QComboBox;
    m_findCombo->setEditable(true);
    m_findCombo->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    QHBoxLayout *optLayout = new QHBoxLayout;
    m_matchWordCheckBox = new QCheckBox(tr("Match whole word"));
    m_matchCaseCheckBox = new QCheckBox(tr("Match case"));
    m_useRegexCheckBox = new QCheckBox(tr("Regular expression"));
    m_findSubCheckBox = new QCheckBox(tr("Scan subdirectories"));
    optLayout->addWidget(m_matchWordCheckBox);
    optLayout->addWidget(m_matchCaseCheckBox);
    optLayout->addWidget(m_useRegexCheckBox);
    optLayout->addWidget(m_findSubCheckBox);
    optLayout->addStretch();

    QHBoxLayout *findLayout = new QHBoxLayout;
    //findLayout->setMargin(0);
    m_findButton = new QPushButton(tr("Search"));
    m_stopButton = new QPushButton(tr("Cancel"));
    m_stopButton->setEnabled(false);
    //findLayout->addWidget();
    findLayout->addWidget(m_findCombo);
    findLayout->addWidget(m_findButton);
    findLayout->addWidget(m_stopButton);
    //findLayout->addStretch(0);
    //topLayout->addLayout(findLayout,4,1);


    topLayout->addWidget(new QLabel(tr("Search for:")),0,0);
    topLayout->addLayout(findLayout,0,1);
    topLayout->addWidget(new QLabel(tr("Options:")),1,0);
    topLayout->addLayout(optLayout,1,1);

    QHBoxLayout *dirLayout = new QHBoxLayout;
    m_findPathCombo = new QComboBox;
    m_findPathCombo->setEditable(true);
    QPushButton *browserBtn = new QPushButton(tr("Browse..."));
    QPushButton *currentBtn = new QPushButton(tr("Use Current"));
    dirLayout->addWidget(m_findPathCombo,1);
    dirLayout->addWidget(currentBtn);
    dirLayout->addWidget(browserBtn);

    topLayout->addWidget(new QLabel("Directory:"),2,0);
    topLayout->addLayout(dirLayout,2,1);

    m_filterCombo = new QComboBox;
    m_filterCombo->setEditable(true);

    m_filterCombo->addItem("*.go");
    m_filterCombo->addItem("*.lua;*.wlua");
    m_filterCombo->addItem("*.c;*.cpp;*.cxx;*.cc;*.c++;*.h;*.hpp;*.hh;*.hxx;*.h++;*.hcc;*.moc");
    m_filterCombo->addItem("*.htm;*.html;*.shtml;*.shtm");
    m_filterCombo->addItem("*");

    topLayout->addWidget(new QLabel(tr("Filter:")),3,0);
    topLayout->addWidget(m_filterCombo,3,1);

    m_findWidget->setLayout(topLayout);

    QAction *clearAct = new QAction(tr("Clear"),this);
    clearAct->setIcon(QIcon("icon:images/cleanoutput.png"));

    m_findPathCombo->setEditText(QDir::homePath());
    m_liteApp->settings()->beginGroup("findfiles");
    m_matchWordCheckBox->setChecked(m_liteApp->settings()->value("matchWord",false).toBool());
    m_matchCaseCheckBox->setChecked(m_liteApp->settings()->value("matchCase",false).toBool());
    m_useRegexCheckBox->setChecked(m_liteApp->settings()->value("useRegexp",false).toBool());
    m_findSubCheckBox->setChecked(m_liteApp->settings()->value("findSub",true).toBool());
    m_liteApp->settings()->endGroup();

    connect(browserBtn,SIGNAL(clicked()),this,SLOT(browser()));
    connect(currentBtn,SIGNAL(clicked()),this,SLOT(currentDir()));
    connect(m_findButton,SIGNAL(clicked()),this,SLOT(findInFiles()));
    connect(m_stopButton,SIGNAL(clicked()),m_thread,SLOT(stop()));
    connect(m_thread,SIGNAL(started()),this,SIGNAL(findStarted()));
    connect(m_thread,SIGNAL(finished()),this,SIGNAL(findFinished()));
    connect(m_thread,SIGNAL(findResult(LiteApi::FileSearchResult)),this,SIGNAL(findResult(LiteApi::FileSearchResult)));
    connect(m_findCombo->lineEdit(),SIGNAL(returnPressed()),this,SLOT(findInFiles()));
}

FileSearch::~FileSearch()
{
    m_liteApp->settings()->beginGroup("findfiles");
    m_liteApp->settings()->setValue("matchWord",m_matchWordCheckBox->isChecked());
    m_liteApp->settings()->setValue("matchCase",m_matchCaseCheckBox->isChecked());
    m_liteApp->settings()->setValue("useRegexp",m_useRegexCheckBox->isChecked());
    m_liteApp->settings()->setValue("findSub",m_findSubCheckBox->isChecked());
    m_liteApp->settings()->endGroup();

    if (m_thread) {
        m_thread->stop();
        delete m_thread;
    }
    if (m_findWidget) {
        delete m_findWidget;
    }
}

void FileSearch::setVisible(bool b)
{
    if (b) {
        LiteApi::IProject *proj = m_liteApp->projectManager()->currentProject();
        if (proj && !LiteApi::mimeIsFolder(proj->mimeType())) {
            QFileInfo info(proj->filePath());
            if (info.isDir())
                m_findPathCombo->setEditText(info.filePath());
            else
                m_findPathCombo->setEditText(info.path());
        }
        m_findCombo->setFocus();
        m_findCombo->lineEdit()->selectAll();
        LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
        if (editor) {
            QString text;
            QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
            if (ed) {
                text = ed->textCursor().selectedText();
            } else {
                QTextBrowser *ed = LiteApi::findExtensionObject<QTextBrowser*>(editor,"LiteApi.QTextBrowser");
                if (ed) {
                    text = ed->textCursor().selectedText();
                }
            }
            if (!text.isEmpty()) {
                this->m_findCombo->setEditText(text);
            }
            if (editor && !editor->filePath().isEmpty()) {
                QFileInfo info(editor->filePath());
                m_findPathCombo->setEditText(info.path());
            }
        }
    }
}

QString FileSearch::mimeType() const
{
    return "search/filesystem";
}

QString FileSearch::displayName() const
{
    return tr("Files on File System");
}

QWidget *FileSearch::widget() const
{
    return m_findWidget;
}

void FileSearch::start()
{
    this->findInFiles();
}

void FileSearch::cancel()
{
    m_thread->stop();
}

void FileSearch::activate()
{
    LiteApi::IProject *proj = m_liteApp->projectManager()->currentProject();
    if (proj && !LiteApi::mimeIsFolder(proj->mimeType())) {
        QFileInfo info(proj->filePath());
        if (info.isDir())
            m_findPathCombo->setEditText(info.filePath());
        else
            m_findPathCombo->setEditText(info.path());
    }
    m_findCombo->setFocus();
    m_findCombo->lineEdit()->selectAll();
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor) {
        QString text;
        QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
        if (ed) {
            text = ed->textCursor().selectedText();
        } else {
            QTextBrowser *ed = LiteApi::findExtensionObject<QTextBrowser*>(editor,"LiteApi.QTextBrowser");
            if (ed) {
                text = ed->textCursor().selectedText();
            }
        }
        if (!text.isEmpty()) {
            this->m_findCombo->setEditText(text);
        }
        if (editor && !editor->filePath().isEmpty()) {
            QFileInfo info(editor->filePath());
            m_findPathCombo->setEditText(info.path());
        }
    }
}

QString FileSearch::searchText() const
{
    return m_thread->findText;
}

void FileSearch::findInFiles()
{
    if (m_thread->isRunning()) {
        m_thread->stop();
    }
    QString text = m_findCombo->currentText();
    QString path = m_findPathCombo->currentText();
    if (text.isEmpty() || path.isEmpty()) {
        return;
    }
    m_thread->findPath = path;
    m_thread->findText = text;
    m_thread->useRegExp = m_useRegexCheckBox->isChecked();
    m_thread->matchCase = m_matchCaseCheckBox->isChecked();
    m_thread->matchWord = m_matchWordCheckBox->isChecked();
    m_thread->findSub = m_findSubCheckBox->isChecked();
    m_thread->nameFilter = m_filterCombo->currentText().split(";");
    m_thread->start(QThread::LowPriority);
    if (m_findCombo->findText(text) < 0) {
        m_findCombo->addItem(text);
    }
    if (m_findPathCombo->findText(path) < 0) {
        m_findPathCombo->addItem(path);
    }
}

void FileSearch::currentDir()
{
    LiteApi::IProject *proj = m_liteApp->projectManager()->currentProject();
    if (proj && !LiteApi::mimeIsFolder(proj->mimeType())) {
        QFileInfo info(proj->filePath());
        if (info.isDir()) {
            m_findPathCombo->setEditText(info.filePath());
        } else {
            m_findPathCombo->setEditText(info.path());
        }
    } else {
        LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
        if (editor && !editor->filePath().isEmpty()) {
            QFileInfo info(editor->filePath());
            m_findPathCombo->setEditText(info.path());
        }
    }

}

void FileSearch::browser()
{
    QString dir = QFileDialog::getExistingDirectory(m_liteApp->mainWindow(), tr("Open Directory"),
                                                    m_findPathCombo->currentText(),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        m_findPathCombo->setEditText(dir);
    }
}
