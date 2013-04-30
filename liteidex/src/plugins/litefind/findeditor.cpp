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
// Module: findeditor.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "findeditor.h"
#include "litefind_global.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QTextBlock>
#include <QTextBrowser>
#include <QStatusBar>
#include <QRegExp>
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

FindEditor::FindEditor(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_widget(new QWidget)
{
    m_findEdit = new QLineEdit;
    m_replaceEdit = new QLineEdit;

    m_findNext = new QPushButton(tr("Find Next"));
    m_findPrev = new QPushButton(tr("Find Previous"));
    m_replaceLabel = new QLabel(tr("Replace With:"));
    m_replace = new QPushButton(tr("Replace"));
    m_replaceAll = new QPushButton(tr("Replace All"));

    m_matchWordCheckBox = new QCheckBox(tr("Match whole word only"));
    m_matchCaseCheckBox = new QCheckBox(tr("Match case"));
    m_useRegexCheckBox = new QCheckBox(tr("Regular expression"));
    m_wrapAroundCheckBox = new QCheckBox(tr("Wrap around"));

    m_matchWordCheckBox->setChecked(m_liteApp->settings()->value(FIND_MATCHWORD,true).toBool());
    m_matchCaseCheckBox->setChecked(m_liteApp->settings()->value(FIND_MATCHCASE,true).toBool());
    m_useRegexCheckBox->setChecked(m_liteApp->settings()->value(FIND_USEREGEXP,false).toBool());
    m_wrapAroundCheckBox->setChecked(m_liteApp->settings()->value(FIND_WRAPAROUND,true).toBool());

    m_status = new QLabel(tr("Ready"));
    m_status->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_status->setAlignment(Qt::AlignRight);
    m_status->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    QGridLayout *layout = new QGridLayout;
    layout->setMargin(0);
    layout->setVerticalSpacing(1);

    QHBoxLayout *optLayout = new QHBoxLayout;
    optLayout->setMargin(0);    

    optLayout->addWidget(m_matchWordCheckBox);
    optLayout->addWidget(m_matchCaseCheckBox);
    optLayout->addWidget(m_useRegexCheckBox);
    optLayout->addWidget(m_wrapAroundCheckBox);
    optLayout->addStretch();
    optLayout->addWidget(m_status);

    layout->addWidget(new QLabel(tr("Find What:")),0,0);
    layout->addWidget(m_findEdit,0,1);
    layout->addWidget(m_findNext,0,2);
    layout->addWidget(m_findPrev,0,3);
    //layout->addWidget(hideReplace,0,3);

    layout->addWidget(m_replaceLabel,1,0);
    layout->addWidget(m_replaceEdit,1,1);
    layout->addWidget(m_replace,1,2);
    layout->addWidget(m_replaceAll,1,3);

    layout->addWidget(new QLabel(tr("Options:")),3,0);
    layout->addLayout(optLayout,3,1,1,3);

    m_widget->setLayout(layout);

    QWidget::setTabOrder(m_findEdit,m_replaceEdit);

    connect(m_findEdit,SIGNAL(returnPressed()),this,SLOT(findNext()));
    connect(m_findNext,SIGNAL(clicked()),this,SLOT(findNext()));
    connect(m_findPrev,SIGNAL(clicked()),this,SLOT(findPrev()));
    connect(m_replaceEdit,SIGNAL(returnPressed()),this,SLOT(replace()));
    connect(m_replace,SIGNAL(clicked()),this,SLOT(replace()));
    connect(m_replaceAll,SIGNAL(clicked()),this,SLOT(replaceAll()));
    connect(m_matchCaseCheckBox,SIGNAL(toggled(bool)),this,SLOT(findOptionChanged()));
    connect(m_matchWordCheckBox,SIGNAL(toggled(bool)),this,SLOT(findOptionChanged()));
    connect(m_useRegexCheckBox,SIGNAL(toggled(bool)),this,SLOT(findOptionChanged()));
    connect(m_wrapAroundCheckBox,SIGNAL(toggled(bool)),this,SLOT(findOptionChanged()));
    connect(m_findEdit,SIGNAL(textChanged(QString)),this,SLOT(findOptionChanged()));
    connect(m_replaceEdit,SIGNAL(textChanged(QString)),this,SLOT(replaceChanged()));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(updateCurrentEditor(LiteApi::IEditor*)));
}

