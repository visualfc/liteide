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
// Module: replaceeditor.h
// Creator: visualfc <visualfc@gmail.com>



#ifndef REPLACEEDITOR_H
#define REPLACEEDITOR_H

#include "liteapi/liteapi.h"
#include <QTextCursor>
#include <QLabel>
#include <QDebug>

class QLineEdit;
class QCheckBox;

struct ReplaceState {
    QString findText;
    QString replaceText;
    bool    useRegexp;
    bool    matchWord;
    bool    matchCase;
    bool    wrapAround;
    bool    backWard;
    bool isValid() {
        return !findText.isEmpty();
    }
};

class ReplaceEditor : public QObject
{
    Q_OBJECT
public:
    explicit ReplaceEditor(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~ReplaceEditor();
    virtual QWidget *widget();
    virtual void setReady(const QString &findText = QString());
    void genFindState(ReplaceState *state, bool backWard);
    void setVisible(bool b);
signals:
    void hideReplace();
public slots:
    void findNext();
    void replace();
    void replaceAll();
public:
    QTextCursor findEditor(QTextDocument *ed, const QTextCursor &cursor, ReplaceState *state);
    template <typename T>
    void findHelper(T *ed, ReplaceState *state)
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
    void replaceHelper(T *ed, ReplaceState *state, int replaceCount = -1)
    {
        QTextCursor find;
        QTextCursor cursor = ed->textCursor();
        int line = cursor.blockNumber();
        int col = cursor.columnNumber();
        Qt::CaseSensitivity cs = Qt::CaseInsensitive;
        if (state->matchCase) {
            cs = Qt::CaseSensitive;
        }
        if ( cursor.hasSelection() ) {
            QString text = cursor.selectedText();
            if (state->useRegexp) {
                if (text.indexOf(QRegExp(state->findText,cs),0) != -1) {
                    find = cursor;
                }
            } else {
                if (text.indexOf(state->findText,0,cs) != -1) {
                    find = cursor;
                }
            }
        }
        int number = 0;
        bool warp = state->wrapAround;
        do {
            if (!find.isNull()) {
                number++;
                find.beginEditBlock();
                QString text = find.selectedText();
                if (state->useRegexp) {
                    text.replace(QRegExp(state->findText,cs),state->replaceText);
                } else {
                    text.replace(state->findText,state->replaceText);
                }
                find.removeSelectedText();
                find.insertText(text);
                find.endEditBlock();
                ed->setTextCursor(find);
            }
            cursor = ed->textCursor();
            find = findEditor(ed->document(),cursor,state);
            if (find.isNull() && warp) {
                warp = false;
                if (state->backWard) {
                    cursor.movePosition(QTextCursor::End,QTextCursor::MoveAnchor);
                } else {
                    cursor.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
                }
                find = findEditor(ed->document(),cursor,state);
            }
            if (state->wrapAround && !warp) {
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
                                          arg(find.columnNumber()));
                } else {
                    m_status->setText(tr("Not find"));
                }
                break;
            }
        } while(!find.isNull());
        if (replaceCount == -1) {
            m_status->setText(QString("Replace:%1").arg(number));
        }
    }
protected:
    LiteApi::IApplication   *m_liteApp;
    QWidget *m_widget;
    QLineEdit *m_findEdit;
    QLineEdit *m_replaceEdit;
    QCheckBox   *m_matchWordCheckBox;
    QCheckBox   *m_matchCaseCheckBox;
    QCheckBox   *m_useRegexCheckBox;
    QCheckBox   *m_wrapAroundCheckBox;
    QLabel      *m_status;
};

#endif // REPLACEEDITOR_H
