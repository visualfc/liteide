/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2020 LiteIDE. All rights reserved.
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
// Module: vtermwidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef VTERMWIDGET_H
#define VTERMWIDGET_H

#include "vtermwidgetbase.h"
#include "ptyqt/core/ptyqt.h"

class QProcess;
class VTermWidget : public VTermWidgetBase
{
    Q_OBJECT
public:
    explicit VTermWidget(QWidget *parent);
    virtual ~VTermWidget();
    bool isAvailable() const;
    void start(const QString &program, const QStringList &arguments, const QString &workingDirectory, QStringList env);
    IPtyProcess *process() const;
signals:
    void exited();
public slots:
    void readyRead();
    void resizePty(int rows,int cols);
protected:
    virtual void write_data(const char *buf, int len);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
protected:
    IPtyProcess *m_process;
};

#endif // VTERMWIDGET_H