FindEditor::~FindEditor()
{
    m_liteApp->settings()->setValue(FIND_MATCHWORD,m_matchWordCheckBox->isChecked());
    m_liteApp->settings()->setValue(FIND_MATCHCASE,m_matchCaseCheckBox->isChecked());
    m_liteApp->settings()->setValue(FIND_USEREGEXP,m_useRegexCheckBox->isChecked());
    m_liteApp->settings()->setValue(FIND_WRAPAROUND,m_wrapAroundCheckBox->isChecked());

    if (m_widget) {
        delete m_widget;
    }
}

QWidget *FindEditor::widget()
{
    return m_widget;
}

void FindEditor::setVisible(bool b)
{
    this->m_widget->setVisible(b);
    if (b) {
        LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
        if (editor) {
            QString text;
            QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
            if (ed) {
                text = ed->textCursor().selectedText();
            }
            if (!text.isEmpty()) {
                this->m_findEdit->setText(text);
            }
            this->m_findEdit->setFocus();
            this->m_findEdit->selectAll();
        }
    }
    updateCurrentEditor(m_liteApp->editorManager()->currentEditor());
}

void FindEditor::setReplaceMode(bool b)
{
    m_replaceLabel->setVisible(b);
    m_replaceEdit->setVisible(b);
    m_replace->setVisible(b);
    m_replaceAll->setVisible(b);
}

void FindEditor::findHelper(FindOption *opt)
{
    bool bFocus = m_findEdit->hasFocus();
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    QTextCursor find;
    if (textEditor) {
        QPlainTextEdit *ed = LiteApi::getPlainTextEdit(editor);
        if (ed) {
            find = findEditor(ed->document(),ed->textCursor(),opt);
            if (!find.isNull()) {
                ed->setTextCursor(find);
            }
        }
    } else {
        QTextBrowser *ed = LiteApi::findExtensionObject<QTextBrowser*>(editor,"LiteApi.QTextBrowser");
        if (ed) {
            find = findEditor(ed->document(),ed->textCursor(),opt);
            if (!find.isNull()) {
                ed->setTextCursor(find);
            }
        }
    }
    if (find.isNull()) {
        m_status->setText(tr("Not find"));
    } else {
        m_status->setText(QString("Ln:%1 Col:%2").
                              arg(find.blockNumber()+1).
                              arg(find.columnNumber()+1));
    }
    if (bFocus) {
        m_findEdit->setFocus();
    } else if (textEditor) {
        textEditor->onActive();
    }
}

void FindEditor::setReady(const QString &findText)
{
    m_widget->setFocus();
    if (!findText.isEmpty()) {
        m_findEdit->setText(findText);
    }
    if (m_findEdit->text().isEmpty()) {
        m_findEdit->setFocus();
    } else {
        m_replaceEdit->setFocus();
    }
    m_status->setText(tr("Ready"));
}

void FindEditor::getFindOption(FindOption *opt, bool backWard)
{
    opt->findText = m_findEdit->text();
    opt->replaceText = m_replaceEdit->text();
    opt->matchWord = m_matchWordCheckBox->isChecked();
    opt->matchCase = m_matchCaseCheckBox->isChecked();
    opt->useRegexp = m_useRegexCheckBox->isChecked();
    opt->wrapAround = m_wrapAroundCheckBox->isChecked();
    opt->backWard = backWard;
}

QTextCursor FindEditor::findEditor(QTextDocument *doc, const QTextCursor &cursor, FindOption *opt, bool wrap)
{
    QTextDocument::FindFlags flags = 0;
    if (opt->backWard) {
        flags |= QTextDocument::FindBackward;
    }
    if (opt->matchCase) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (opt->matchWord) {
        flags |= QTextDocument::FindWholeWords;
    }
    Qt::CaseSensitivity cs = Qt::CaseInsensitive;
    if (opt->matchCase) {
        cs = Qt::CaseSensitive;
    }
    int from = cursor.position();
    if (cursor.hasSelection()) {
        if (opt->backWard) {
            from = cursor.selectionStart();
        } else {
            from = cursor.selectionEnd();
        }
    }

    QTextCursor find;
    if (opt->useRegexp) {
        find = doc->find(QRegExp(opt->findText,cs),from,flags);
    } else {
        find = doc->find(opt->findText,from,flags);
    }

    if (find.isNull() && opt->wrapAround && wrap) {
        if (opt->backWard) {
            from = doc->lastBlock().position()+doc->lastBlock().length();
        } else {
            from = 0;
        }
        if (opt->useRegexp) {
            find = doc->find(QRegExp(opt->findText,cs),from,flags);
        } else {
            find = doc->find(opt->findText,from,flags);
        }
    }
    return find;
}

