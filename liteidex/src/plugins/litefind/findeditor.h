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
// Module: findeditor.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FINDEDITOR_H
#define FINDEDITOR_H

#include "liteapi/liteapi.h"
#include <QTextEdit>
#include <QTextCursor>
#include <QLabel>

class QLineEdit;
class QCheckBox;

struct FindState {
    QString findText;
    bool    useRegexp;
    bool    matchWord;
    bool    matchCase;
    bool    wrapAround;
    bool    backWard;
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
    virtual void setReady();
    QString findText() const;
    void genFindState(FindState *state, bool backWard);
    void setVisible(bool b);
signals:
    void hideFind();
    void swithReplace();
public slots:
    void findNext();
    void findPrev();
public:
    QTextCursor findEditor(QTextDocument *ed, const QTextCursor &cursor, FindState *state);        
    template <typename T>
    void findHelper(T *ed, FindState *state)
    {
        QTextCursor cursor = ed->textCursor();
        QTextCursor find = findEditor(ed->document(),cursor,state);
        if (find.isNull() && state->wrapAround) {
            if (state->backWard) {
                cursor.movePosition(QTextCursor::End,QTextCursor::MoveAnchor);
            } else {
                cursor.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
            }
            find = findEditor(ed->document(),cursor,state);
        }
        if (!find.isNull()) {
            ed->setTextCursor(find);
            m_status->setText(QString("Ln:%1 Col:%2").
                                  arg(find.blockNumber()+1).
                                  arg(find.columnNumber()));
        } else {
            m_status->setText(tr("Not find"));
        }
    }
    template <typename T>
    void findAllHelper(T *ed, FindState *state)
    {
        QTextCursor cursor = ed->textCursor();
        cursor.setPosition(0);
        QList<QTextEdit::ExtraSelection> selections;
        QTextCursor find = findEditor(ed->document(),cursor,state);
        while (!find.isNull()) {
            QTextEdit::ExtraSelection sel;
            sel.cursor = find;
            sel.format.setFontUnderline(true);
            sel.format.setBackground(Qt::yellow);
            sel.format.setAnchor(true);
            selections.append(sel);
            cursor.setPosition(find.position()+1);
            find = findEditor(ed->document(),cursor,state);
        }
        ed->setExtraSelections(selections);
    }
protected:
    LiteApi::IApplication *m_liteApp;
    QWidget     *m_widget;
    QLineEdit   *m_findEdit;
    QCheckBox   *m_matchWordCheckBox;
    QCheckBox   *m_matchCaseCheckBox;
    QCheckBox   *m_useRegexCheckBox;
    QCheckBox   *m_wrapAroundCheckBox;
    QLabel      *m_status;
};



#endif // FINDEDITOR_H
