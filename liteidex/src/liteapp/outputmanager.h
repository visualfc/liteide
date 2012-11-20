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
// Module: outputmanager.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: outputmanager.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include "liteapi/liteapi.h"
#include "elidedlabel/elidedlabel.h"
#include <QPointer>

using namespace LiteApi;

class MainWindow;
class QStackedWidget;
class QToolBar;
class QActionGroup;
class QAction;
class QPushButton;
class QToolButton;
class QHBoxLayout;
class QLabel;

class OutputManager : public IOutputManager
{
    Q_OBJECT
public:
    ~OutputManager();
    virtual bool initWithApp(IApplication *app);
    virtual QWidget *widget();
    virtual QStatusBar *statusBar();
    virtual void addOutuput(QWidget *w, const QString &label);
    virtual void addOutuput(QWidget *w, const QIcon &icon, const QString &label);
    virtual void removeOutput(QWidget *w);
    virtual void showOutput(QWidget *w);
    virtual void hideOutput(QWidget *w);
    virtual QWidget *currentOutput();
    virtual void setStatusInfo(const QString &info);
    void setProjectInfo(const QString &text);
public slots:
    virtual void setCurrentOutput(QWidget *w = 0);
protected slots:
    void selectedOutputAct(QAction*);
protected:
    QStackedWidget    *m_stackedWidget;
    QStatusBar        *m_statusBar;
    ElidedLabel       *m_projectInfoLabel;
    ElidedLabel       *m_statusInfoLabel;
    QActionGroup      *m_outputActGroup;
    QPointer<QAction>  m_lastOutputAct;
    QMap<QWidget*,QAction*>   m_widgetActionMap;
    QWidget         *m_buttonsWidget;
    QMap<QWidget*,QToolButton*> m_widgetButtonMap;
    QHBoxLayout *m_buttonsLayout;
};

#endif // OUTPUTMANAGER_H