void FindEditor::replaceHelper(LiteApi::ITextEditor *editor, FindOption *opt, int replaceCount)
{
    bool bFocus = m_replaceEdit->hasFocus();

    QPlainTextEdit *ed = LiteApi::getPlainTextEdit(editor);
    if (!ed) {
        return;
    }

    QTextCursor find;
    QTextCursor cursor = ed->textCursor();
    int line = cursor.blockNumber();
    int col = cursor.columnNumber();
    Qt::CaseSensitivity cs = Qt::CaseInsensitive;
    if (opt->matchCase) {
        cs = Qt::CaseSensitive;
    }
    if ( cursor.hasSelection() ) {
        QString text = cursor.selectedText();
        if (opt->useRegexp) {
            if (text.indexOf(QRegExp(opt->findText,cs),0) != -1) {
                find = cursor;
            }
        } else {
            if (text.indexOf(opt->findText,0,cs) != -1) {
                find = cursor;
            }
        }
    }
    int number = 0;
    bool wrap = opt->wrapAround;
    do {
        if (!find.isNull()) {
            number++;
            find.beginEditBlock();
            QString text = find.selectedText();
            if (opt->useRegexp) {
                text.replace(QRegExp(opt->findText,cs),opt->replaceText);
            } else {
                text.replace(opt->findText,opt->replaceText);
            }
            find.removeSelectedText();
            find.insertText(text);
            find.endEditBlock();
            ed->setTextCursor(find);
        }
        cursor = ed->textCursor();
        find = findEditor(ed->document(),cursor,opt,false);
        if (find.isNull() && wrap) {
            wrap = false;
            find = findEditor(ed->document(),cursor,opt,true);
        }
        if (opt->wrapAround && !wrap) {
            if (find.blockNumber() > line ||
                    (find.blockNumber() >= line && find.columnNumber() > col) )  {
                break;
            }
        }
        if (replaceCount != -1 && number >= replaceCount) {
            if (!find.isNull()) {
                ed->setTextCursor(find);
                m_status->setText(QString("Ln:%1 Col:%2").
                                      arg(find.blockNumber()+1).
                                      arg(find.columnNumber()+1));
            } else {
                m_status->setText(tr("Not find"));
            }
            break;
        }
    } while(!find.isNull());
    if (replaceCount == -1) {
        m_status->setText(QString("Replace:%1").arg(number));
    }

    if (bFocus) {
        m_replaceEdit->setFocus();
    } else if (editor) {
        editor->onActive();
    }
}


void FindEditor::findNext()
{
    if (!m_option.isValid()) {
        return;
    }
    m_option.backWard = false;
    findHelper(&m_option);
}

void FindEditor::findPrev()
{
    if (!m_option.isValid()) {
        return;
    }
    m_option.backWard = true;
    findHelper(&m_option);
}

void FindEditor::replace()
{
    if (!m_option.isValid()) {
        return;
    }

    m_option.backWard = false;

    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        return;
    }
    replaceHelper(textEditor,&m_option,1);
}

void FindEditor::replaceAll()
{
    if (!m_option.isValid()) {
        return;
    }

    m_option.backWard = false;

    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        return;
    }
    replaceHelper(textEditor,&m_option,-1);
}


void FindEditor::findOptionChanged()
{
    getFindOption(&m_option,false);
    m_status->setText(tr("Ready"));
    if (m_option.useRegexp) {
        QRegExp reg(m_option.findText);
        if (!reg.isValid()) {
            m_status->setText(reg.errorString());
        }
    }
    updateCurrentEditor(m_liteApp->editorManager()->currentEditor());
}

void FindEditor::replaceChanged()
{
    m_option.replaceText = m_replaceEdit->text();
}


void FindEditor::updateCurrentEditor(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        return;
    }
    if (m_widget->isVisible()) {
        textEditor->setFindOption(&m_option);
    } else {
        textEditor->setFindOption(0);
    }
}
