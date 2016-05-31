/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: quickopenwidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "quickopenwidget.h"
#include <QVBoxLayout>
#include <QFocusEvent>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

QuickOpenWidget::QuickOpenWidget(LiteApi::IApplication *app, QWidget *parent) :
    QWidget(parent,Qt::Popup),// Qt::ToolTip | Qt::WindowStaysOnTopHint)
    m_liteApp(app)
{
    //this->setFocusPolicy(Qt::NoFocus);
    m_edit = new Utils::FancyLineEdit;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_edit);

    this->setLayout(layout);
}

void QuickOpenWidget::showPopup()
{
    QToolBar *toolBar =  m_liteApp->actionManager()->loadToolBar("toolbar/std");
    QRect rc = toolBar->frameGeometry();
    QPoint pt = rc.topRight();
    pt.rx() += 4;
    pt.ry() += 2;
    this->move(m_liteApp->mainWindow()->mapToGlobal(pt));
    m_edit->setFocus();
    this->show();
}

bool QuickOpenWidget::eventFilter(QObject *o, QEvent *e)
{
    return QWidget::eventFilter(o,e);
}
