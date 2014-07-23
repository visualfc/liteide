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
// Module: functiontooltip.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FUNCTIONTOOLTIP_H
#define FUNCTIONTOOLTIP_H

#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include <QWidget>
#include <QLabel>

class FakeToolTipFrame : public QWidget
{
public:
    FakeToolTipFrame(QWidget *parent = 0) :
        QWidget(parent, Qt::ToolTip | Qt::WindowStaysOnTopHint)
    {
        setFocusPolicy(Qt::NoFocus);
        //setAttribute(Qt::WA_DeleteOnClose);

        // Set the window and button text to the tooltip text color, since this
        // widget draws the background as a tooltip.
        QPalette p = palette();
        const QColor toolTipTextColor = p.color(QPalette::Inactive, QPalette::ToolTipText);
        p.setColor(QPalette::Inactive, QPalette::WindowText, toolTipTextColor);
        p.setColor(QPalette::Inactive, QPalette::ButtonText, toolTipTextColor);
        setPalette(p);
    }

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
};

struct TipInfo
{
    TipInfo(int _pos, const QString &_tip) :
        startPos(_pos),tip(_tip)
    {}
    int     startPos;
    QString tip;
};

class QPlainTextEdit;
class FunctionTooltip : public QObject
{
    Q_OBJECT
public:
    FunctionTooltip(LiteApi::ITextEditor *editor, LiteApi::ITextLexer *lexer, int maxTipCount = 20, QObject *parent = 0);
    ~FunctionTooltip();
    void showFunctionHint(int startPosition, const QString &tip);
protected:
    bool eventFilter(QObject *obj, QEvent *e);
    void updateArgumentHighlight();
    void updateHintText();
    void showPopup(int startpos);
    void hide();
    void saveTip(int startPos, const QString &text);
    bool restoreTip(int startpos);
protected:
    LiteApi::ITextEditor *m_editor;
    LiteApi::ITextLexer *m_lexer;
    QWidget              *m_popup;
    QLabel               *m_label;
    QList<TipInfo>       m_infoList;
    QString              m_tip;
    int                  m_currentarg;
    int                  m_startpos;
    int                  m_startFuncitonPos;
    int                  m_maxTipCount;
    bool                 m_escapePressed;    
};


#endif // FUNCTIONTOOLTIP_H
