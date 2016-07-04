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
// Module: quickopenwidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef QUICKOPENWIDGET_H
#define QUICKOPENWIDGET_H

#include "liteapi/liteapi.h"
#include "../3rdparty/qtc_editutil/fancylineedit.h"
#include <QAbstractItemModel>

class QTreeView;
class QuickOpenWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QuickOpenWidget(LiteApi::IApplication *app, QWidget *parent = 0);
    void setModel(QAbstractItemModel *model);
    QLineEdit *editor();
    QTreeView *view();
    virtual void hideEvent(QHideEvent *e);
signals:
    void filterChanged(QString);
    void selected();
    void hidePopup();
public slots:
    void showPopup();
protected:
    virtual bool eventFilter(QObject *o, QEvent *e);
protected:
    LiteApi::IApplication *m_liteApp;
    Utils::FancyLineEdit *m_edit;
    QTreeView   *m_view;
    bool         m_wrap;
};

#endif // QUICKOPENWIDGET_H
