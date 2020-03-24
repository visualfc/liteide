/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
#include "../3rdparty/qtc_editutil/filterlineedit.h"
#include <QAbstractItemModel>
#include <QScopedPointer>

class QTreeView;
class QVBoxLayout;
class QuickOpenWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QuickOpenWidget(LiteApi::IApplication *app, QWidget *parent = 0);
    void setModel(QAbstractItemModel *model, const QModelIndex &rootIndex);
    QLineEdit *editor();
    QTreeView *view();
    void setTempToolBar(QToolBar *toolBar);
    virtual void hideEvent(QHideEvent *e);
    void closeWidget();
signals:
    void hideWidget();
    void indexChanage(const QModelIndex &index);
    void indexEnter(const QModelIndex &index);
    void filterChanged(const QString &filter);
public:
    void showView(QPoint *pos = 0);
protected:
    virtual bool eventFilter(QObject *o, QEvent *e);
protected:
    LiteApi::IApplication *m_liteApp;
    Utils::FilterLineEdit *m_edit;
    QTreeView   *m_view;
    QVBoxLayout *m_layout;
    QScopedPointer<QToolBar>  m_tmpToolBar;
    bool         m_wrap;
};

#endif // QUICKOPENWIDGET_H
