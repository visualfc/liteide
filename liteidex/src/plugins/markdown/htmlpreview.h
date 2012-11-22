/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: htmlpreview.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-11-21
// $Id: htmlpreview.h,v 1.0 2012-11-21 visualfc Exp $

#ifndef HTMLPREVIEW_H
#define HTMLPREVIEW_H

#include <liteapi/liteapi.h>

#include <QPointer>

class HtmlPreview : public QObject
{
    Q_OBJECT
public:
    explicit HtmlPreview(LiteApi::IApplication *app,QObject *parent = 0);
    
signals:
    
public slots:
    void appLoaded();
    void loadHeadData(const QString &css);
    void currentEditorChanged(LiteApi::IEditor*);
    void editorHtmlPrivew();
    void triggered(bool);
protected:
    LiteApi::IApplication *m_liteApp;
    QWidget               *m_widget;
    LiteApi::IHtmlWidget  *m_htmlWidget;
    QAction      *m_toolAct;
    QPointer<LiteApi::ITextEditor> m_curEditor;
    QByteArray  m_lastData;
    QByteArray  m_head;
};

#endif // HTMLPREVIEW_H
