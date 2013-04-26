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
// Module: findeditor.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef REPLACEEDITOR_H
#define REPLACEEDITOR_H

#include "liteapi/liteapi.h"
#include <QTextCursor>
#include <QLabel>

class QLineEdit;
class QCheckBox;
class QPushButton;
class QLabel;

struct FindOption : public LiteApi::FindOption
{
    QString replaceText;
    bool isValid() {
        return !findText.isEmpty();
    }
};

class FindEditor : public QObject
{
    Q_OBJECT
public:
    explicit FindEditor(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~FindEditor();
    virtual QWidget *widget();
    virtual void setReady(const QString &findText = QString());
    void getFindOption(FindOption *state, bool backWard);
    void setVisible(bool b);
    void setReplaceMode(bool b);
    void findHelper(FindOption *opt);
public slots:
    void updateCurrentEditor(LiteApi::IEditor*);
    void findOptionChanged();
    void replaceChanged();
    void findNext();
    void findPrev();
    void replace();
    void replaceAll();
public:
    QTextCursor findEditor(QTextDocument *ed, const QTextCursor &cursor, FindOption *opt, bool wrap = true);
    void replaceHelper(LiteApi::ITextEditor *editor, FindOption *opt,int replaceCount = -1);
protected:
    LiteApi::IApplication   *m_liteApp;
    QWidget *m_widget;
    QLineEdit *m_findEdit;
    QPushButton *m_findNext;
    QPushButton *m_findPrev;
    QLineEdit *m_replaceEdit;
    QLabel    *m_replaceLabel;
    QPushButton *m_replace;
    QPushButton *m_replaceAll;
    QCheckBox   *m_matchWordCheckBox;
    QCheckBox   *m_matchCaseCheckBox;
    QCheckBox   *m_useRegexCheckBox;
    QCheckBox   *m_wrapAroundCheckBox;
    QLabel      *m_status;
    FindOption   m_option;
};

#endif // REPLACEEDITOR_H
