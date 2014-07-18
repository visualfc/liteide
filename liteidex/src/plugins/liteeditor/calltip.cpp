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
// Module: calltip.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "calltip.h"
#include <QPainter>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

CallTip::CallTip(QObject *parent) :
    QObject(parent)
{
    popup = new CallTipWidget;
    m_inCallTipMode = false;
    m_pos = 0;
    m_bEnable = false;
    connect(popup,SIGNAL(mouseClick()),this,SLOT(CallTipCancel()));
}

CallTip::~CallTip()
{
    if (popup) {
        delete popup;
    }
}

QWidget *CallTip::widget()
{
    return popup;
}

void CallTip::callTipStart(int pos, const QRect& rect, QString text, QWidget *parent)
{
    m_inCallTipMode = true;
    m_pos = pos;
    popup->setInfo(text);
    QPoint pt;
    pt = parent->mapToGlobal(rect.topLeft());
    int w = rect.width();
    int h = rect.height();
    popup->setGeometry(pt.x(), pt.y()-h, w, h);
    if (!popup->isVisible())
        popup->show();
}

void CallTip::showTip()
{
    m_inCallTipMode = true;
    if (!popup->isVisible())
        popup->show();
}

void CallTip::CallTipCancel()
{
    m_inCallTipMode = false;
    if (popup->isVisible()) {
        popup->hide();
    }
}

void CallTip::showPopup(const QRect& rect, QWidget *widget)
{
    const QRect screen = QApplication::desktop()->availableGeometry(widget);
    Qt::LayoutDirection dir = widget->layoutDirection();
    QPoint pos;
    int rh, w;
    int h = 100;
    if (rect.isValid()) {
        rh = rect.height();
        w = rect.width();
        pos = widget->mapToGlobal(dir == Qt::RightToLeft ? rect.bottomRight() : rect.bottomLeft());
    } else {
        rh = widget->height();
        pos = widget->mapToGlobal(QPoint(0, widget->height() - 2));
        w = widget->width();
    }

    if (w > screen.width())
        w = screen.width();
    if ((pos.x() + w) > (screen.x() + screen.width()))
        pos.setX(screen.x() + screen.width() - w);
    if (pos.x() < screen.x())
        pos.setX(screen.x());

    int top = pos.y() - rh - screen.top() + 2;
    int bottom = screen.bottom() - pos.y();
    h = qMax(h, popup->minimumHeight());
    if (h > bottom) {
        h = qMin(qMax(top, bottom), h);

        if (top > bottom)
            pos.setY(pos.y() - h - rh + 2);
    }

    popup->setGeometry(pos.x(), pos.y(), w, h);

    if (!popup->isVisible())
        popup->show();
}

bool CallTip::isEnable() const
{
    return m_bEnable;
}

void CallTip::setEnable(bool b)
{
    m_bEnable = b;
}


CallTipWidget::CallTipWidget(QWidget *parent) : QWidget(parent,Qt::ToolTip)
{

}

CallTipWidget::~CallTipWidget()
{

}

void CallTipWidget::setInfo(QString info)
{
    m_info = info;
}

void CallTipWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QRect rc = this->rect();
    painter.fillRect(rc,Qt::white);
    painter.drawText(rc,m_info,Qt::AlignLeft|Qt::AlignVCenter);
}

void CallTipWidget::mousePressEvent(QMouseEvent *event)
{
    emit mouseClick();
}
