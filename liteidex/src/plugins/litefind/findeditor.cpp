/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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

#include <QLineEdit>
#include <QTextCursor>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QTextBrowser>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
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

    m_matchWordCheckBox = new QCheckBox(tr("Match word"));
    m_matchCaseCheckBox = new QCheckBox(tr("Match case"));
    m_useRegexCheckBox = new QCheckBox(tr("Regular expression"));
    m_wrapAroundCheckBox = new QCheckBox(tr("Wrap around"));

    m_liteApp->settings()->beginGroup("findeditor");
    m_matchWordCheckBox->setChecked(m_liteApp->settings()->value("matchWord",true).toBool());
    m_matchCaseCheckBox->setChecked(m_liteApp->settings()->value("matchCase",true).toBool());
    m_useRegexCheckBox->setChecked(m_liteApp->settings()->value("useRegexp",false).toBool());
    m_wrapAroundCheckBox->setChecked(m_liteApp->settings()->value("wrapAround",true).toBool());
    m_liteApp->settings()->endGroup();

    QPushButton *findNext = new QPushButton(tr("Find Next"));
    QPushButton *findPrev = new QPushButton(tr("Find Prev"));
    QPushButton *switchReplace = new QPushButton(tr("Replace"));
    switchReplace->setToolTip(tr("Switch Replace"));
    QPushButton *hideFind = new QPushButton(tr("Hide"));
    hideFind->setToolTip(tr("Hide Find"));


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
    layout->addWidget(findNext,0,2);
    layout->addWidget(findPrev,0,3);
    layout->addWidget(switchReplace,0,4);
    layout->addWidget(hideFind,0,5);
    layout->addWidget(new QLabel(tr("Find Option:")),1,0);
    layout->addLayout(optLayout,1,1,1,5);

    m_widget->setLayout(layout);

    connect(m_findEdit,SIGNAL(returnPressed()),this,SLOT(findNext()));
    connect(findNext,SIGNAL(clicked()),this,SLOT(findNext()));
    connect(findPrev,SIGNAL(clicked()),this,SLOT(findPrev()));
    connect(hideFind,SIGNAL(clicked()),this,SIGNAL(hideFind()));
    connect(switchReplace,SIGNAL(clicked()),this,SIGNAL(swithReplace()));
}

FindEditor::~FindEditor()
{
    m_liteApp->settings()->beginGroup("findeditor");
    m_liteApp->settings()->setValue("matchWord",m_matchWordCheckBox->isChecked());
    m_liteApp->settings()->setValue("matchCase",m_matchCaseCheckBox->isChecked());
    m_liteApp->settings()->setValue("useRegexp",m_useRegexCheckBox->isChecked());
    m_liteApp->settings()->setValue("wrapAround",m_wrapAroundCheckBox->isChecked());
    m_liteApp->settings()->endGroup();

    if (m_widget) {
        delete m_widget;
    }
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
            } else {
                QTextBrowser *ed = LiteApi::findExtensionObject<QTextBrowser*>(editor,"LiteApi.QTextBrowser");
                if (ed) {
                    text = ed->textCursor().selectedText();
                }
            }
            if (!text.isEmpty()) {
                this->m_findEdit->setText(text);
            }
            this->m_findEdit->setFocus();
        }
    }

}

QWidget *FindEditor::widget()
{
    return m_widget;
}

void FindEditor::setReady()
{
    m_widget->setFocus();
    m_status->setText(tr("Ready"));
    m_findEdit->setFocus();
}

QString FindEditor::findText() const
{
    return m_findEdit->text();
}

void FindEditor::genFindState(FindState *state, bool backWard)
{
    state->findText = m_findEdit->text();
    state->matchWord = m_matchWordCheckBox->isChecked();
    state->matchCase = m_matchCaseCheckBox->isChecked();
    state->useRegexp = m_useRegexCheckBox->isChecked();
    state->wrapAround = m_wrapAroundCheckBox->isChecked();
    state->backWard = backWard;
}

void FindEditor::findNext()
{
    FindState state;
    genFindState(&state,false);
    if (!state.isValid()) {
        return;
    }

    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (ed) {
        //findAllHelper(ed,&state);
        findHelper(ed,&state);
    } else {
        QTextBrowser *ed = LiteApi::findExtensionObject<QTextBrowser*>(editor,"LiteApi.QTextBrowser");
        if (ed) {
            findHelper(ed,&state);
        }
    }
}

void FindEditor::findPrev()
{
    FindState state;
    genFindState(&state,true);
    if (!state.isValid()) {
        return;
    }

    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (ed) {
        findHelper(ed,&state);
    } else {
        QTextBrowser *ed = LiteApi::findExtensionObject<QTextBrowser*>(editor,"LiteApi.QTextBrowser");
        if (ed) {
            findHelper(ed,&state);
        }
    }
}

QTextCursor FindEditor::findEditor(QTextDocument *doc, const QTextCursor &cursor, FindState *state)
{
    QString text = m_findEdit->text();
    QTextDocument::FindFlags flags = 0;
    if (state->backWard) {
        flags |= QTextDocument::FindBackward;
    }
    if (state->matchCase) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (state->matchWord) {
        flags |= QTextDocument::FindWholeWords;
    }
    Qt::CaseSensitivity cs = Qt::CaseInsensitive;
    if (state->matchCase) {
        cs = Qt::CaseSensitive;
    }
    QTextCursor find;
    if (state->useRegexp) {
        find = doc->find(QRegExp(text,cs),cursor,flags);
    } else {
        find = doc->find(text,cursor,flags);
    }
    return find;
}

